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

#include <malloc.h>
#include <stdint.h>
#include <png.h>
#include "rerror.h"

#define TINY_PNG_RGB 0
#define TINY_PNG_RGB16 1
#define TINY_PNG_RGBA 2
#define TINY_PNG_RGBA16 3
#define TINY_PNG_GRAY 4
#define TINY_PNG_GRAY16 5

#define TINY_PNG_ORDER_DN 0
#define TINY_PNG_ORDER_UP 1

#define TINY_PNG_LITTLE_ENDIAN 0
#define TINY_PNG_BIG_ENDIAN 1


int __tiny_png_check_endian();
int __tiny_png_setjmp(png_structp pptr);
void tiny_png_load(const char* fname, int* width, int* height, int* type, void** pixels, int order);
void tiny_png_save(const char* fname, int width, int height, int type, void* pixels, int order);
