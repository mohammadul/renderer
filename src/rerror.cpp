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
#include "../include/rerror.h"

void _renderer_error(int type, const int lnum, const char *fname)
{
    DEBUG_STDOUT("RENDERER_ERROR\n")
    switch(type)
    {
    case RENDERER_ERROR_MALLOC:
        std::cout<<"renderer: malloc error ["<<fname<<": "<<lnum<<"]"<<std::endl;
        break;
    case RENDERER_ERROR_MATH:
        std::cout<<"renderer: math error ["<<fname<<": "<<lnum<<"]"<<std::endl;
        break;
    case RENDERER_ERROR_IO:
        std::cout<<"renderer: io error ["<<fname<<": "<<lnum<<"]"<<std::endl;
        break;
    case RENDERER_ERROR_PARAM:
        std::cout<<"renderer: param error ["<<fname<<": "<<lnum<<"]"<<std::endl;
        break;
    case RENDERER_ERROR_SDL:
        std::cout<<"renderer: SDL error ["<<fname<<": "<<lnum<<"]"<<std::endl;
        break;
    case RENDERER_ERROR_GL:
        std::cout<<"renderer: GL error ["<<fname<<": "<<lnum<<"]"<<std::endl;
        break;
    case RENDERER_ERROR_ARB:
        std::cout<<"renderer: ARB error ["<<fname<<": "<<lnum<<"]"<<std::endl;
        break;
    case RENDERER_ERROR_PNG:
        std::cout<<"renderer: png error ["<<fname<<": "<<lnum<<"]"<<std::endl;
        break;
    case RENDERER_ERROR_UNKNOWN:
    default:
        std::cout<<"renderer: unknown error ["<<fname<<": "<<lnum<<"]"<<std::endl;
    }
    exit(-1-type);
}
