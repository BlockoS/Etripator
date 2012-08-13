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
#ifndef ETRIPATOR_CFG_EXT_H
#define ETRIPATOR_CFG_EXT_H

#include "config.h"
#include "cfg.h"
#include "array.h"
#include "hashtable.h"

/**
 * \brief CFG key parsing helper.
 */
typedef struct
{
	const char*     key;      /**< Key name. */
	CFGKeyValueProc validate; /**< Value validation callback. */
} CFGKeyValidator;

/**
 * \brief Initialize element callback.
 * This callback will be called when the parser encounters a new section block.
 * \param [in] data user provided data 
 * \return 
 *     <=0 failure
 *     >0 success 
 */
typedef int (*CFGExtInitializeElementProc) (void *data);

/**
 * \brief Validate element.
 * This callback will be called when the current section is over (ie at end of file or when a new section starts).
 * \param [in] name Section name.
 * \param [in out] element Current element.
 * \return 
 *     <=0 failure
 *     >0 success 
 */
typedef int (*CFGExtValidateElementProc) (char* name, void *element);

/**
 * CFG/Ini section parser.
 */
struct CFGSectionParser
{
	char            *name;              /**< Section name. */
	CFGKeyValidator *keyValueValidator; /**< Key/Value tuple validators. */
	int8_t          *flag;              /**< Key/Value flag. Tells if a given key is mandatory. */
	size_t           keyCount;          /**< Key count. */
	void            *element;           /**< Temporary element. */
	Array            data;              /**< Array containing parsed elements. */

	CFGExtInitializeElementProc initializeElement; /**< Initialize/reset temporary element. */
	CFGExtValidateElementProc   validateElement;   /**< Validate element. */
};

/**
 * Advanced payload structure for CFG/Ini file parser.
 */
struct CFGPayloadExt_
{
	struct CFGPayload        payload;   /**< Standard payload structure. */
	struct CFGSectionParser  section;   /**< Section parser. */
	int8_t                  *flag;      /**< Current key/value flags. */
	size_t                   flagSize;  /**< Key/Value buffer size. */
};
typedef struct CFGPayloadExt_ CFGPayloadExt;

/**
 * Initialize CFG/Ini parser payload.
 * \param [out]  parser CFG/Ini Advanced parser to use.
 */
void SetupCFGParserPayload(CFGPayloadExt* parser);

// [todo] Create payloadExt
// [todo] Destroy payloadExt

#endif // ETRIPATOR_CFG_EXT_H
