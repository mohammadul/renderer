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
#include "../include/main.h"

using namespace std;

int main(int argc, char* argv[])
{
    DEBUG_STDOUT("MAIN\n")
    if(argc<2) renderer_error(RENDERER_ERROR_PARAM);
    renderer_init_all(argv[1]);
    renderer_render();
    renderer_close();
    return 0;
}

