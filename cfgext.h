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

/**
 * \brief CFG key parsing helper.
 */
typedef struct
{
	const char*     key;      /**< Key name. */
	CFGKeyValueProc validate; /**< Value validation callback. */
} CFGKeyValidator;

/**
 * CFG/Ini section parser.
 */
struct CFGSectionParser
{
	const char*      name;              /**< Section name. */
	CFGKeyValidator* keyValueValidator; /**< Key/Value tuple validators. */
	size_t           keyCount;          /**< Key count. */
	
	CFGBeginSectionProc beginSectionCallback; /**< Section parsing start callback. */
	CFGEndSectionProc   endSectionCallback;   /**< Section parsing end callback. */
};

/**
 * Global CFG/Ini parser.
 */
struct CFGParser
{
	struct CFGSectionParser *section; /**< Section parser array. */
	size_t                   count;   /**< Section count. */
	struct CFGSectionParser *current; /**< Current section parser. */
	void *data;                       /**< Use data. */
};

/**
 * Initialize CFG/Ini parser payload.
 * \param [in]  parser CFG/Ini parser to use.
 * \param [out] payload CFG/Ini payload to be initialized.
 */
void SetupCFGParserPayload(struct CFGParser* parser, struct CFGPayload *payload);

#endif // ETRIPATOR_CFG_EXT_H
