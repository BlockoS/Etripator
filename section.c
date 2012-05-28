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
#include "section.h"
#include "message.h"
#include "labels.h"
#include "cfg.h"

static const char* supportedSectionTypeName[] =
{
	"bin_data", "inc_data", "code", NULL
};

/*
 * Validate section type
 */
static int validateType(const char* value, Section *currentSection)
{
	int i;
		
	for( i=0; supportedSectionTypeName[i] != NULL; ++i )
	{
		if(strcasecmp(value, supportedSectionTypeName[i]) == 0)
		{
			currentSection->type = i;
			return 1;
		}
	}
	return 0;
}

/*
 * Validate bank 
 */
static int validateBank(const char* value, Section *currentSection)
{
	unsigned long bank;
	
	bank = strtoul(value, NULL, 16);
	if(bank == ULONG_MAX)
	{
		return 0;
	}
	
	currentSection->bank = (uint8_t)bank;
	
	return 1;
}

/*
 * Validate org 
 */
static int validateOrg(const char* value, Section *currentSection)
{
	unsigned long org;
	
	org = strtoul(value, NULL, 16);
	if(org == ULONG_MAX)
	{
		return 0;
	}
	
	currentSection->org = (uint16_t)org;
	
	return 1;
}


/*
 * Validate offset 
 */
static int validateOffset(const char* value, Section *currentSection)
{
	unsigned long offset;
	
	offset = strtoul(value, NULL, 16);
	if(offset  == ULONG_MAX)
	{
		return 0;
	}
	
	currentSection->start  = offset ;
	
	return 1;
}

/*
 * Validate size
 */
static int validateSize(const char* value, Section *currentSection)
{
	long size;
	
	size = strtol(value, NULL, 16);
	if(size  == LONG_MAX)
	{
		return 0;
	}
	
	currentSection->size = (int32_t)size;
	
	return 1;
}

/*
 * Validate section Id
 */
static int validateId(const char* value, Section *currentSection)
{
	unsigned long id;
	
	id = strtoul(value, NULL, 16);
	if(id  == ULONG_MAX)
	{
		return 0;
	}

	if(id > 0xff)
	{
		return 0;
	}
	
	currentSection->id  = (uint8_t)id;
	
	return 1;
}

#define KEY_COUNT 6

#define TYPE_MASK   1
#define BANK_MASK   (1<<1)
#define ORG_MASK    (1<<2)
#define OFFSET_MASK (1<<3)
#define SIZE_MASK   (1<<4)
#define ID_MASK     (1<<5)

/**
 * \brief CFG key parsing helper
 */
typedef struct
{
	const char* key;
	int (*validate)(const char* value, Section* section); 
} KeyValidator;

 /**
  * \brief Helper structure for CFG parsing of sections.
  */
 struct SectionValidator
 {
	Section*      section;      /**< Section array. */
	size_t        sectionCount; /**< Number of processed sections. */
	size_t        capacity;     /**< Section array element count. */
	Section*      current;      /**< Current section being processed. */
	uint32_t      flag;         /**< Bitfield where each bit corresponds to a specific key. */
	int           keyCount;     /**< Number of supported key count. */
	KeyValidator* keyChecker;   /**< . Set of key validators. */
 };

/*
 * \brief Expand section array.
 *
 * \param validator  contains the section array to expand.
 * \return
 *     SECTION_MEMORY_ISSUE if reallocation failed.
 *     SECTION_OK if the section array was succesfully expanded.
 */
static SECTION_ERR ExpandSection(struct SectionValidator* validator)
{
	int count = validator->capacity + 4;
	Section *tmp;

	tmp = (Section*)malloc(count * sizeof(Section));
	if(tmp == NULL)
	{
		return SECTION_MEMORY_ISSUE;
	}
	
	validator->current = tmp + (validator->current - validator->section);

	if(validator->section != NULL)
	{
		memcpy(tmp, validator->section, validator->capacity * sizeof(Section));	
		free(validator->section);
	}

	validator->capacity = count;
	validator->section  = tmp;
	
	return SECTION_OK;
}

/**
 * \brief Section start callback
 *
 * \param data         Section validator
 * \param sectionName  Section name
 * \return 0 if an error occured, 1 on success
 */
static int beginCFGSection(void *data, const char* sectionName)
{
	struct SectionValidator *validator = (struct SectionValidator*)data;

	if(validator->sectionCount == validator->capacity)
	{
		if(ExpandSection(validator) != SECTION_OK)
		{
			return 0;
		}
	}

	validator->current        = validator->section + validator->sectionCount;
	validator->current->bank  = 0;
	validator->current->org   = 0;
	validator->current->start = 0;
	validator->current->size  = 0;
	validator->current->id    = validator->sectionCount;
	validator->current->name  = strdup(sectionName);

	validator->flag  = 0;

	++validator->sectionCount;

	return 1;
}

/*
 * \brief Check section attributes and compute start offset if needed.
 * \param data         Section validator
 * \return 0 if an error occured, 1 on success
 */
 static int finalizeCFGSection( void *data )
 {
	struct SectionValidator *validator = (struct SectionValidator*)data;

	uint32_t flag           = validator->flag;
	Section *currentSection = validator->current;

	if( !(flag & (TYPE_MASK)) )
	{
		ERROR_MSG("[%s] : missing type.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & (BANK_MASK)) )
	{
		ERROR_MSG("[%s] : missing bank.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & (ORG_MASK)) )
	{
		ERROR_MSG("[%s] : missing org.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & OFFSET_MASK) )
	{
		currentSection->start = (currentSection->bank << 13) | (currentSection->org & 0x1fff);
	}

	if((currentSection->type != CODE) && (currentSection->size == 0))
	{
		ERROR_MSG("[%s] Automatic section size detection doesn't work for data.\n", currentSection->name);
		return 0;
	}

	return 1;
 }

/**
 * \brief Validate current key value pair.
 * \param data   Pointer to current section validator.
 * \param key    Key string.
 * \param value  Value string.
 * \return 0 on error, 1 on sucess.
 */
static int validateCFGTuple(void *data, const char* key, const char* value)
{
	struct SectionValidator *validator = (struct SectionValidator*)data;

	int i;
	for(i=0; i<validator->keyCount; ++i)
	{
		if(strcmp(validator->keyChecker[i].key, key) == 0)
		{
			if(validator->flag & (1 << i))
			{
				ERROR_MSG("%s already set", key );
				return 0;
			}
			validator->flag |= (1 << i);
			return validator->keyChecker[i].validate(value, validator->current);
		}
	}

	return 0;
}

/**
 * Extract sections from a CFG file.
 * \param [in] iFilename ROM filename.
 * \param [out] iSection Section array.
 * \param [out] iSectionCount Number of succesfully parsed sections.
 * \return SECTION_OK if the parsing was succesful. Otherwise \see SECTION_ERR for more informations.
 **/ 
SECTION_ERR readSectionsFromCFG(char* iFileName, Section** iSection, size_t* iSectionCount)
{
	struct CFGPayload payload;
	struct SectionValidator validator;

	CFG_ERR cfgErr;
	SECTION_ERR sectionErr;

	static KeyValidator keyCheckers[KEY_COUNT] =
	{
		{ "type",   validateType},
		{ "bank",   validateBank},
		{ "org",    validateOrg},
		{ "offset", validateOffset},
		{ "size",   validateSize},
		{ "id",     validateId}
	};

	validator.capacity     = *iSectionCount;
	validator.sectionCount = *iSectionCount;
	validator.current      = (Section*)(*iSectionCount); 
	validator.section      = NULL;
	validator.keyCount     = KEY_COUNT;
	validator.keyChecker   = keyCheckers;

	sectionErr = ExpandSection(&validator);
	if(sectionErr != SECTION_OK)
	{
		return sectionErr;
	}

	payload.data                 = &validator;
	payload.keyValueCallback     = validateCFGTuple;
	payload.beginSectionCallback = beginCFGSection;
	payload.endSectionCallback   = finalizeCFGSection;
	
	cfgErr = ParseCFG(iFileName, &payload);
	if(cfgErr != CFG_OK)
	{
		ERROR_MSG("%s[%d] %s", iFileName, payload.line, GetCFGErrorMsg(cfgErr));
		return SECTION_PARSING_ERROR;
	}

	*iSectionCount = validator.sectionCount;
	*iSection      = validator.section;

	return SECTION_OK;
}

