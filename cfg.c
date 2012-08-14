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
#include "config.h"
#include "cfg.h"

/**
 * \brief CFG parser helper
 */
struct CFGParser
{
	struct CFGPayload* payload; /*< User data and callbacks. */
	char *lineBuffer;           /*< Char buffer large enough to contains a "chomped" line. */
	size_t lineBufferSize;      /*< Line buffer size. */
	size_t lineStringLength;    /*< Length of the string stored in the line buffer. */
	FILE *input;                /*< File being parser. */
	int sectionCount;           /*< Section count. */
};

#define CFG_FORBIDDEN_CHAR_COUNT 4
static const char g_CFGForbiddenChar[CFG_FORBIDDEN_CHAR_COUNT] = 
{
	'=', ';', ' ', '\t'
};

#define CFG_ESCAPED_CHAR_COUNT 9
static const char g_CFGEscapedChar[CFG_ESCAPED_CHAR_COUNT] = 
{
	'n', 'r', 't', '\'', '"', '\\', '?', '=', ';'
};

static const char g_CFGTranslatedChar[CFG_ESCAPED_CHAR_COUNT] = 
{
	'\n', '\r', '\t', '\'', '"', '\\', '?', '=', ';'
};

/**
 * \brief Get the message associated to current CFG parsing error.
 *
 * \param err  Error value.
 * \return Error message.
 */
const char* GetCFGErrorMsg(CFG_ERR err)
{
	static const char* errMsg[] =
	{
		"Success",
		"At least one of the parameter has an invalid value",
		"An error occured while opening source file",
		"An error occured while reading source file",
		"The source file end was reached",
		"Empty string",
		"Invalid string",
		"Closing section delimiter is missing",
		"Section declaration is invalid",
		"Section callback failed",
		"Key/Value callback failed",
		"Missing equal separator",
		"Unexpected memory error",
		"Unknown error"
	};

	if(err > CFG_UNKNOWN_ERROR)
	{
		err = CFG_UNKNOWN_ERROR;
	}
	return errMsg[err];
}

/**
 * \brief Increase line buffer size.
 *
 * The request buffer size is adjusted to the closest highest multiple of 256.
 *
 * \param parser CFG parser holding the line buffer to resize.
 * \param newSize New buffer size.
 * \return
 *     CFG_OK if the line buffer was succesfully resized.
 *     CFG_MEMORY_ISSUE if the memory allocation failed.
 */
static CFG_ERR ResizeLineBuffer(struct CFGParser* parser, size_t newSize)
{
	size_t adjustedSize;	
	char *newBuffer;
	
	adjustedSize = (newSize + 0xff) & ~0xff;
	
	newBuffer = (char*)malloc(adjustedSize);
	if(newBuffer == NULL)
	{
		return CFG_MEMORY_ISSUE;
	}

	if(parser->lineBuffer != NULL)
	{
		free(parser->lineBuffer);
	}
	parser->lineBuffer = newBuffer;
	parser->lineBufferSize = adjustedSize;

	return CFG_OK;
}

/**
 * \brief Cleanup CFG parser
 * \param parser CFG parser
 */
static void CleanupCFGParser(struct CFGParser* parser)
{
	parser->payload = NULL;
	parser->lineBufferSize   = 0;
	parser->lineStringLength = 0;

	if(parser->lineBuffer != NULL)
	{
		free(parser->lineBuffer);
		parser->lineBuffer = NULL;
	}
}

/**
 * \brief Read a line from file.
 *
 * The starting and trailing spaces are removed.
 * \param parser CFG parser
 * \return \see CFG_ERR
 */
static CFG_ERR ReadLine(struct CFGParser* parser)
{
	char byte;
	size_t nRead;

	char *ptr = parser->lineBuffer;
	char *end = parser->lineBuffer + parser->lineBufferSize;

	// Skip starting spaces
	do
	{
		nRead = fread(&byte, 1, 1, parser->input);
	} while( (nRead == 1) && 
			 ((byte == ' ') || (byte == '\t')) );

	if(nRead == 0)
	{
		parser->lineStringLength = 0;
		return feof(parser->input) ? CFG_FILE_EOF : CFG_FILE_READ_ERROR;
	}

	// Read the rest of the line
	while( (nRead) && (byte != '\n') && (byte != '\r') )
	{
		*(ptr++) = byte;
		
		if(ptr == end)
		{
			CFG_ERR err = ResizeLineBuffer(parser, parser->lineBufferSize * 2);
			if(err != CFG_OK)
			{
				// We are in big troubles if resize fails.
				return err;
			}
			ptr = parser->lineBuffer;
			end = parser->lineBuffer + parser->lineBufferSize;
		}

		nRead = fread(&byte, 1, 1, parser->input);
	}

	// Check if we read something
	if(ptr == parser->lineBuffer)
	{
		ptr[0] = '\0';
		parser->lineStringLength = 0;
		return feof(parser->input) ? CFG_FILE_EOF : CFG_OK;
	}

	// Remove trailing spaces
	do
	{
		--ptr;
	} while( (ptr != parser->lineBuffer) && ((*ptr == ' ') || (*ptr == '\t')) );

	parser->lineStringLength = ptr - parser->lineBuffer;
	ptr[1] = '\0';

	return feof(parser->input) ? CFG_FILE_EOF : CFG_OK;
}

/**
 * \brief Validate string and transform escaped char.
 *
 * \param parser CFG parser
 * \return \see CFG_ERR
 */
static CFG_ERR ValidateString(char* start, char* end, char **out)
{
	char *ptr = NULL;

	// Skip starting spaces
	for( ; (start != end) && ((*start == ' ') || (*start == '\t')); ++start)
	{}

	if(start == end)
	{
		return CFG_EMPTY_STRING;
	}

	// Skip trailing spaces
	for(--end ; (end != start) && ((*end == ' ') || (*end == '\t')); --end)
	{}

	// Validate the rest of the string
	for(ptr=start; (ptr <= end); ++ptr)
	{
		if(memchr(g_CFGForbiddenChar, *((unsigned char*)ptr), CFG_FORBIDDEN_CHAR_COUNT) != NULL)
		{
			return CFG_INVALID_STRING;
		}
		if(*ptr == '\\')
		{
			char *escPtr;

			if(ptr > end)
			{
				return CFG_INVALID_STRING;
			}

			escPtr = (char*)memchr(g_CFGEscapedChar, *((unsigned char*)ptr+1), CFG_ESCAPED_CHAR_COUNT);
			if(escPtr == NULL)
			{
				return CFG_INVALID_STRING;		
			}

			ptr[0] = g_CFGTranslatedChar[escPtr - g_CFGEscapedChar];
			--end;
			memmove(ptr+1, ptr+2, end-ptr);
		}
	}

	end[1] = '\0';
	*out = start;

	return CFG_OK;
}

/**
 * \brief Validate section.
 *
 * \param parser CFG parser
 * \return \see CFG_ERR
 */
static CFG_ERR ValidateSection(struct CFGParser* parser)
{
	char *ptr;
	size_t i, end; 
	CFG_ERR err = CFG_OK;

	// Look for closing delimiter
	for( end=1, ptr=parser->lineBuffer + 1;
		 (end < parser->lineStringLength) && (*ptr != ']'); 
		 ++ptr, ++end )
	{}
	
	if(end == 0)
	{
		return CFG_SECTION_MISSING_DELIMITER;
	}

	// Validate the remaing chars
	for( i=end+1, ptr=parser->lineBuffer+end+1; 
		 (i<parser->lineStringLength) && ((*ptr == ' ' ) || (*ptr == '\t'));
		 ++ptr, ++i )
	{}

	// The only valid string after a section closing delimiter is a commentary
	if((i < parser->lineStringLength) && (*ptr != ';'))
	{
		return CFG_SECTION_INVALID_DECLARATION;
	}

	err = ValidateString(parser->lineBuffer+1, parser->lineBuffer+end, &ptr);
	if(err == CFG_OK)
	{
		if(parser->payload->beginSectionCallback(parser->payload->data, ptr) <= 0)
		{
			err = CFG_SECTION_CALLBACK_FAILED;
		}
		else
		{
			++parser->sectionCount;
		}
	}

	return err;
}

/**
 * \brief Validate key/value.
 *
 * \param parser CFG parser
 * \return \see CFG_ERR
 */
static CFG_ERR ValidateKeyValue(struct CFGParser* parser)
{
	char *ptr, *key, *value;
	size_t separatorPos, end; 
	CFG_ERR err = CFG_OK;

	// Look for equal sign
	for( separatorPos=0, ptr=parser->lineBuffer;
		 (separatorPos < parser->lineStringLength) && (*ptr != '='); 
		 ++ptr, ++separatorPos )
	{}
	
 	if((separatorPos == 0) || (separatorPos == parser->lineStringLength))
	{
		return CFG_MISSING_SEPARATOR;
	}

	// Validate key
	err = ValidateString(parser->lineBuffer, parser->lineBuffer+separatorPos, &key);
	if(err != CFG_OK)
	{
		return err;
	}
	
	// Look for comments
	for(end=separatorPos+1, ptr=parser->lineBuffer+separatorPos+1; (end<parser->lineStringLength) && (ptr[1] != ';'); ++end, ++ptr)
	{}

	// Validate value
	err = ValidateString(parser->lineBuffer+separatorPos+1, parser->lineBuffer+end+1, &value);
	if(err == CFG_OK)
	{
		if(parser->payload->keyValueCallback(parser->payload->data, key, value) <= 0)
		{
			err = CFG_KEY_VALUE_CALLBACK_FAILED;
		}
	}

	return err;
}

/**
 * \brief Parse CFG file
 *
 * \param filename CFG/Ini filename.
 * \param payload User defined callbacks and data.
 * \return \see CFG_ERR
 */
CFG_ERR ParseCFG(const char* filename, struct CFGPayload* payload)
{
	FILE *input;
	struct CFGParser parser;
	CFG_ERR err;

	if(payload == NULL)
	{
		return CFG_INVALID_PARAMETERS;
	}

	if( (payload->data == NULL) || 
		(payload->beginSectionCallback == NULL) ||
		(payload->endSectionCallback == NULL) ||
		(payload->keyValueCallback == NULL) )
	{
		return CFG_INVALID_PARAMETERS;
	}

	input = fopen(filename, "rb");
	if(input == NULL)
	{
		return CFG_FILE_OPEN_ERR;
	}
	
	parser.payload          = payload;
	parser.lineBuffer       = NULL;
	parser.lineBufferSize   = 0;
	parser.lineStringLength = 0;
	parser.input            = input;
	parser.sectionCount     = 0;

	err = ResizeLineBuffer(&parser, 256);
	
	payload->line = 0;

	if(err == CFG_OK)
	{
		while(err == CFG_OK)
		{
			++payload->line;
			err = ReadLine(&parser);

			if(((err == CFG_OK) || (err == CFG_FILE_EOF)) && (parser.lineStringLength))
			{
				if(parser.lineBuffer[0] == '[')
				{
					if(parser.sectionCount)
					{
						if(payload->endSectionCallback(payload->data) <= 0)
						{
							err = CFG_SECTION_CALLBACK_FAILED;
						}
					}
					if(err == CFG_OK)
					{
						err = ValidateSection(&parser);
					}
				}
				else if(parser.lineBuffer[0] != ';')
				{
					err = ValidateKeyValue(&parser);
				}
			}
		}

		if((err == CFG_FILE_EOF) && (parser.sectionCount))
		{
			err = CFG_OK;
			
			if(payload->endSectionCallback(payload->data) <= 0)
			{
				err = CFG_SECTION_CALLBACK_FAILED;
			}
		}

		CleanupCFGParser(&parser);
	}

	fclose(input);
	return err;
}
