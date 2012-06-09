#include "cfgext.h"

// [todo] Add messages.

/**
 * \brief Section start callback.
 * This callback will be called when the parser encounters a new section block.
 * If the name of the section matches one of the names specified in the section 
 * parser array, the corresponding callback will be called.
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

	payloadExt->current = NULL;
	for(i=0; i<payloadExt->count; ++i)
	{
		if(strcmp(sectionName, payloadExt->section[i].name) == 0)
		{
			payloadExt->current = payloadExt->section + i;

			if(payloadExt->current->keyCount > payloadExt->flagSize)
			{
				uint8_t *tmp = (uint8_t*)realloc(payloadExt->flag, payloadExt->current->keyCount);
				if(tmp == NULL)
				{
					return 0;
				}
				payloadExt->flag     = tmp;
				payloadExt->flagSize = payloadExt->current->keyCount;
			}
			memcpy(payloadExt->flag, 0, payloadExt->flagSize);

			err =  payloadExt->current->initializeElement(payloadExt->current->element);
			if(!err)
			{
				return 0;
			}
		}
	}

	// TODO : handle unknown sections ?

	return 0;
}

/**
 * \brief Section end callback
 * This callback will be called when the current section is over (ie at end of file or when a new section starts).
 * If the name of the section matches one of the names specified in the section 
 * parser array, the corresponding callback will be called.
 * \param [in] data user provided data 
 * \return 
 *     <=0 failure
 *     >0 success 
 */
static int payloadEndCFGSection(void *data)
{
	ARRAY_ERR aErr;
	HASHTABLE_ERROR hErr;
	CFGPayloadExt *payloadExt = (CFGPayloadExt*)data;
	struct CFGSectionParser *current = payloadExt->current;
	size_t last = current->data.count;

	// [todo] Check flags

	/* Commit current memory layout */
	aErr = ArrayPush(&current->data, (uint8_t*)current->element);
	if(aErr != ARRAY_OK)
	{
		return 0;
	}

	/* Add id to hashtable */
	hErr = SLAdd(&current->dict, payloadExt->id, strlen(payloadExt->id)+1, last);
	if(hErr != HASHTABLE_OK)
	{
		return 0;
	}

	/* Copy temporary element into the array. */
	return current->copyElement(ArrayAt(&current->data, last), data);
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
	struct CFGSectionParser *current = payloadExt->current;

	/* Id key receives a special treatment. */
	if(strcmp("id", key) == 0)
	{
		uintptr_t dummy;
		size_t idLen = strlen(value)+1;
		HASHTABLE_ERROR err = SLFind(&current->dict, value, idLen, &dummy);
		if(err != HASHTABLE_UNKNOWN_ID)
		{
			/* There is already an entry for this id! */
			return 0;
		}

		/* Save id for commit */
		if(idLen > payloadExt->idSize)
		{
			char *tmp = (char*)realloc(payloadExt->id, idLen);
			if(tmp == 0)
				return 0;
			payloadExt->id     = tmp;
			payloadExt->idSize = idLen;
		}
		memcpy(payloadExt->id, value, idLen);

		return 1;
	}
	else
	{
		size_t i;
		for(i=0; i<current->keyCount; ++i)
		{
			if(strcmp(current->keyValueValidator[i].key, key) == 0)
			{
				if(payloadExt->flag[i])
				{
					return 0;
				}
				++payloadExt->flag[i];
				return current->keyValueValidator[i].validate(current->element, key, value);
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
