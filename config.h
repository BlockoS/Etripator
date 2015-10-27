/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

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
#ifndef CONFIG_H
#define CONFIG_H

#if defined(_MSC_VER)
#include "platform/windows/config.h"
#else
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

//#define strncasecmp _strnicmp
//#define strcasecmp _stricmp

#include <unistd.h>
#include <getopt.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>

#ifndef ARCH_API
#if defined(HAVE_VISIBILITY)
#define ARCH_API __attribute__((__visibility__("default")))
#else
#define ARCH_API
#endif // HAVE_VISIBILITY
#endif // ARCH_API

#endif

#endif // CONFIG_H
