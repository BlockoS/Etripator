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
 * Validate memory section.
 * \param [in out] element Current memory layout.
 * \return always 1.
 */
static int ValidateMemoryLayout(void* element)
{
	// nothing to do atm.
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

/** Memory layout key flags. **/
static int8_t g_memoryKeyFlag[PCE_MPR_COUNT] =
{
	0, 0, 0, 0, 0, 0, 0, 0
};

static const char* g_supportedSectionTypeName[] =
{
	"bin_data", "inc_data", "code", NULL
};

static const char* g_defaultPrefix = "l_";

/**
 * Reset section.
 * \param [in] data Pointer to section.
 * \return always 1.
 */
static int ResetSection (void *data)
{
	Section *section = (Section*)data;
	section->bank   = 0;
	section->org    = 0;
	section->size   = 0;
	section->start  =-1;
	section->type   = 0;
	section->prefix = NULL;
	section->memory = NULL;
	return 1;
}

/**
 * Validate current section.
 * \param [in out] element Current section.
 * \return 
 *     <=0 failure
 *     >0 success 
 */
static int ValidateSection(void *element)
{
	Section *section = (Section*)element;

	/* Check prefix */
	if(section->prefix == NULL)
	{
		section->prefix = strdup(g_defaultPrefix);
	}

	/* Compute start address */
	if(section->start < 0)
	{
		section->start = (section->bank << 13) | (section->org & 0x1fff);
	}

	/* Other minor check. This will disapear in the next release. */
	if((section->type != CODE) && (section->size == 0))
	{
		ERROR_MSG("[%s] Automatic section size detection doesn't work for data.\n", section->name);
		return 0;
	}

	return 1;
}

/**
 * Validate section type
 * \see CFGKeyValueProc 
 */
static int validateSectionType(void *data, const char* key, const char* value)
{
	Section *section = (Section*)data;
	int i;
		
	for(i=0; g_supportedSectionTypeName[i] != NULL; i++)
	{
		if(strcasecmp(value, g_supportedSectionTypeName[i]) == 0)
		{
			section->type = i;
			return 1;
		}
	}
	return 0;
}

/**
 * Validate section bank
 * \see CFGKeyValueProc 
 */
static int validateSectionBank(void *data, const char* key, const char* value)
{
	Section *section = (Section*)data;
	unsigned long bank = strtoul(value, NULL, 16);
	if(errno || (bank > 0xff))
	{
		return 0;
	}
	section->bank = (uint8_t)bank;
	return 1;
}

/**
 * Validate section org 
 * \see CFGKeyValueProc 
 */
static int validateSectionOrg(void *data, const char* key, const char* value)
{
	Section *section = (Section*)data;
	unsigned long org = strtoul(value, NULL, 16);
	if(errno || (org > 0xffff))
	{
		return 0;
	}
	section->org = (uint16_t)org;
	return 1;
}

/**
 * Validate section offset
 * \see CFGKeyValueProc 
 */
static int validateSectionOffset(void *data, const char* key, const char* value)
{
	Section *section = (Section*)data;
	unsigned long offset = strtoul(value, NULL, 16);
	if(errno)
	{
		return 0;
	}
	section->start = offset ;
	return 1;
}

/**
 * Validate section size
 * \see CFGKeyValueProc 
 */
static int validateSectionSize(void *data, const char* key, const char* value)
{
	Section *section = (Section*)data;
	long size = strtol(value, NULL, 16);
	if(errno)
	{
		return 0;
	}
	section->size = (int32_t)size;
	return 1;
}

/**
 * Validate section prefix
 * \see CFGKeyValueProc 
 */
static int validateSectionPrefix(void *data, const char* key, const char* value)
{
	Section *section = (Section*)data;
	section->prefix = strdup(value);
	return 1;
}

/**
 * Validate section memory layout id
 * \see CFGKeyValueProc 
 */
static int validateSectionMemory(void *data, const char* key, const char* value)
{
	Section *section = (Section*)data;
	section->memory = strdup(value);
	return 1;
}

/** section key validator */
static CFGKeyValidator g_sectionKeyValidator[] = 
{
	{ "type",   validateSectionType},
	{ "bank",   validateSectionBank},
	{ "org",    validateSectionOrg},
	{ "offset", validateSectionOffset},
	{ "size",   validateSectionSize},
	{ "prefix", validateSectionPrefix},
	{ "memory", validateSectionMemory}
};

/** Section key flags. **/
static int8_t g_sectionKeyFlag[] =
{
	1, 1, 1, 0, 0, 0, 0
};

// [todo] output and fallback mecanism to the "standard" cfg parser

int ParseAppConfig(const char* cfgFilename)
{
	CFG_ERR cfgErr;

	CFGPayloadExt payloadExt;

	Memory   tmpMemoryLayout;
	Section  tmpSection;

	struct CFGSectionParser tmpSectionParser[2];

	size_t i;

	// [todo] Setup section parser and payloadExt
	payloadExt.flag     = NULL;
	payloadExt.flagSize = 0;
	
	payloadExt.id     = NULL;
	payloadExt.idSize = 0;

	payloadExt.count = 2;

	payloadExt.section = tmpSectionParser;

	payloadExt.section[0].name              = "memory";
	payloadExt.section[0].initializeElement = ResetMemoryLayout;
	payloadExt.section[0].validateElement   = ValidateMemoryLayout;
	payloadExt.section[0].keyCount          = PCE_MPR_COUNT;
	payloadExt.section[0].keyValueValidator = g_memoryKeyValidator;
	payloadExt.section[0].flag              = g_memoryKeyFlag;

	payloadExt.section[1].name              = "section";
	payloadExt.section[1].initializeElement = ResetSection;
	payloadExt.section[1].validateElement   = ValidateSection;
	payloadExt.section[1].keyCount          = 7;
	payloadExt.section[1].keyValueValidator = g_sectionKeyValidator;
	payloadExt.section[1].flag              = g_sectionKeyFlag;
	
	payloadExt.section[0].element = &tmpMemoryLayout;
	if(ArrayCreate(&payloadExt.section[0].data, sizeof(Memory)) != ARRAY_OK)
	{
		return 0;
	}
	if(SLCreate(&payloadExt.section[0].dict) != HASHTABLE_OK)
	{
		return 0;
	}

	payloadExt.section[1].element = &tmpSection;
	if(ArrayCreate(&payloadExt.section[1].data, sizeof(Section)) != ARRAY_OK)
	{
		return 0;
	}
	if(SLCreate(&payloadExt.section[1].dict) != HASHTABLE_OK)
	{
		return 0;
	}

	SetupCFGParserPayload(&payloadExt);

	/* Parse file */
	cfgErr = ParseCFG(cfgFilename, &(payloadExt.payload));
	if(cfgErr != CFG_OK)
	{
		ERROR_MSG("%s[%d] %s", cfgFilename, payloadExt.payload.line, GetCFGErrorMsg(cfgErr));
		return 0;
	}

	/* Check section memory id */
	for(i=0; i<payloadExt.section[1].data.count; i++)
	{
		Section *current = (Section*)ArrayAt(&payloadExt.section[1].data, i);
		if((current != NULL) && (current->memory != NULL))
		{
			uintptr_t dummy;
			if(SLFind(&payloadExt.section[0].dict, current->memory, strlen(current->memory)+1, &dummy) != HASHTABLE_OK)
			{
				ERROR_MSG("%s[%s] Unknown memory layout: %s", cfgFilename, current->name, current->memory);
				return 0;
			}
		}
	}

	// [todo] clean memory

	return 1;
}
