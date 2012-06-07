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

	AppConfig *config = (AppConfig*)data;

	start = value;
	end   = NULL;
	
	errno = 0;
	bank = strtoul(start, &end, 16);
	id   = key[3] - '0';
	if(errno || (bank >= 0xff) || (id >= PCE_MPR_COUNT))
	{
		return 0;
	}

	config->tmpMemoryLayout.mpr[id] = bank;

	return 1;
}

/**
 * Validate memory section id.
 * \see CFGKeyValueProc
 */
static int validateId (void *data, const char* key, const char* value)
{
	// [todo] loop through the memory layout array and check id
	return 1;
}

/**
 *
 */
static int beginMemorySection (void *data, const char* sectionName)
{
	AppConfig *config = (AppConfig*)data;
	
	/* Reset temp memory layout */
	memset(config->tmpMemoryLayout.mpr, 0, PCE_MPR_COUNT);
	config->tmpMemoryLayout.mpr[0] = 0xff;
	config->tmpMemoryLayout.mpr[1] = 0xf8;

	return 1;
}

/**
 *
 */
static int endMemorySection(void *data)
{
	AppConfig *config = (AppConfig*)data;
	size_t currentMemoryLayout = config->memoryLayoutCount;
	++config->memoryLayoutCount;

	/* Expand memory layout array */
	if(config->memoryLayoutCapacity <= config->memoryLayoutCount)
	{
		Memory *ptr;
		ptr = (Memory*)realloc(config->memoryLayout, config->memoryLayoutCapacity*2*sizeof(Memory));
		if(ptr == NULL)
		{
			// [todo] error msg
			return 0;
		}
		config->memoryLayoutCapacity *= 2;
		config->memoryLayout = ptr;
	}

	/* Commit current memory layout */
	memcpy(config->memoryLayout[currentMemoryLayout].mpr, config->tmpMemoryLayout.mpr, PCE_MPR_COUNT);
	// [todo] Id

	return 1;
}

/** MPR key validator */
static CFGKeyValidator g_memoryMprKeyValidator[PCE_MPR_COUNT+1] = 
{
	{"mpr0", validateMPR}, {"mpr1", validateMPR},
	{"mpr2", validateMPR}, {"mpr3", validateMPR},
	{"mpr4", validateMPR}, {"mpr5", validateMPR},
	{"mpr6", validateMPR}, {"mpr7", validateMPR},
	{"id",   validateId}
};

/** Memory section parser */
static struct CFGSectionParser g_memoryParser =
{
	"memory", 
	g_memoryMprKeyValidator,
	PCE_MPR_COUNT+1,
	beginMemorySection, endMemorySection
};

/**
 *
 * \param [in] cfgFilename
 * \param [out] config
 * \return 
 */
int ParseAppConfig(const char* cfgFilename, AppConfig *config)
{
	CFG_ERR cfgErr;
	CFGPayloadExt payloadExt;

	// todo : reset memory layout function
	memset(config->tmpMemoryLayout.mpr, 0, PCE_MPR_COUNT);
	config->tmpMemoryLayout.mpr[0] = 0xff;
	config->tmpMemoryLayout.mpr[1] = 0xf8;

	config->memoryLayout         = NULL;
	config->memoryLayoutCount    = 0;
	config->memoryLayoutCapacity = 1;

	// todo : reset section function
	config->tmpSection.bank  = 0;
	config->tmpSection.id    = 0;
	config->tmpSection.org   = 0;
	config->tmpSection.size  = 0;
	config->tmpSection.start = 0;
	config->tmpSection.type  = 0;

	config->section         = NULL;
	config->sectionCount    = 0;
	config->sectionCapacity = 1;

	// todo
	payloadExt.count   = 1;
	payloadExt.section = &g_memoryParser;
	
	SetupCFGParserPayload(&payloadExt);

	payloadExt.data = config;
	cfgErr = ParseCFG(cfgFilename, &(payloadExt.payload));
	if(cfgErr != CFG_OK)
	{
		ERROR_MSG("%s[%d] %s", cfgFilename, payloadExt.payload.line, GetCFGErrorMsg(cfgErr));
		return 0;
	}

	return 1;
}

