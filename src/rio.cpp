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
#include "../include/rio.h"

void matRT::computeRT()
{
    DEBUG_STDOUT("COMPUTERT\n")
    RT[0] = R[0];
    RT[1] = R[1];
    RT[2] = R[2];
    RT[3] = 0.0;
    RT[4] = R[3];
    RT[5] = R[4];
    RT[6] = R[5];
    RT[7] = 0.0;
    RT[8] = R[6];
    RT[9] = R[7];
    RT[10] = R[8];
    RT[11] = 0.0;
    RT[12] = T[0];
    RT[13] = T[1];
    RT[14] = T[2];
    RT[15] = 1.0;
}

void matRT::computeiRT()
{
    DEBUG_STDOUT("COMPUTEIRT\n")
    double tmp[16];
    tmp[0] = R[0];
    tmp[1] = R[1];
    tmp[2] = R[2];
    tmp[3] = 0.0;
    tmp[4] = R[3];
    tmp[5] = R[4];
    tmp[6] = R[5];
    tmp[7] = 0.0;
    tmp[8] = R[6];
    tmp[9] = R[7];
    tmp[10] = R[8];
    tmp[11] = 0.0;
    tmp[12] = T[0];
    tmp[13] = T[1];
    tmp[14] = T[2];
    tmp[15] = 1.0;
    renderer_mat4_inv(tmp, RT);
}

allparams::allparams()
{
    DEBUG_STDOUT("ALLPARAMS\n")
    m = NULL;
    point_size = 0.0;
    disable_lighting = 0;
}

allparams::~allparams()
{
    DEBUG_STDOUT("~ALLPARAMS\n")
    if(m!=NULL) mesh_free_mesh(m);
}

MESH renderer_load_file(const char* fname)
{
    DEBUG_STDOUT("RENDERER_LOAD_FILE\n")
    MESH m = mesh_load_file(fname);
    if(m==NULL) renderer_error(RENDERER_ERROR_IO);
    if(m->is_loaded==0||m->num_vertices==0)//||m->num_faces==0)
    {
        mesh_free_mesh(m);
        renderer_error(RENDERER_ERROR_IO);
        return NULL;
    }
    return m;
}

ALLPARAMS renderer_load_params(const char* fname)
{
    DEBUG_STDOUT("RENDERER_LOAD_PARAMS\n")
    ALLPARAMS prms = NULL;
    std::ifstream fs;
    prms = new allparams;
    fs.open(fname, std::fstream::in|std::ios::binary);
    if(!fs.is_open()) renderer_error(RENDERER_ERROR_IO);
    if(!(fs.getline(prms->model_fname, 256))) renderer_error(RENDERER_ERROR_IO);
    if(!(fs.getline(prms->ofname, 256))) renderer_error(RENDERER_ERROR_IO);

    if(!(fs>>prms->imsize[0]>>prms->imsize[1])) renderer_error(RENDERER_ERROR_IO);
    if(!(fs>>prms->outbpp)) renderer_error(RENDERER_ERROR_IO);
    if(prms->outbpp==0) prms->outbpp = GL_RGBA;
    else prms->outbpp = GL_RGBA16;
    if(!(fs>>prms->K[0]>>prms->K[3]>>prms->K[6]
            >>prms->K[1]>>prms->K[4]>>prms->K[7]
            >>prms->K[2]>>prms->K[5]>>prms->K[8])) renderer_error(RENDERER_ERROR_IO);

    if(!(fs>>prms->N)) renderer_error(RENDERER_ERROR_IO);
    prms->projRT.resize(prms->N);
    for(int i=0; i<prms->N; ++i)
    {
        if(!(fs>>prms->projRT[i].R[0]>>prms->projRT[i].R[3]>>prms->projRT[i].R[6]
                >>prms->projRT[i].R[1]>>prms->projRT[i].R[4]>>prms->projRT[i].R[7]
                >>prms->projRT[i].R[2]>>prms->projRT[i].R[5]>>prms->projRT[i].R[8])) renderer_error(RENDERER_ERROR_IO);
        if(!(fs>>prms->projRT[i].T[0]>>prms->projRT[i].T[1]>>prms->projRT[i].T[2])) renderer_error(RENDERER_ERROR_IO);
        prms->projRT[i].computeiRT();
    }

    if(!(fs>>prms->sceneRT.R[0]>>prms->sceneRT.R[3]>>prms->sceneRT.R[6]
            >>prms->sceneRT.R[1]>>prms->sceneRT.R[4]>>prms->sceneRT.R[7]
            >>prms->sceneRT.R[2]>>prms->sceneRT.R[5]>>prms->sceneRT.R[8])) renderer_error(RENDERER_ERROR_IO);
    if(!(fs>>prms->sceneRT.T[0]>>prms->sceneRT.T[1]>>prms->sceneRT.T[2])) renderer_error(RENDERER_ERROR_IO);

    if(!(fs>>prms->projpattype)) renderer_error(RENDERER_ERROR_IO);
    if(prms->projpattype<0 ||prms->projpattype>1) renderer_error(RENDERER_ERROR_IO);
    if(prms->projpattype>0)
    {
        if(!(fs>>prms->ipfname)) renderer_error(RENDERER_ERROR_IO);
    }
    else prms->ipfname[0] = '\0';
    if(!(fs>>prms->light_diffuse[0]>>prms->light_diffuse[1]>>prms->light_diffuse[2])) renderer_error(RENDERER_ERROR_IO);
    prms->light_diffuse[3] = 1.0;
    if(!(fs>>prms->scenenoisetype>>prms->scenenoisesigma)) renderer_error(RENDERER_ERROR_IO);
    if(fs.peek()!=EOF)
    {
        if(!(fs>>prms->point_size)) renderer_error(RENDERER_ERROR_IO);
        if(!(fs>>prms->disable_lighting)) renderer_error(RENDERER_ERROR_IO);
    }
    fs.close();

    prms->sceneRT.computeRT();
    return prms;
}


