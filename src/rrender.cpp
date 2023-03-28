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
#include "../include/rrender.h"
#include <cstring>

SDL_Window* gwin = NULL;
SDL_GLContext gcontxt;
ALLPARAMS prms = NULL;
SDL_Surface* pat = NULL;
GLuint fbo[2], sptex_buf[2], ptex_buf, render_buf, drender_buf, dlist;
int num_textures;
void* patpixels;

void renderer_init()
{
    DEBUG_STDOUT("RENDERER_INIT\n")
    if(SDL_Init(SDL_INIT_VIDEO)<0) renderer_error(RENDERER_ERROR_SDL);
    else
    {
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        glEnable(GL_MULTISAMPLE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        gwin = SDL_CreateWindow("RENDERER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN);
        if(gwin==NULL) renderer_error(RENDERER_ERROR_SDL);
        else
        {
            SDL_HideWindow(gwin);
            gcontxt = SDL_GL_CreateContext(gwin);
            if(gcontxt==NULL) renderer_error(RENDERER_ERROR_SDL);
            else
            {
                if(SDL_GL_SetSwapInterval(1)<0) renderer_error(RENDERER_ERROR_SDL);
                __renderer_initGL();
            }
        }
    }
}

void __renderer_initGL()
{
    DEBUG_STDOUT("__RENDERER_INITGL\n")
    glewExperimental = true;
    glewInit();
    if(!GL_ARB_depth_texture||!GL_ARB_shadow) renderer_error(RENDERER_ERROR_ARB);
    glViewport(0, 0, prms->imsize[1], prms->imsize[0]);

    dlist = glGenLists(1);
    if(prms->point_size>0) glPointSize(prms->point_size);
    if(prms->disable_lighting==1) glDisable(GL_LIGHTING);
    glNewList(dlist, GL_COMPILE);
    if(prms->m->is_faces && prms->m->num_faces>0) mesh_draw_mesh(prms->m);
    else mesh_draw_point_cloud(prms->m);
    glEndList();

    if(glGenFramebuffers==nullptr)
    {
        renderer_error(RENDERER_ERROR_GL);
    }
    glGenFramebuffers(2, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
    glViewport(0, 0, prms->imsize[1], prms->imsize[0]);

    glGenTextures(num_textures, sptex_buf);

    glBindTexture(GL_TEXTURE_2D, sptex_buf[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, prms->imsize[1], prms->imsize[0], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sptex_buf[0], 0);
    glDrawBuffer(GL_NONE);

    if(prms->projpattype>0)
    {
        glBindTexture(GL_TEXTURE_2D, sptex_buf[1]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        char *ext = strrchr(prms->ipfname, '.');
        if(strcmp(ext,".bmp")==0)
        {
            int i, hd2;
            void* temp_row = NULL;
            if((pat = SDL_LoadBMP(prms->ipfname))==NULL) renderer_error(RENDERER_ERROR_IO);
            if(pat->w!=prms->imsize[1]||pat->h!=prms->imsize[0]) renderer_error(RENDERER_ERROR_IO);
            if((temp_row = (void *)malloc(pat->pitch))==NULL) renderer_error(RENDERER_ERROR_MALLOC);
            hd2 = (int)(pat->h*.5);
            for(i=0; i<hd2; ++i)
            {
                memcpy((Uint8 *)temp_row,(Uint8 *)(pat->pixels)+pat->pitch*i, pat->pitch);
                memcpy((Uint8 *)(pat->pixels)+pat->pitch*i, (Uint8 *)(pat->pixels)+pat->pitch*(pat->h-i-1), pat->pitch);
                memcpy((Uint8 *)(pat->pixels)+pat->pitch*(pat->h-i-1), temp_row, pat->pitch);
            }
            free(temp_row);
            if(pat->format->BitsPerPixel==8)
            {
                pat = SDL_ConvertSurfaceFormat(pat, SDL_PIXELFORMAT_BGR24, 0);
                gluBuild2DMipmaps(GL_TEXTURE_2D, 4, prms->imsize[1], prms->imsize[0], GL_BGR_EXT, GL_UNSIGNED_BYTE, pat->pixels);
            }
            else if(pat->format->BitsPerPixel==24)
            {
                gluBuild2DMipmaps(GL_TEXTURE_2D, 4, prms->imsize[1], prms->imsize[0], GL_BGR_EXT, GL_UNSIGNED_BYTE, pat->pixels);
            }
            else renderer_error(RENDERER_ERROR_GL);
            SDL_FreeSurface(pat);
    }
        else if(strcmp(ext,".png")==0)
        {
            int width, height, type;
            tiny_png_load(prms->ipfname, &width, &height, &type, &patpixels, TINY_PNG_ORDER_UP);
            if(width!=prms->imsize[1]||height!=prms->imsize[0]) renderer_error(RENDERER_ERROR_IO);
            switch(type)
            {
            case TINY_PNG_GRAY:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 4, prms->imsize[1], prms->imsize[0], GL_INTENSITY, GL_UNSIGNED_BYTE, patpixels);
                break;
            case TINY_PNG_GRAY16:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 4, prms->imsize[1], prms->imsize[0], GL_INTENSITY, GL_UNSIGNED_SHORT, patpixels);
                break;
            case TINY_PNG_RGB:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 4, prms->imsize[1], prms->imsize[0], GL_RGB, GL_UNSIGNED_BYTE, patpixels);
                break;
            case TINY_PNG_RGB16:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 4, prms->imsize[1], prms->imsize[0], GL_RGB, GL_UNSIGNED_SHORT, patpixels);
                break;
            case TINY_PNG_RGBA:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 4, prms->imsize[1], prms->imsize[0], GL_RGBA, GL_UNSIGNED_BYTE, patpixels);
                break;
            case TINY_PNG_RGBA16:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 4, prms->imsize[1], prms->imsize[0], GL_RGBA, GL_UNSIGNED_SHORT, patpixels);
                break;
            default:
                renderer_error(RENDERER_ERROR_IO);
            }
            free(patpixels);
        }
        else renderer_error(RENDERER_ERROR_IO);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindTexture(GL_TEXTURE_2D, sptex_buf[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER,fbo[1]);
    glViewport(0, 0, prms->imsize[1], prms->imsize[0]);
    glGenRenderbuffers(1,&render_buf);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buf);
    glRenderbufferStorage(GL_RENDERBUFFER, prms->outbpp, prms->imsize[1], prms->imsize[0]);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);

    glGenRenderbuffers(1, &drender_buf);
    glBindRenderbuffer(GL_RENDERBUFFER, drender_buf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, prms->imsize[1], prms->imsize[0]);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, drender_buf);

    GLenum sts = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(sts!=GL_FRAMEBUFFER_COMPLETE) renderer_error(RENDERER_ERROR_GL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearDepth(1.f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    GLfloat lamb[] = {0.0f, 0.0f, 0.0f, 0.0f};
    if(prms->disable_lighting!=1)
    {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lamb);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
    }
}

void renderer_proj_intrinsics(double nclip, double fclip)
{
    DEBUG_STDOUT("RENDERER_PROJ_INTRINSICS\n")
    double alpha = prms->K[0]/prms->K[8];
    double beta = prms->K[4]/prms->K[8];
    double skew = prms->K[3]/prms->K[8];
    double u0 = prms->K[6]/prms->K[8];
    double v0 = prms->imsize[0]-prms->K[7]/prms->K[8];
    double L = 0;
    double R = prms->imsize[1];
    double B = 0;
    double T = prms->imsize[0];
    double N = nclip;
    double F = fclip;
    double ortho[16];
    double persp[16];
    memset(ortho, 0, sizeof(double)*16);
    ortho[0] = 2.0/(R-L);
    ortho[12] = -(R+L)/(R-L);
    ortho[5] =  2.0/(T-B);
    ortho[13] = -(T+B)/(T-B);
    ortho[10] = -2.0/(F-N);
    ortho[14] = -(F+N)/(F-N);
    ortho[15] =  1.0;
    memset(persp, 0, sizeof(double)*16);
    persp[0] = alpha;
    persp[4] = skew;
    persp[8] = -u0;
    persp[5] = beta;
    persp[9] = -v0;
    persp[10] = N+F;
    persp[14] = N*F;
    persp[11] = -1.0;
    renderer_mat4_mul(ortho, persp, prms->frustum);
}

void renderer_render()
{
    DEBUG_STDOUT("RENDERER_RENDER\n")
    GLfloat light_ambient[] = {-1.0f, -1.0f, -1.0f, 1.0f};
    double biasMatrix[16] = {0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0};	//bias from [-1, 1] to [0, 1]
    double textureMatrix[16], tmpmat[16];
    char c_num[16], t_num[64];
    int l = 0, lnew;
    snprintf(t_num, 10, "%d", prms->N);

    for(int i=0; i<prms->N; ++i)
    {
        GLfloat light_position[] = {prms->projRT[i].T[0], prms->projRT[i].T[1], prms->projRT[i].T[2], 1.0f};
        snprintf(c_num, 10, "%d", i+1);
        std::cout<<"rendering: ";
        strcat(c_num, " of ");
        strcat(c_num, t_num);
        std::cout<<c_num;
        lnew = strlen(c_num);
        if(lnew>0)
        {
            for(int p=0; p<(l-lnew); ++p) std::cout<<" ";
            for(int p=0; p<(l-lnew); ++p) std::cout<<"\b";
        }
        l = lnew;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
        glViewport(0, 0, prms->imsize[1], prms->imsize[0]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(prms->frustum);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(prms->projRT[i].RT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        glCullFace(GL_FRONT);
        glShadeModel(GL_FLAT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        if(prms->point_size>0) glPointSize(prms->point_size);
        if(prms->disable_lighting==1) glDisable(GL_LIGHTING);
        glCallList(dlist);
        glBindTexture(GL_TEXTURE_2D, sptex_buf[0]);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, prms->imsize[1], prms->imsize[0]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDisable(GL_POLYGON_OFFSET_FILL);
        glCullFace(GL_BACK);
        glShadeModel(GL_SMOOTH);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
        glViewport(0, 0, prms->imsize[1], prms->imsize[0]);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(prms->frustum);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_ambient);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
        if(prms->point_size>0) glPointSize(prms->point_size);
        if(prms->disable_lighting==1) glDisable(GL_LIGHTING);

        glCallList(dlist);

        glLightfv(GL_LIGHT0, GL_DIFFUSE, prms->light_diffuse);
        renderer_mat4_mul(biasMatrix, prms->frustum, tmpmat);
        renderer_mat4_mul(tmpmat, prms->projRT[i].RT, textureMatrix);
        renderer_mat4_transpose(textureMatrix);

        glBindTexture(GL_TEXTURE_2D, sptex_buf[0]);
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGendv(GL_S, GL_EYE_PLANE, textureMatrix);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGendv(GL_T, GL_EYE_PLANE, textureMatrix+4);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGendv(GL_R, GL_EYE_PLANE, textureMatrix+8);
        glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGendv(GL_Q, GL_EYE_PLANE, textureMatrix+12);

        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        glEnable(GL_TEXTURE_GEN_Q);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
        glAlphaFunc(GL_GEQUAL, 0.99f);
        glEnable(GL_ALPHA_TEST);
        if(prms->projpattype>0)
        {
            glBindTexture(GL_TEXTURE_2D, sptex_buf[1]);
            glActiveTexture(GL_TEXTURE1);
            glEnable(GL_TEXTURE_2D);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGendv(GL_S, GL_EYE_PLANE, textureMatrix);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGendv(GL_T, GL_EYE_PLANE, textureMatrix+4);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGendv(GL_R, GL_EYE_PLANE, textureMatrix+8);
            glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGendv(GL_Q, GL_EYE_PLANE, textureMatrix+12);

            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_GEN_R);
            glEnable(GL_TEXTURE_GEN_Q);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
        glCallList(dlist);
        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_TEXTURE_GEN_Q);
        glDisable(GL_ALPHA_TEST);
        if(prms->projpattype>0)
        {
            glActiveTexture(GL_TEXTURE1);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_GEN_R);
            glDisable(GL_TEXTURE_GEN_Q);
        }

        glDisable(GL_LIGHTING);
        glFlush();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        renderer_copy_img_data(i);
        renderer_copy_depth_data(i);
        for(int p=0; p<(l+11); ++p) std::cout<<"\b";
    }
    std::cout<<"done.";
    for(int p=0; p<(l+7); ++p) std::cout<<" ";
    for(int p=0; p<(l+7); ++p) std::cout<<"\b";
    std::cout<<std::endl;
}

void renderer_init_all(const char *fname)
{
    DEBUG_STDOUT("RENDERER_INIT_ALL\n")
    MESH_ROTATION m_r = NULL;
    prms = renderer_load_params(fname);
    renderer_proj_intrinsics(R_NEAR_PLANE, R_FAR_PLANE);
    std::cout<<"input: "<<prms->model_fname<<std::endl;
    std::cout<<"output: "<<prms->ofname<<std::endl;
    std::cout<<"num of views: "<<prms->N<<std::endl;

    prms->m = renderer_load_file(prms->model_fname);
    std::cout<<"vertices: "<<prms->m->num_vertices<<", faces: "<<prms->m->num_faces<<std::endl;

    m_r = mesh_rotation_create();
    mesh_rotation_set_matrix(prms->sceneRT.R, m_r);
    mesh_rotate(prms->m, m_r);
    mesh_rotation_free(m_r);
    mesh_translate(prms->m, prms->sceneRT.T[0], prms->sceneRT.T[1], prms->sceneRT.T[2]);

    if(prms->projpattype>0)
    {
        num_textures = 2;
        std::cout<<"pattern: "<<prms->ipfname<<std::endl;
    }
    else num_textures = 2;
    renderer_init();
}

void renderer_copy_img_data(int i)
{
    DEBUG_STDOUT("RENDERER_COPY_IMG_DATA\n")
    char ofname[512], numc[32];
    unsigned char* pixels = NULL;
    int m;
    if(prms->outbpp==GL_RGBA) m = 1;
    else m = 2;

    if((pixels = (unsigned char*) malloc(prms->imsize[1]*prms->imsize[0]*4*m))==NULL) renderer_error(RENDERER_ERROR_MALLOC);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
    glViewport(0, 0, prms->imsize[1], prms->imsize[0]);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    if(m==1) glReadPixels(0,0,prms->imsize[1],prms->imsize[0], GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    else glReadPixels(0,0,prms->imsize[1],prms->imsize[0], GL_RGBA, GL_UNSIGNED_SHORT, pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    strcpy(ofname, prms->ofname);
    snprintf(numc, 10, "%d", i);
    strcat(ofname, "_photo_");
    strcat(ofname, numc);
    strcat(ofname, ".png");
    if(m==1) tiny_png_save(ofname, prms->imsize[1], prms->imsize[0], TINY_PNG_RGBA, pixels, TINY_PNG_ORDER_UP);
    else tiny_png_save(ofname, prms->imsize[1], prms->imsize[0], TINY_PNG_RGBA16, pixels, TINY_PNG_ORDER_UP);
    free(pixels);
}

void renderer_copy_depth_data(int i)
{
    DEBUG_STDOUT("RENDERER_COPY_DEPTH_DATA\n")
    char ofname[512], numc[32];
    int l1, l2, hd2;
    GLfloat* pixels = NULL, tmp;
    int32_t sz[2];
    std::ofstream fs;
    if((pixels = (GLfloat*)malloc(prms->imsize[1]*prms->imsize[0]*sizeof(GLfloat)))==NULL) renderer_error(RENDERER_ERROR_MALLOC);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
    glViewport(0, 0, prms->imsize[1], prms->imsize[0]);
    glReadBuffer(GL_DEPTH_ATTACHMENT);
    glReadPixels(0,0,prms->imsize[1],prms->imsize[0], GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    sz[0] = prms->imsize[0];
    sz[1] = prms->imsize[1];

    hd2 = prms->imsize[0]/2;
    for(int j=0; j<hd2; ++j)
    {
        l1 = prms->imsize[1]*j;
        l2 = prms->imsize[1]*(prms->imsize[0]-j-1);
        for(int k=0; k<prms->imsize[1]; ++k)
        {
            tmp = (R_NEAR_PLANE*R_FAR_PLANE/(R_NEAR_PLANE-R_FAR_PLANE))/(pixels[k+l1]-R_FAR_PLANE/(R_FAR_PLANE-R_NEAR_PLANE));
            pixels[k+l1] = (R_NEAR_PLANE*R_FAR_PLANE/(R_NEAR_PLANE-R_FAR_PLANE))/(pixels[k+l2]-R_FAR_PLANE/(R_FAR_PLANE-R_NEAR_PLANE));
            pixels[k+l2] = tmp;
        }
    }

    strcpy(ofname, prms->ofname);
    snprintf(numc, 10, "%d", i);
    strcat(ofname, "_depth_");
    strcat(ofname, numc);
    strcat(ofname, ".dpt");

    fs.open(ofname, std::ios::out|std::ios::binary);
    if(!fs.is_open()) renderer_error(RENDERER_ERROR_IO);
    fs<<"DPT0";
    fs.write(reinterpret_cast<char*>(sz), 2*sizeof(int32_t));
    fs.write(reinterpret_cast<char*>(pixels), prms->imsize[1]*prms->imsize[0]*sizeof(GLfloat));
    fs.close();
    free(pixels);
}

void renderer_close()
{
    DEBUG_STDOUT("RENDERER_CLOSE\n")
    glDeleteFramebuffers(2,fbo);
    glDeleteTextures(num_textures, sptex_buf);
    glDeleteRenderbuffers(1,&render_buf);
    glDeleteRenderbuffers(1,&drender_buf);
    glDeleteLists(dlist, 1);
    SDL_GL_DeleteContext(gcontxt);
    SDL_DestroyWindow(gwin);
    delete prms;
    gwin = NULL;
    SDL_Quit();
}

