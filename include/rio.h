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

#ifndef RIO_H_INCLUDED
#define RIO_H_INCLUDED
#include <meshlib.h>
#include <fstream>
#include <vector>
#include <GL/gl.h>
#include "rerror.h"
#include "rmath.h"
#include "tinypng.h"

MESH renderer_load_file(const char* fname);

struct matRT
{
    mat3 R;
    mat3 T;
    mat4 RT;
    void computeRT();
    void computeiRT();
};
typedef matRT* MATRT;

struct allparams
{
    char model_fname[512];
    char ofname[512];
    char ipfname[512];
    int imsize[2];
    mat3 K;
    std::vector<matRT> projRT;
    matRT sceneRT;
    mat4 frustum;
    GLfloat light_diffuse[4];
    int N;
    int outbpp;
    int scenenoisetype;
    double scenenoisesigma;
    int projpattype;
    MESH m;
    float point_size;
    int disable_lighting;
    allparams();
    ~allparams();
};
typedef allparams* ALLPARAMS;

ALLPARAMS renderer_load_params(const char* fname);


#endif // RIO_H_INCLUDED
