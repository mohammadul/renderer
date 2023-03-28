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

#ifndef RMATH_H_INCLUDED
#define RMATH_H_INCLUDED
#include <iostream>
#include "rerror.h"

typedef double mat3[9];
typedef double mat4[16];

void renderer_mat4_mul(mat4 a, mat4 b, mat4 c);
void renderer_mat4_transpose(mat4 a);
void renderer_mat4_disp(mat4 a);
void renderer_mat4_inv(mat4 a, mat4 b);

#endif // RMATH_H_INCLUDED
