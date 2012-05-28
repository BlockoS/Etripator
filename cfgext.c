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
	struct CFGParser *parser = (struct CFGParser*)data;
	size_t i;

	parser->current = NULL;
	for(i=0; i<parser->count; ++i)
	{
		if(strcmp(sectionName, parser->section[i].name) == 0)
		{
			parser->current = parser->section + i;
			return parser->current->beginSectionCallback(parser->data, sectionName);
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
		struct CFGParser *parser = (struct CFGParser*)data;
		if(parser->current == NULL)
		{
			return parser->current->endSectionCallback(parser->data);
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
	struct CFGParser *parser = (struct CFGParser*)data;
	struct CFGSectionParser *current = parser->current;
	size_t i;

	for(i=0; i<current->keyCount; ++i)
	{
		if(strcmp(current->keyValueValidator[i].key, key) == 0)
		{
			return current->keyValueValidator[i].validate(parser->data, key, value);
		}
	}

	return 0;
}

/**
 * Initialize CFG/Ini parser payload.
 * \param [in]  parser CFG/Ini parser to use.
 * \param [out] payload CFG/Ini payload to be initialized.
 */
void SetupCFGParserPayload(struct CFGParser* parser, struct CFGPayload *payload)
{
	payload->line = 0;
	payload->data = parser;

	payload->beginSectionCallback    = payloadBeginCFGSection;
	payload->endSectionCallback      = payloadEndCFGSection;
	payload->keyValueCallback        = payloadValidateCFGTuple;
}
