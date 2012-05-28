/*
    This file is part of Etripator,
    copyright (c) 2009--2012 Vincent Cruz.

    Etripator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Etripator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef CONFIG_WIN_H
#define CONFIG_WIN_H

#define _GNU_SOURCE

#pragma warning(disable : 4996)

#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define snprintf _snprintf

#include "getopt_win.h"
#include "stdint.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>

#if defined(_MSC_VER) && !defined(__cplusplus)
#define inline __forceinline
#endif

#ifdef ARCH_EXPORTS
#define ARCH_API __declspec(dllexport)
#else
#define ARCH_API __declspec(dllimport)
#endif

#endif // CONFIG_WIN_H