#include "array.h"

/**
 * Create and initialize array structure.
 * \param [out] array Array to be initialized.
 * \param [in] elmntSize Element size.
 * \return ARRAY_OK upon success.
 */
ARRAY_ERR ArrayCreate(Array* array, size_t elmntSize)
{
	array->buffer = NULL;
	array->elementSize = elmntSize;
	array->count = 0;
	array->capacity = 0;
	return ARRAY_OK;
}
 
/**
 * Release array memory.
 * \param [in] array Array to be destroyed.
 */
void ArrayDestroy(Array* array)
{
	if(array->buffer != NULL)
	{
		free(array->buffer);
		array->buffer = NULL;
	}
	array->elementSize = 0;
	array->count = 0;
	array->capacity = 0;
}
 
/**
 * Expand buffer storage capacity.
 * \param [out] array Array
 * \param [in] count Number of elements to add to storage.
 * \return ARRAY_OK upon success.
 */
ARRAY_ERR ArrayGrow(Array* array, size_t count)
{
	size_t capacity = array->capacity + (count * array->elementSize);
	uint8_t *ptr = (uint8_t*)realloc(array->buffer, capacity);
	if(ptr == NULL)
	{
		return ARRAY_MEMALLOC_ERROR;
	}
	
	array->capacity = capacity;
	array->buffer   = ptr;
	
	return ARRAY_OK;
}

/**
 * Add an element at the end of the array.
 * \param [out] array Array
 * \param [in] elmnt Element to add at the end of the buffer
 * \return ARRAY_OK upon success.
 */
ARRAY_ERR ArrayPush(Array* array, uint8_t *elmnt)
{
	ARRAY_ERR err = ARRAY_OK;
	size_t lastElementOffset = array->count * array->elementSize;
	if((lastElementOffset + array->elementSize) >= array->capacity)
	{
		err = ArrayGrow(array, array->count);
		if(err != ARRAY_OK)
		{
			return err;
		}
	}
	
	memcpy(array->buffer + lastElementOffset, elmnt, array->elementSize);
	++array->count;

	return err;
}
 
/**
 * Remove last element.
 * \param [out] array Array
 * \return ARRAY_OK upon success.
 */
ARRAY_ERR ArrayPop(Array* array)
{
	if(array->count == 0)
	{
		return ARRAY_EMPTY;
	}
	--array->count;
	return ARRAY_OK;
}
 
/**
 * Flush array.
 * \note This will not release storage memory. It will just release
 * the number of elements stored.
 * \param [out] array Array
 */
void ArrayReset(Array* array)
{
	array->count = 0;
}

/**
 * Get element at a given index.
 * \param [in] array Array.
 * \param [in] index Element index.
 * \return A pointer to the element or NULL if the index is invalid.
 **/
void* ArrayAt(Array* array, size_t index)
{
	if(index >= array->count)
	{
		return NULL;
	}
	return array->buffer + (index * array->elementSize);
}
