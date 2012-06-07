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
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "cfgext.h"
#include "memory.h"
#include "section.h"

/**
 *
 */
struct AppConfig_
{
	// [todo] use array.h macro?
	// [todo] use hashtable for id
	Memory   tmpMemoryLayout;      /**< */
	Memory  *memoryLayout;         /**< */
	size_t   memoryLayoutCount;    /**< */
	size_t   memoryLayoutCapacity; /**< */

	Section  tmpSection;      /**< */
	Section *section;         /**< */
	size_t   sectionCount;    /**< */
	size_t   sectionCapacity; /**< */
};
typedef struct AppConfig_ AppConfig;

/**
 *
 * \param [in] cfgFilename
 * \param [out] config
 * \return 
 */
int ParseAppConfig(const char* cfgFilename, AppConfig *config);

/**
 *
 * \param [in/out] config
 */
void DestroyAppConfig(AppConfig *config);

#endif // APP_CONFIG_H