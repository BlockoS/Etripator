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
#ifndef ETRIPATOR_CFG_H
#define ETRIPATOR_CFG_H

/**
 * \brief CFG parsing errors.
 */
typedef enum
{
	CFG_OK = 0,                      /*< Success. */
	CFG_INVALID_PARAMETERS,          /*< At least one of the parameter has an invalid value. */
	CFG_FILE_OPEN_ERR,               /*< An error occured while opening source file. */
	CFG_FILE_READ_ERROR,             /*< An error occured while reading source file. */
	CFG_FILE_EOF,                    /*< The source file end was reached. */
	CFG_EMPTY_STRING,                /*< Empty string. */
	CFG_INVALID_STRING,              /*< Invalid string. */
	CFG_SECTION_MISSING_DELIMITER,   /*< Closing section delimiter is missing. */
	CFG_SECTION_INVALID_DECLARATION, /*< Section declaration is invalid. */
	CFG_SECTION_CALLBACK_FAILED,     /*< Section callback failed. */
	CFG_KEY_VALUE_CALLBACK_FAILED,   /*< Key/Value callback failed. */
	CFG_MISSING_SEPARATOR,           /*< Missing equal separator. */
	CFG_MEMORY_ISSUE,                /*< Something went wrong with the memory. */
	CFG_UNKNOWN_ERROR                /*< Unknown error. \note: must be the last one. */
} CFG_ERR;

/**
 * \brief Get the message associated to current CFG parsing error.
 * \param err  Error value.
 * \return Error message.
 */
const char* GetCFGErrorMsg(CFG_ERR err);

/**
 * \brief Section start callback.
 * This callback will be called when the parser encounters a new section block.
 * \param [in] data user provided data 
 * \param [in] sectionName section name
 * \return 
 *     <=0 failure
 *     >0 success 
 */
typedef int (*CFGBeginSectionProc) (void *data, const char* sectionName);

/**
 * \brief Section end callback.
 * This callback will be called when the current section is over (ie at end of file or when a new section starts).
 * \param [in] data user provided data 
 * \return 
 *     <=0 failure
 *     >0 success 
 */
typedef int (*CFGEndSectionProc) (void *data);

/**
 * \brief Key value callback.
 * This callback is called when the parser encounters a new key/value pair.
 * \param [in] data user provided data 
 * \param [in] key key name
 * \param [in] value value name
 * \return
 *     <=0 failure
 *     >0 success 
 */
typedef int (*CFGKeyValueProc) (void *data, const char* key, const char* value);


/**
 * \brief Payload structure for CFG/Ini file parser.
 */
struct CFGPayload
{
	void *data; /**< Use data which will be passed as argument to one the callback function. */

	int  line;  /**< Current line number. */

	CFGBeginSectionProc beginSectionCallback; /**< Section start callback. */
	CFGEndSectionProc   endSectionCallback;   /**< Section end callback. */
	CFGKeyValueProc     keyValueCallback;     /**< Key value callback. */
};

/**
 * \brief Parse CFG file.
 * \param [in] filename CFG/Ini filename.
 * \param [in] payload User defined callbacks and data.
 * \return \see CFG_ERR
 */
CFG_ERR ParseCFG(const char* filename, struct CFGPayload* payload);

#endif // ETRIPATOR_CFG_H