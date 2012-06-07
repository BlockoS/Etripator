#include "cfgext.h"

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

	payloadExt->current = NULL;
	for(i=0; i<payloadExt->count; ++i)
	{
		if(strcmp(sectionName, payloadExt->section[i].name) == 0)
		{
			payloadExt->current = payloadExt->section + i;
			return payloadExt->current->beginSectionCallback(payloadExt->data, sectionName);
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
	if(data != NULL)
	{
		CFGPayloadExt *payloadExt = (CFGPayloadExt*)data;
		if(payloadExt->current != NULL)
		{
			return payloadExt->current->endSectionCallback(payloadExt->data);
		}
	}
	return 0;
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
	size_t i;

	for(i=0; i<current->keyCount; ++i)
	{
		if(strcmp(current->keyValueValidator[i].key, key) == 0)
		{
			return current->keyValueValidator[i].validate(payloadExt->data, key, value);
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
