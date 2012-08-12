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
#ifndef _SECTION_H_
#define _SECTION_H_

#include "config.h"
#include "labels.h"

/**
 * Section related errors
 **/
typedef enum
{
	SECTION_OK = 0,                  /**< Success. */
	SECTION_INVALID_PARAMETERS,      /**< At least one of the parameter has an invalid value. */
	SECTION_PARSING_ERROR,           /**< Todo . */
	SECTION_MEMORY_ISSUE             /**< Something went wrong with the memory. */
} SECTION_ERR;

/**
 *   Define a ROM area.
 * 	 This area can contain data or code.
 **/
struct Section_
{
	char     type;   /**< type (defined below) */
	char*    name;   /**< name */
	uint8_t  bank;   /**< rom bank */
	uint16_t org;    /**< org */
	off_t    start;  /**< start address (file offset in bytes) */
	size_t   size;   /**< section size */
	char*    prefix; /**< label prefix */
	char*    memory; /**< memory layout id */
};
typedef struct Section_ Section;

enum SectionType {
	BIN_DATA = 0,
	INC_DATA,
	CODE
};

/**
 * Extract sections from a CFG file.
 * \param [in] iFilename ROM filename.
 * \param [out] iSection Section array.
 * \param [out] iSectionCount Number of succesfully parsed sections.
 * \return SECTION_OK if the parsing was succesful. Otherwise \see SECTION_ERR for more informations.
 **/ 
SECTION_ERR readSectionsFromCFG(char* iFileName, Section** iSection, size_t* iSectionCount);

#endif // _SECTION_H_
