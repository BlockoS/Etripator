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
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "config.h"

/**
 * \brief Hashtable errors.
 */
typedef enum
{
	HASHTABLE_OK = 0,       /*< Success. */
	HASHTABLE_ID_FOUND,     /*< There is already an entry for the specified id. */
	HASHTABLE_UNKNOWN_ID,   /*< No match found for specified id. */
	HASHTABLE_MEMORY_ISSUE, /*< Something went wrong with the memory. */
	HASHTABLE_UNKNOWN_ERROR /*< Unknown error. \note: must be the last one. */
} HASHTABLE_ERR;

/** \brief Skip list node */
typedef struct _SkipListNode
{
	uint32_t  hash;  /**< Key hash value. */
	char      *key;  /**< String key.     */
	uintptr_t value; /**< Value.          */
	int       level; /**< Node level.     */

	struct _SkipListNode **next; /**< Array of "level" pointer to next nodes. */
} SkipListNode;

/* Skip list */
typedef struct
{
	int maxLevel;       /**< Maximal level. */
	int level;          /**< Current level. */
	SkipListNode* head; /**< List head.     */
} SkipList;

/**
 * \brief Create/Initialize skip list.
 * \param list  Skip list to initialize.
 * \return HASHTABLE_OK if the skip list was successfully created.
 *         HASHTABLE_MEMORY_ISSUE if an error occured.
 */
HASHTABLE_ERR SLCreate(SkipList* list);

/**
 * \brief Release memory used by the current skip list.
 * \param list  Skip list to destroy.
 */
void SLDestroy(SkipList* list);

/**
 * \brief Add element to the skip list.
 *
 * \param list   Skip list where the new element will be stored.
 * \param key    Hash key.
 * \param keyLen Length of the hash key.
 * \param value  Value.
 * \return HASHTABLE_OK if the entry was successfully added.
 *         HASHTABLE_ID_FOUND if there is already an entry associated to the given key.
 *         HASHTABLE_MEMORY_ISSUE if the new entry memory could not be allocated.
 */
HASHTABLE_ERR SLAdd(SkipList* list, const char* key, size_t keyLen, uintptr_t value);

/**
 * \brief Delete an element from the list.
 *
 * \param list   Skip list where the value is supposed to be stored.
 * \param key    Hash key of the element to be deleted.
 * \param keyLen Length of the hash key.
 * \param result Retrieved value.
 * \return HASHTABLE_OK if the element was successfully deleted.
 *         HASHTABLE_UNKNOWN_ID if the element was not found.
 */
HASHTABLE_ERR SLDelete(SkipList* list, const char* key, size_t keyLen, uintptr_t *value);

/**
 * \brief Find the value associated to the key.
 * \param list   Skip list where the value is supposed to be stored.
 * \param key    Hash key.
 * \param keyLen Length of the hash key.
 * \param result Retrieved value.
 * \return HASHTABLE_OK if the element was found.
 *         HASHTABLE_UNKNOWN_ID if the element was not found.
 */
HASHTABLE_ERR SLFind(const SkipList* list, const char* key, size_t keyLen, uintptr_t *result);


#endif // HASHTABLE_H
