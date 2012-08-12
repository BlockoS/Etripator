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
#ifndef ARRAY_H
#define ARRAY_H

#include "config.h"

/**
 * Array errors
 */
typedef enum
{
	ARRAY_OK = 0,          /**< Success. */
	ARRAY_MEMALLOC_ERROR,  /**< Unable to allocate memory. */
	ARRAY_EMPTY,           /**< The array is empty. */
	ARRAY_UNKNOWN_ERROR    /**< Unknown error. \note: must be the last one. */
} ARRAY_ERR;

/**
 * Base array structure
 */
struct Array_
{
	uint8_t *buffer;      /**< Data buffeR. */
	size_t   elementSize; /**< Element size. */
	size_t   count;       /**< Element count. */	
	size_t   capacity;    /**< Buffer capacity in bytes. */
};
typedef struct Array_ Array;

/**
 * Create and initialize array structure.
 * \param [out] array Array to be initialized.
 * \param [in] elmntSize Element size.
 * \return ARRAY_OK upon success.
 */
ARRAY_ERR ArrayCreate(Array* array, size_t elmntSize);
 
/**
 * Release array memory.
 * \param [in] array Array to be destroyed.
 */
void ArrayDestroy(Array* array);
 
/**
 * Expand buffer storage capacity.
 * \param [out] array Array
 * \param [in] count Number of elements to add to storage.
 * \return ARRAY_OK upon success.
 */
ARRAY_ERR ArrayGrow(Array* array, size_t count);

/**
 * Add an element at the end of the array.
 * \param [out] array Array
 * \param [in] elmnt Element to add at the end of the buffer
 * \return ARRAY_OK upon success.
 */
ARRAY_ERR ArrayPush(Array* array, uint8_t *elmnt);
 
/**
 * Remove last element.
 * \param [out] array Array
 * \return ARRAY_OK upon success.
 */
ARRAY_ERR ArrayPop(Array* array);
 
/**
 * Flush array.
 * \note This will not release storage memory. It will just release
 * the number of elements stored.
 * \param [out] array Array
 */
void ArrayReset(Array* array);

/**
 * Get element at a given index.
 * \param [in] array Array.
 * \param [in] index Element index.
 * \return A pointer to the element or NULL if the index is invalid.
 **/
void* ArrayAt(Array* array, size_t index);

#endif // ARRAY_H
