#include "funcs.h"
#ifdef BLOB_BUILD
ProgramArgs arguments;
#else
FT_Init_FreeType_fn My_FT_Init_FreeType = NULL;
FT_New_Face_fn My_FT_New_Face = NULL;
FT_Set_Pixel_Sizes_fn My_FT_Set_Pixel_Sizes = NULL;
FT_Get_Char_Index_fn My_FT_Get_Char_Index = NULL;
FT_Load_Glyph_fn My_FT_Load_Glyph = NULL;
FT_Render_Glyph_fn My_FT_Render_Glyph = NULL;

png_create_write_struct_fn My_png_create_write_struct = NULL;
png_create_info_struct_fn My_png_create_info_struct = NULL;
png_destroy_write_struct_fn My_png_destroy_write_struct = NULL;
png_init_io_fn My_png_init_io = NULL;
png_set_IHDR_fn My_png_set_IHDR = NULL;
png_write_info_fn My_png_write_info = NULL;
png_write_image_fn My_png_write_image = NULL;
png_write_end_fn My_png_write_end = NULL;
png_free_fn My_png_free = NULL;
png_malloc_fn My_png_malloc = NULL;
#endif
void init_libs()
{
    //Инициализация библиотек в зависимости от типа сборки: статическая или динамическая
    #ifdef STATIC_BUILD
    My_FT_Init_FreeType = FT_Init_FreeType;
    My_FT_New_Face = FT_New_Face;
    My_FT_Set_Pixel_Sizes = FT_Set_Pixel_Sizes;
    My_FT_Get_Char_Index = FT_Get_Char_Index;
    My_FT_Load_Glyph = FT_Load_Glyph;
    My_FT_Render_Glyph = FT_Render_Glyph;

    My_png_create_write_struct = png_create_write_struct;
    My_png_create_info_struct = png_create_info_struct;
    My_png_destroy_write_struct = png_destroy_write_struct;
    My_png_init_io = png_init_io;
    My_png_set_IHDR = png_set_IHDR;
    My_png_write_info = png_write_info;
    My_png_write_image = png_write_image;
    My_png_write_end = png_write_end;
    My_png_free = png_free;
    My_png_malloc = png_malloc;
    #elif DYNAMIC_BUILD
    zlib_handle = dlopen(DYN_FREETYPE, RTLD_LAZY);
    png_lib_handle = dlopen(DYN_LIBPNG, RTLD_LAZY);
    freetype_handle = dlopen(DYN_FREETYPE, RTLD_LAZY);
 
    My_FT_Init_FreeType = dlsym(freetype_handle, "FT_Init_FreeType");
    My_FT_New_Face = dlsym(freetype_handle,"FT_New_Face");
    My_FT_Set_Pixel_Sizes = dlsym(freetype_handle,"FT_Set_Pixel_Sizes");
    My_FT_Get_Char_Index = dlsym(freetype_handle,"FT_Get_Char_Index");
    My_FT_Load_Glyph = dlsym(freetype_handle,"FT_Load_Glyph");
    My_FT_Render_Glyph = dlsym(freetype_handle,"FT_Render_Glyph");

    My_png_create_write_struct = dlsym(png_lib_handle,"png_create_write_struct");
    My_png_create_info_struct = dlsym(png_lib_handle,"png_create_info_struct");
    My_png_destroy_write_struct = dlsym(png_lib_handle,"png_destroy_write_struct");
    My_png_init_io = dlsym(png_lib_handle,"png_init_io");
    My_png_set_IHDR = dlsym(png_lib_handle,"png_set_IHDR");
    My_png_write_info = dlsym(png_lib_handle,"png_write_info");
    My_png_write_image = dlsym(png_lib_handle,"png_write_image");
    My_png_write_end = dlsym(png_lib_handle,"png_write_end");
    My_png_free = dlsym(png_lib_handle,"png_free");
    My_png_malloc = dlsym(png_lib_handle,"png_malloc");
    #endif
}

void deinit_libs()
{
#ifdef DYNAMIC_BUILD
    if (freetype_handle) dlclose(freetype_handle);
    if (png_lib_handle) dlclose(png_lib_handle);
    if (zlib_handle) dlclose(zlib_handle);
#endif
}

void main_logic(const char* name_of_file_text, const char* text, const char* name_of_output_file)
{
    
    init_libs();
    printf("BUILD MODE: %s", BUILD_MODE);
    //Инициализация библиотеки шрифтов
    FT_Library ft;
    FT_Error err = My_FT_Init_FreeType(&ft);
    FT_Face face;
    err = My_FT_New_Face(ft, name_of_file_text, 0, &face);
    My_FT_Set_Pixel_Sizes(face, 0, 128);
    FT_Int32 load_flags = FT_LOAD_DEFAULT;
    FT_Int32 render_flags = FT_RENDER_MODE_NORMAL;
    int cur_char = 0;
    if(err != 0)
    {
        return;
    }
    //Инициализация буффера под картинку
    for(cur_char = 0; text[cur_char] != '\0'; cur_char++);
    int width = 64 + 64 + cur_char*128;
    int height = 64 + 64 + 128;
    char* image = malloc(width*height*4);
    for(int i = 0; i < width*height*4; i++)
    {
        image[i] = 255; //4 по 255==белый
    }
    cur_char = 0;
    //Запись текста в буффер
    while(text[cur_char] != '\0')
    {
        FT_UInt glyph_index = My_FT_Get_Char_Index(face, text[cur_char]);
        //Находим в файле с шрифтом символ и рендерим его
        err = My_FT_Load_Glyph(face, glyph_index, load_flags);
        err = My_FT_Render_Glyph(face->glyph, render_flags);
        //Заносим символ в картинку
        for (size_t i = 0; i < face->glyph->bitmap.rows; i++) 
        {
            for (size_t j = 0; j < face->glyph->bitmap.width; j++) {
                unsigned char pixel_brightness = face->glyph->bitmap.buffer[i * face->glyph->bitmap.pitch + j];
                pixel_brightness = 255-pixel_brightness;
                int cur_pos = ((i+64)*width + 64 + cur_char*128+j)*4;//i+64, потому что
                //смещение на 64 пикселя
                image[cur_pos] = pixel_brightness;
                image[cur_pos+1] = pixel_brightness;
                image[cur_pos+2] = pixel_brightness;
            } 
        }
        cur_char++;
    }
    //Записываем картинку в png файл
    FILE* f = fopen(name_of_output_file, "wb");

    png_structp png_ptr = My_png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = My_png_create_info_struct(png_ptr);

    My_png_init_io(png_ptr, f);
    My_png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, // 8 бит на канал
                 PNG_COLOR_TYPE_RGBA, // RGBA формат
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_bytepp row_pointers = (png_bytepp)My_png_malloc(png_ptr, height * sizeof(png_bytep));
    
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte *)(image + (y * width * 4));
    }
    My_png_write_info(png_ptr, info_ptr);
    My_png_write_image(png_ptr, row_pointers);
    My_png_write_end(png_ptr, NULL);
   
    // Освобождаем ресурсы
    My_png_free(png_ptr, row_pointers);
    My_png_destroy_write_struct(&png_ptr, &info_ptr);
    free(image);
    fclose(f);

    deinit_libs();
}

