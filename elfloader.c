/* Copyright © 2014, Owen Shepherd
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted without restriction.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include "elfload.h"
#include "zlib.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "png.h"

FILE *f;
void *buf;


void print_line(const char* text_1, const char* text_2)
{
    printf("%s %s\n", text_1, text_2);
}

void print_line_double(const char* text_1, double time)
{
    printf("%s %f\n", text_1, time);
}
typedef struct {
//Аргументы командной строки добавим позже
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

ProgramArgs arguments;

typedef void (*entrypoint_t)(ProgramArgs*);


static bool fpread(el_ctx *ctx, void *dest, size_t nb, size_t offset)
{
    (void) ctx;

    if (fseek(f, offset, SEEK_SET))
        return false;

    if (fread(dest, nb, 1, f) != 1)
        return false;

    return true;
}

static void *alloccb(
    el_ctx *ctx,
    Elf_Addr phys,
    Elf_Addr virt,
    Elf_Addr size)
{
    (void) ctx;
    (void) phys;
    (void) size;
    return (void*) virt;
}

static void check(el_status stat, const char* expln)
{
    if (stat) {
        fprintf(stderr, "%s: error %d\n", expln, stat);
        exit(1);
    }
}

static void go(entrypoint_t ep) {
    ep(&arguments);
}

int main(int argc, char **argv)
{
    clock_t start_time, end_time;
    start_time = clock();
    if (argc != 5) {
        fprintf(stderr, "usage: %s [elf-to-load]\n", argv[0]);
        return 1;
    }

    f = fopen(argv[1], "rb");
    if (!f) {
        perror("opening file");
        return 1;
    }
    printf("%s %s %s\n", argv[2], argv[3], argv[4]);
    arguments.name_of_file_text = argv[2];
    arguments.text=argv[3];
    arguments.name_of_output_file=argv[4];
    arguments.malloc = malloc;
    arguments.free = free;
    arguments.fopen = fopen;
    arguments.fclose = fclose;

    arguments.print_line = print_line;
    arguments.print_line_double = print_line_double;
    arguments.clock=clock;
    arguments.FT_Init_FreeType = FT_Init_FreeType;
    arguments.FT_New_Face = FT_New_Face;
    arguments.FT_Set_Pixel_Sizes = FT_Set_Pixel_Sizes;
    arguments.FT_Get_Char_Index = FT_Get_Char_Index;
    arguments.FT_Load_Glyph = FT_Load_Glyph;
    arguments.FT_Render_Glyph = FT_Render_Glyph;

    arguments.png_create_write_struct = png_create_write_struct;
    arguments.png_create_info_struct = png_create_info_struct;
    arguments.png_destroy_write_struct = png_destroy_write_struct;
    arguments.png_init_io = png_init_io;
    arguments.png_set_IHDR = png_set_IHDR;
    arguments.png_write_info = png_write_info;
    arguments.png_write_image = png_write_image;
    arguments.png_write_end = png_write_end;
    arguments.png_free = png_free;
    arguments.png_malloc = png_malloc;

    el_ctx ctx;
    ctx.pread = fpread;

    check(el_init(&ctx), "initialising");

    if (posix_memalign(&buf, ctx.align, ctx.memsz)) {
        perror("memalign");
        return 1;
    }

    if (mprotect(buf, ctx.memsz, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        perror("mprotect");
        return 1;
    }

    ctx.base_load_vaddr = ctx.base_load_paddr = (uintptr_t) buf;

    check(el_load(&ctx, alloccb), "loading");
    check(el_relocate(&ctx), "relocating");

    uintptr_t epaddr = ctx.ehdr.e_entry + (uintptr_t) buf;

    entrypoint_t ep = (entrypoint_t) epaddr;

    printf("Binary entrypoint is %" PRIxPTR "; invoking %p\n", (uintptr_t) ctx.ehdr.e_entry, ep);

    go(ep);

    fclose(f);

    free(buf);
    end_time = clock();
    printf("Total time: %f\n", (double)(end_time - start_time));
    return 0;
}
