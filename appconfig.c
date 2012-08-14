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
#include "cfg.h"
#include "message.h"

static const char* g_supportedSectionTypeName[] =
{
	"bin_data", "inc_data", "code", "memory", NULL
};

static const char* g_defaultPrefix = "l_";

typedef struct 
{
	CFGKeyValidator *keyValueValidator; /**< Key/Value tuple validators. */
	int8_t          *flag;              /**< Key/Value flag. Tells if a given key is mandatory. */
	int8_t          *keyOccurence;      /**< Number of times a key was encountered. */
	size_t           keyCount;          /**< Key count. */
	char             type;              /**< Item type. */
	char*            name;              /**< Item name. */
	Section          section;           /**< Current section being parsed. */
	Memory           memory;			/**< Current memory layout being parsed. */
	// Todo array pointers
} ConfigurationData;

/**
 * Reset configuration data.
 * \param [in] data Pointer to configuration data.
 * \return always 1.
 **/
static int ResetConfigurationData (void *data)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;

	cfgData->section.type   = 0;
	cfgData->section.name   = NULL;
	cfgData->section.bank   = 0;
	cfgData->section.org    = 0;
	cfgData->section.size   = -1;
	cfgData->section.prefix = NULL;
	cfgData->section.memory = NULL;
	cfgData->section.start  = -1;

	memset(cfgData->memory.mpr, 0, PCE_MPR_COUNT);
	cfgData->memory.mpr[0] = 0xff;
	cfgData->memory.mpr[1] = 0xf8;

	memset(cfgData->keyOccurence, 0, cfgData->keyCount * sizeof(int8_t));

	return 1;
}

/**
 * \return
 *		<= 0 failure
 *		>  0 success
 **/
static int ValidateSection(Section *section)
{

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
 * \brief Key value callback
 * This callback is called when the parser encounters a new key/value pair.
 * If the key name matches one of the names specified in the key/value parser 
 * array of the current section, the corresponding callback will be called.
 * \param [in] data user provided data 
 * \param [in] key key name
 * \param [in] value value name
 * \return
 *     <=0 failure
 *     >0 success 
 */
static int payloadValidateCFGTuple(void *data, const char* key, const char* value)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;

	size_t i;
	for(i=0; i<cfgData->keyCount; ++i)
	{
		if(strcmp(cfgData->keyValueValidator[i].key, key) == 0)
		{
			if((cfgData->flag[i] > 0) && cfgData->keyOccurence[i])
			{
				ERROR_MSG("[%s] : %s multiple attributes found.\n", cfgData->name, key);
				return 0;
			}
			else
			{
				int err;
				++cfgData->keyOccurence[i];
				err = cfgData->keyValueValidator[i].validate(cfgData, key, value);
				if(err <= 0)
				{
					ERROR_MSG("[%s] : %s invalid value.\n", cfgData->name, key);
				}
				return err;
			}
		}
	}

	return 0;
}

/**
 * \brief Section start callback.
 * This callback will be called when the parser encounters a new section block.
 * \param [in] data user provided data 
 * \param [in] sectionName section name
 * \return 
 *     <=0 failure
 *     >0 success 
 */
static int payloadBeginCFGSection(void *data, const char* sectionName)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;
	int err;

	err =  ResetConfigurationData(cfgData);
	if(!err)
	{
		return 0;
	}
	cfgData->name = strdup(sectionName);
	return 1;
}

/**
 * \brief Section end callback
 * This callback will be called when the current section is over (ie at end of file or when a new section starts).
 * \param [in] data user provided data 
 * \return 
 *     <=0 failure
 *     >0 success 
 */
static int payloadEndCFGSection(void *data)
{
/*
	ARRAY_ERR aErr;
	HASHTABLE_ERR hErr;
*/
	ConfigurationData *cfgData = (ConfigurationData*)data;
	int err = 1;

	if(cfgData->type == 3)
	{
		// (todo) check name unicity
		// (todo) Memory
	}
	else
	{
		// (todo) check name unicity
		cfgData->section.type = cfgData->type;
		cfgData->section.name = cfgData->name;

		err = ValidateSection(&cfgData->section);
		if(!err)
		{
			return err;
		}

		// (todo) Push to array
	}

	return err;
}

/**
 * Validate section type
 * \see CFGKeyValueProc 
 */
static int validateSectionType(void *data, const char* key, const char* value)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;
	int i;
		
	for(i=0; g_supportedSectionTypeName[i] != NULL; i++)
	{
		if(strcasecmp(value, g_supportedSectionTypeName[i]) == 0)
		{
			cfgData->type = i;
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
	ConfigurationData *cfgData = (ConfigurationData*)data;
	unsigned long bank = strtoul(value, NULL, 16);
	if(errno || (bank > 0xff))
	{
		return 0;
	}
	cfgData->section.bank = (uint8_t)bank;
	return 1;
}

/**
 * Validate section org 
 * \see CFGKeyValueProc 
 */
static int validateSectionOrg(void *data, const char* key, const char* value)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;
	unsigned long org = strtoul(value, NULL, 16);
	if(errno || (org > 0xffff))
	{
		return 0;
	}
	cfgData->section.org = (uint16_t)org;
	return 1;
}

/**
 * Validate section offset
 * \see CFGKeyValueProc 
 */
static int validateSectionOffset(void *data, const char* key, const char* value)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;
	unsigned long offset = strtoul(value, NULL, 16);
	if(errno)
	{
		return 0;
	}
	cfgData->section.start = offset ;
	return 1;
}

/**
 * Validate section size
 * \see CFGKeyValueProc 
 */
static int validateSectionSize(void *data, const char* key, const char* value)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;
	long size = strtol(value, NULL, 16);
	if(errno)
	{
		return 0;
	}
	cfgData->section.size = (int32_t)size;
	return 1;
}

/**
 * Validate section prefix
 * \see CFGKeyValueProc 
 */
static int validateSectionPrefix(void *data, const char* key, const char* value)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;
	cfgData->section.prefix = strdup(value);
	return 1;
}

/**
 * Validate section memory layout id
 * \see CFGKeyValueProc 
 */
static int validateSectionMemory(void *data, const char* key, const char* value)
{
	ConfigurationData *cfgData = (ConfigurationData*)data;
	cfgData->section.memory = strdup(value);
	return 1;
}

/**
 * Validate mpr keys
 * \see CFGKeyValueProc
 **/
static int validateMPR(void *data, const char* key, const char* value)
{
	const char *start;
	char *end;
	unsigned int bank;
	unsigned int id;
	
	ConfigurationData *cfgData = (ConfigurationData*)data;
	
	start = value;
	end   = NULL;

	errno = 0;
	bank = strtoul(start, &end, 16);
	id   = key[3] - '0';
	if(errno || (bank >= 0xff) || (id >= PCE_MPR_COUNT))
	{
		return 0;
	}

	cfgData->memory.mpr[id] = bank;
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
	{ "memory", validateSectionMemory},
	{ "mpr0",   validateMPR},
	{ "mpr1",   validateMPR},
	{ "mpr2",   validateMPR}, 
	{ "mpr3",   validateMPR},
	{ "mpr4",   validateMPR},
	{ "mpr5",   validateMPR},
	{ "mpr6",   validateMPR},
	{ "mpr7",   validateMPR}
};

/** Section key flags. **/
static int8_t g_sectionKeyFlag[] =
{
	1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int ParseAppConfig(const char* cfgFilename)
{
	CFG_ERR cfgErr;
	ConfigurationData cfgData;
	struct CFGPayload cfgPayload;

	cfgData.name              = NULL;
	cfgData.type              = 0;
	cfgData.keyCount          = 15; // todo
	cfgData.keyValueValidator = g_sectionKeyValidator;
	cfgData.flag              = g_sectionKeyFlag;
	
	cfgData.keyOccurence = (int8_t*)malloc(cfgData.keyCount * sizeof(int8_t));
	
	ResetConfigurationData(&cfgData);

/* (todo) main
	if(ArrayCreate(&payloadExt.section.data, sizeof(Section)) != ARRAY_OK)
	{
		return 0;
	}
*/
	cfgPayload.data = &cfgData;
	cfgPayload.line = 0;
	cfgPayload.beginSectionCallback = payloadBeginCFGSection;
	cfgPayload.endSectionCallback   = payloadEndCFGSection;
	cfgPayload.keyValueCallback     = payloadValidateCFGTuple;

	/* Parse file */
	cfgErr = ParseCFG(cfgFilename, &cfgPayload);
	if(cfgErr != CFG_OK)
	{
		ERROR_MSG("%s[%d] %s", cfgFilename, cfgPayload.line, GetCFGErrorMsg(cfgErr));
		return 0;
	}

	// [todo] check section memory id
	// [todo] clean memory

	return 1;
}
