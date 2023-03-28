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

#ifndef RRENDER_H_INCLUDED
#define RRENDER_H_INCLUDED
#define GLEW_STATIC
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <iostream>
#include "rio.h"
#include "tinypng.h"

#define R_NEAR_PLANE (0.01)
#define R_FAR_PLANE (100.0)

extern SDL_Window* gwin;
extern SDL_GLContext gcontxt;
extern ALLPARAMS prms;

void renderer_init();
void __renderer_initGL();
void renderer_proj_intrinsics(double nclip, double fclip);
void renderer_render();
void renderer_init_all(const char *fname);
void renderer_copy_img_data(int i);
void renderer_copy_depth_data(int i);
void renderer_close();

#endif // RRENDER_H_INCLUDED
