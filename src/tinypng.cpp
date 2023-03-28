/*  Structured-Light Simulator
    Copyright (C) 2014-2015 Sk. Mohammadul Haque

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

#include "../include/rcommon.h"
#include "../include/tinypng.h"

int __tiny_png_check_endian()
{
    unsigned int i = 1;
    char *c = (char*)&i;
    if(*c) return TINY_PNG_LITTLE_ENDIAN;
    return TINY_PNG_BIG_ENDIAN;
}

int __tiny_png_setjmp(png_structp pptr)
{
    return setjmp(png_jmpbuf(pptr));
}

void tiny_png_save(const char* fname, int width, int height, int type, void* pixels, int order)
{
    DEBUG_STDOUT("TINY_PNG_SAVE\n")
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte color_type;
    png_byte bit_depth;
    int pitch, i;
    png_bytep* row_pointers;
    FILE *fp;

    if((fp = fopen(fname, "wb"))==NULL) renderer_error(RENDERER_ERROR_PNG);
    if((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))==NULL)
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }
    if((info_ptr = png_create_info_struct(png_ptr))==NULL)
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }
    if(__tiny_png_setjmp(png_ptr))
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }

    png_init_io(png_ptr, fp);

    switch(type)
    {
    case TINY_PNG_RGB:
        color_type = PNG_COLOR_TYPE_RGB;
        bit_depth = 8;
        pitch = 3*width;
        break;
    case TINY_PNG_RGB16:
        color_type = PNG_COLOR_TYPE_RGB;
        bit_depth = 16;
        pitch = 6*width;
        break;
    case TINY_PNG_RGBA:
        color_type = PNG_COLOR_TYPE_RGBA;
        bit_depth = 8;
        pitch = 4*width;
        break;
    case TINY_PNG_RGBA16:
        color_type = PNG_COLOR_TYPE_RGBA;
        bit_depth = 16;
        pitch = 8*width;
        break;
    case TINY_PNG_GRAY:
        color_type = PNG_COLOR_TYPE_GRAY;
        bit_depth = 8;
        pitch = width;
        break;
    case TINY_PNG_GRAY16:
        color_type = PNG_COLOR_TYPE_GRAY;
        bit_depth = 16;
        pitch = 2*width;
        break;
    default:
        renderer_error(RENDERER_ERROR_PNG);
        color_type = PNG_COLOR_TYPE_RGB;
        bit_depth = 8;
        pitch = 3*width;
    }
    if(__tiny_png_setjmp(png_ptr))
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);

    if(__tiny_png_check_endian()==TINY_PNG_LITTLE_ENDIAN) png_set_swap(png_ptr);
    if(__tiny_png_setjmp(png_ptr))
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }
    if((row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*height))==NULL)
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }
    if(order==TINY_PNG_ORDER_DN) for(i=0; i<height; ++i) row_pointers[i] = (png_bytep)(uint8_t*)pixels+i*pitch;
    else for(i=0; i<height; ++i) row_pointers[height-i-1] = (png_bytep)(uint8_t*)pixels+i*pitch;
    png_write_image(png_ptr, row_pointers);
    free(row_pointers);
    if(__tiny_png_setjmp(png_ptr))
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }
    png_write_end(png_ptr, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}


void tiny_png_load(const char *fname, int* width, int* height, int* type, void** pixels, int order)
{
    DEBUG_STDOUT("TINY_PNG_LOAD\n")
    FILE* fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int i, pitch;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep* row_pointers;

    if((fp = fopen(fname, "rb"))==NULL) renderer_error(RENDERER_ERROR_PNG);
    if((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))==NULL)
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }
    if((info_ptr = png_create_info_struct(png_ptr))==NULL)
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }

    if(__tiny_png_setjmp(png_ptr))
    {
        #if !defined(__linux__) || !defined(__linux)
        //png_read_destroy(png_ptr, info_ptr, (png_info *)0);
        #endif
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    *width = png_get_image_width(png_ptr, info_ptr);
    *height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if(color_type==PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    #if !defined(__linux__) || !defined(__linux)
    //if(info_ptr->bit_depth<8) png_set_packing(png_ptr);
    #endif
    if(__tiny_png_check_endian()==TINY_PNG_LITTLE_ENDIAN) png_set_swap(png_ptr);

    png_start_read_image(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if((row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*(*height)))==NULL)
    {
        fclose(fp);
        renderer_error(RENDERER_ERROR_PNG);
    }
    pitch = png_get_rowbytes(png_ptr,info_ptr);
    switch(color_type)
    {
    case PNG_COLOR_TYPE_GRAY:
        if(bit_depth==8) *type = TINY_PNG_GRAY;
        else if(bit_depth==16) *type = TINY_PNG_GRAY16;
        break;
    case PNG_COLOR_TYPE_RGB:
        if(bit_depth==8) *type = TINY_PNG_RGB;
        else if(bit_depth==16) *type = TINY_PNG_RGB16;
        break;
    case PNG_COLOR_TYPE_RGBA:
        if(bit_depth==8) *type = TINY_PNG_RGBA;
        else if(bit_depth==16) *type = TINY_PNG_RGBA16;
        break;
    default:
        renderer_error(RENDERER_ERROR_PNG);
    }

    if((row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*(*height)))==NULL) renderer_error(RENDERER_ERROR_PNG);
    if((*pixels = malloc(pitch*(*height)))==NULL) renderer_error(RENDERER_ERROR_PNG);
    if(order==TINY_PNG_ORDER_DN) for(i=0; i<(*height); ++i) row_pointers[i] = (png_bytep)(uint8_t*)(*pixels)+i*pitch;
    else for(i=0; i<(*height); ++i) row_pointers[(*height)-i-1] = (png_bytep)(uint8_t*)(*pixels)+i*pitch;
    png_read_image(png_ptr, row_pointers);
    #if !defined(__linux__) || !defined(__linux)
    //png_read_destroy(png_ptr, info_ptr, (png_infop)0);
    #endif
    free(png_ptr);
    free(row_pointers);
    free(info_ptr);
    fclose(fp);
}



