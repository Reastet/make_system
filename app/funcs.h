#ifndef FUNCS_H
#define FUNCS_H

#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include "zlib.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "png.h"
// 
void main_logic(const char* name_of_file_text, const char* text, const char* name_of_output_file);
void init_libs();
void deinit_libs();
//Контрольные переменные, чтобы убедиться в корректности сборки
#ifdef STATIC_BUILD
#define BUILD_MODE "STATIC\n"
#elif DYNAMIC_BUILD
#define BUILD_MODE "DYNAMIC\n"
#include "dlfcn.h"
#elif BLOB_BUILD
#define BUILD_MODE "BLOB\n"
#else
#define BUILD_MODE "SMTH_ELSE\n"
#endif

#ifdef BLOB_BUILD
typedef struct {

const char* name_of_file_text;
const char* text;
const char* name_of_output_file;
void *(*malloc)(size_t);
void (*free)(void*);
FILE *(*fopen)(const char *, const char *);
int (*fclose)(FILE *);

void (*print_line)(const char*, const char*);
void (*print_line_double)(const char*, double);

clock_t (*clock)(void);

FT_Error (*FT_Init_FreeType)(FT_Library *alibrary);
FT_Error (*FT_New_Face)(FT_Library library, const char *filepathname, FT_Long face_index, FT_Face *aface);
FT_Error (*FT_Set_Pixel_Sizes)(FT_Face face, FT_UInt pixel_width, FT_UInt pixel_height);
FT_UInt (*FT_Get_Char_Index)(FT_Face face, FT_ULong charcode);
FT_Error (*FT_Load_Glyph)(FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags);
FT_Error (*FT_Render_Glyph)(FT_GlyphSlot slot, FT_Render_Mode render_mode);

png_structp (*png_create_write_struct)(png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
png_infop (*png_create_info_struct)(png_const_structrp png_ptr);
void (*png_destroy_write_struct)(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr);
void (*png_init_io)(png_structrp png_ptr, png_FILE_p fp);
void (*png_set_IHDR)(png_const_structrp png_ptr, png_inforp info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth, int color_type, int interlace_method, int compression_method, int filter_method);
void (*png_write_info)(png_structrp png_ptr, png_const_inforp info_ptr);
void (*png_write_image)(png_structrp png_ptr, png_bytepp image);
void (*png_write_end)(png_structrp png_ptr, png_inforp info_ptr);
void (*png_free)(png_const_structrp png_ptr, png_voidp ptr);
png_voidp (*png_malloc)(png_const_structrp png_ptr, png_alloc_size_t size);


} ProgramArgs;

extern ProgramArgs arguments;




#define malloc arguments.malloc
#define free arguments.free
#define fopen arguments.fopen
#define fclose arguments.fclose 

#define printf arguments.print_line
#define print_line_double arguments.print_line_double
#define clock arguments.clock

#define My_FT_Init_FreeType arguments.FT_Init_FreeType
#define My_FT_New_Face arguments.FT_New_Face
#define My_FT_Set_Pixel_Sizes arguments.FT_Set_Pixel_Sizes
#define My_FT_Get_Char_Index arguments.FT_Get_Char_Index
#define My_FT_Load_Glyph arguments.FT_Load_Glyph
#define My_FT_Render_Glyph arguments.FT_Render_Glyph

#define My_png_create_write_struct arguments.png_create_write_struct
#define My_png_create_info_struct arguments.png_create_info_struct
#define My_png_destroy_write_struct arguments.png_destroy_write_struct
#define My_png_init_io arguments.png_init_io
#define My_png_set_IHDR arguments.png_set_IHDR
#define My_png_write_info arguments.png_write_info
#define My_png_write_image arguments.png_write_image
#define My_png_write_end arguments.png_write_end
#define My_png_free arguments.png_free
#define My_png_malloc arguments.png_malloc
#else
void* zlib_handle;
void* png_lib_handle;
void* freetype_handle;



//Указатели для корректной работы с функциями
typedef FT_Error (*FT_Init_FreeType_fn)(FT_Library *alibrary);
typedef FT_Error (*FT_New_Face_fn)(FT_Library library, const char *filepathname, FT_Long face_index, FT_Face *aface);
typedef FT_Error (*FT_Set_Pixel_Sizes_fn)(FT_Face face, FT_UInt pixel_width, FT_UInt pixel_height);
typedef FT_UInt (*FT_Get_Char_Index_fn)(FT_Face face, FT_ULong charcode);
typedef FT_Error (*FT_Load_Glyph_fn)(FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags);
typedef FT_Error (*FT_Render_Glyph_fn)(FT_GlyphSlot slot, FT_Render_Mode render_mode);

typedef png_structp (*png_create_write_struct_fn)(png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
typedef png_infop (*png_create_info_struct_fn)(png_const_structrp png_ptr);
typedef void (*png_destroy_write_struct_fn)(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr);
typedef void (*png_init_io_fn)(png_structrp png_ptr, png_FILE_p fp);
typedef void (*png_set_IHDR_fn)(png_const_structrp png_ptr, png_inforp info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth, int color_type, int interlace_method, int compression_method, int filter_method);
typedef void (*png_write_info_fn)(png_structrp png_ptr, png_const_inforp info_ptr);
typedef void (*png_write_image_fn)(png_structrp png_ptr, png_bytepp image);
typedef void (*png_write_end_fn)(png_structrp png_ptr, png_inforp info_ptr);
typedef void (*png_free_fn)(png_const_structrp png_ptr, png_voidp ptr);
typedef png_voidp (*png_malloc_fn)(png_const_structrp png_ptr, png_alloc_size_t size);



extern FT_Init_FreeType_fn My_FT_Init_FreeType;
extern FT_New_Face_fn My_FT_New_Face;
extern FT_Set_Pixel_Sizes_fn My_FT_Set_Pixel_Sizes;
extern FT_Get_Char_Index_fn My_FT_Get_Char_Index;
extern FT_Load_Glyph_fn My_FT_Load_Glyph;
extern FT_Render_Glyph_fn My_FT_Render_Glyph;

extern png_create_write_struct_fn My_png_create_write_struct;
extern png_create_info_struct_fn My_png_create_info_struct;
extern png_destroy_write_struct_fn My_png_destroy_write_struct;
extern png_init_io_fn My_png_init_io;
extern png_set_IHDR_fn My_png_set_IHDR;
extern png_write_info_fn My_png_write_info;
extern png_write_image_fn My_png_write_image;
extern png_write_end_fn My_png_write_end;
extern png_free_fn My_png_free;
extern png_malloc_fn My_png_malloc;
#endif
#endif 