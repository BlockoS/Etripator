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
#include "appconfig.h"
#include "message.h"

// [todo] add section parsing

/**
 * Validate mpr value.
 * \see CFGKeyValueProc
 */
static int validateMPR (void *data, const char* key, const char* value)
{
	const char *start;
	char *end;
	unsigned int bank;
	unsigned int id;

	Memory *memoryLayout = (Memory*)data;

	start = value;
	end   = NULL;
	
	errno = 0;
	bank = strtoul(start, &end, 16);
	id   = key[3] - '0';
	if(errno || (bank >= 0xff) || (id >= PCE_MPR_COUNT))
	{
		return 0;
	}

	memoryLayout->mpr[id] = bank;

	return 1; 
}

/**
 * Reset memory layout.
 * \param [in] data Pointer to memory layout.
 * \return always 1.
 */
static int ResetMemoryLayout (void *data)
{
	Memory *memoryLayout = (Memory*)data;
	memset(memoryLayout->mpr, 0, PCE_MPR_COUNT);
	memoryLayout->mpr[0] = 0xff;
	memoryLayout->mpr[1] = 0xf8;
	return 1;
}

/**
 * Copy memory layout into another.
 * \param [out] dst Destination.
 * \param [in]  src Source.
 * \return always 1.
 */
static int CopyMemoryLayout(void *dst, void *src)
{
	memcpy(dst, src, sizeof(Memory));
	return 1;
}

/** MPR key validator */
static CFGKeyValidator g_memoryKeyValidator[PCE_MPR_COUNT] = 
{
	{"mpr0", validateMPR}, {"mpr1", validateMPR},
	{"mpr2", validateMPR}, {"mpr3", validateMPR},
	{"mpr4", validateMPR}, {"mpr5", validateMPR},
	{"mpr6", validateMPR}, {"mpr7", validateMPR}
};


/**
 * Reset section.
 * \param [in] data Pointer to section.
 * \return always 1.
 */
static int ResetMemory (void *data)
{
	Section *section = (Section*)data;
	section->bank  = 0;
	section->id    = 0;
	section->org   = 0;
	section->size  = 0;
	section->start = 0;
	section->type  = 0;
	return 1;
}

/**
 * Copy section into another.
 * \param [out] dst Destination.
 * \param [in]  src Source.
 * \return always 1.
 */
static int CopySection(void *dst, void *src)
{
	memcpy(dst, src, sizeof(Section));
	return 1;
}

/** Code key validator */
/*
static CFGKeyValidator g_codeKeyValidator[6] = 
{
	{ "bank",   validateBank},
	{ "org",    validateOrg},
	{ "offset", validateOffset},
	{ "size",   validateSize},
	{ "prefix", validatePrefix}
};
*/

#if 0
int ParseAppConfig(const char* cfgFilename)
{
	CFG_ERR cfgErr;
	CFGPayloadExt payloadExt;

	Memory   tmpMemoryLayout;
	Section  tmpSection;

	// [todo] Setup section parser and payloadExt

	SetupCFGParserPayload(&payloadExt);

	/* Parse file */
	cfgErr = ParseCFG(cfgFilename, &(payloadExt.payload));
	if(cfgErr != CFG_OK)
	{
		ERROR_MSG("%s[%d] %s", cfgFilename, payloadExt.payload.line, GetCFGErrorMsg(cfgErr));
		return 0;
	}

	return 1;
}
#endif
