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

#ifndef RCOMMON_H_INCLUDED
#define RCOMMON_H_INCLUDED

#if defined(_DEBUG)
#define DEBUG
#endif
#if defined(NDEBUG)
#undef DEBUG
#endif

#ifdef DEBUG
#define DEBUG_STDERR(x) {std::cerr<<(x);}
#define DEBUG_STDOUT(x) {std::cout<<(x);}
#else
#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
#endif

#endif // RCOMMON_H_INCLUDED
