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

#ifndef RERROR_H_INCLUDED
#define RERROR_H_INCLUDED
#include <cstdlib>
#include <iostream>

#define RENDERER_ERROR_MALLOC 0
#define RENDERER_ERROR_MATH 1
#define RENDERER_ERROR_IO 2
#define RENDERER_ERROR_PARAM 3
#define RENDERER_ERROR_SDL 4
#define RENDERER_ERROR_GL 5
#define RENDERER_ERROR_ARB 6
#define RENDERER_ERROR_PNG 7
#define RENDERER_ERROR_UNKNOWN 8


#define renderer_error(type) _renderer_error(type, __LINE__, __FILE__)
void _renderer_error(int type, const int lnum, const char *fname);

#endif // RERROR_H_INCLUDED
