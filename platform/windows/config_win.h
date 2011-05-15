#ifndef CONFIG_WIN_H
#define CONFIG_WIN_H

#define _GNU_SOURCE

#pragma warning(disable : 4996)

#include <stdlib.h>
#include <stdio.h>

#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#include "getopt_win.h"
#include "stdint.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>

#endif // CONFIG_WIN_H