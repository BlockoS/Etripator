#include "message.h"
#include "cfgext.h"

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
	CFGPayloadExt *payloadExt = (CFGPayloadExt*)data;
	size_t i;
	int err;

	memset(payloadExt->flag, 0, payloadExt->flagSize);

	payloadExt->section.name = strdup(sectionName);
	err =  payloadExt->section.initializeElement(payloadExt->section.element);
	if(!err)
	{
		return 0;
	}
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
	ARRAY_ERR aErr;
	HASHTABLE_ERR hErr;
	CFGPayloadExt *payloadExt = (CFGPayloadExt*)data;
	struct CFGSectionParser *current = &payloadExt->section;
	size_t last = current->data.count;
	size_t i, keyFound;
	
	/* Check flags */
	for(i=0, keyFound=0; i<current->keyCount; i++)
	{
		/* If the payload flag is set, this means that the flag is mandatory. */
		if((current->flag[i] > 0) && (payloadExt->flag[i] == 0))
		{
			ERROR_MSG("[%s] : missing %s.\n", current->name, current->keyValueValidator[i].key);
			return 0;
		}
		keyFound += payloadExt->flag[i];
	}

	/* Check for empty section */
	if(keyFound == 0)
	{
		ERROR_MSG("[%s] : no attributes found.\n", current->name);
		return 0;
	}

	/* Validate element */
	if(current->validateElement(current->name, current->element) <= 0)
	{
		return 0;
	}

	/* Commit current section */
	aErr = ArrayPush(&current->data, (uint8_t*)current->element);
	if(aErr != ARRAY_OK)
	{
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
	CFGPayloadExt *payloadExt = (CFGPayloadExt*)data;
	struct CFGSectionParser *current = &payloadExt->section;

	size_t i;
	for(i=0; i<current->keyCount; ++i)
	{
		if(strcmp(current->keyValueValidator[i].key, key) == 0)
		{
			if((payloadExt->flag[i] > 0) && current->flag[i])
			{
				ERROR_MSG("[%s] : %s multiple attributes found.\n", current->name, key);
				return 0;
			}
			else
			{
				int err;
				++payloadExt->flag[i];
				err = current->keyValueValidator[i].validate(current->element, key, value);
				if(err <= 0)
				{
					ERROR_MSG("[%s] : %s invalid value.\n", current->name, key);
				}
				return err;
			}
		}
	}

	return 0;
}

/**
 * Initialize CFG/Ini parser payload.
 * \param [out]  parser CFG/Ini Advanced parser to use.
 */
void SetupCFGParserPayload(CFGPayloadExt* parser)
{
	parser->payload.line = 0;
	parser->payload.data = parser;

	parser->payload.beginSectionCallback = payloadBeginCFGSection;
	parser->payload.endSectionCallback   = payloadEndCFGSection;
	parser->payload.keyValueCallback     = payloadValidateCFGTuple;
}
