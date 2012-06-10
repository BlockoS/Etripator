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
#include "hashtable.h"
#include "random.h"

#define MAX_LEVEL 31
#define HASH_SEED 67295

/** \brief Murmur3 hash function
 * code from http://www.team5150.com/~andrew/noncryptohashzoo/Murmur3.html
 */
uint32_t Murmur3( const char *key, size_t len, uint32_t seed )
{
	// TODO _rotl under gcc?
    #define mmix3(h,k) do { k *= m1; k = _rotl(k,r1); k *= m2; h *= 3; h ^= k; } while(0);
 
    const uint32_t m1 = 0x0acffe3d, m2 = 0x0e4ef5f3, m3 = 0xa729a897;
    const uint32_t r1 = 11, r2 = 18, r3 = 18;
    const uint8_t *tail;

    uint32_t h = len + seed, k = 0;
 
    const uint32_t *dwords = (const uint32_t *)key;
    while( len >= 4 ) {
        k = *dwords++;
        mmix3(h,k);
        len -= 4;
    }
 
    tail = (const uint8_t *)dwords;
    switch( len )
	{
        case 3: k ^= tail[2] << 16;
        case 2: k ^= tail[1] << 8;
        case 1: k ^= tail[0];
            mmix3(h,k);
    };
 
    h ^= h >> r2;
    h *= m3;
    h ^= h >> r3;
    return h;
}

/** \brief Compute node level by virtually flipping a coin */
static int RandomLevel(void)
{
	static 
	unsigned long u;
	int level;

#if defined(_MSC_VER)
	unsigned long index;
	unsigned char ret;
#endif

	while( !(u = CMWC4096()) )
	{}

	// TODO : platform specific code
#if defined(_MSC_VER)
	ret   = _BitScanForward(&index, u);
	level = (index & 0xffffffff);
#else
	level = __builtin_ctz(u) - 1;
#endif
	return level;
}

/** \brief Create a node for current key,value. */
static SkipListNode* SLCreateNode(int level, const char* key, size_t keyLen, uintptr_t value)
{
	size_t nodeSize = sizeof(SkipListNode) + (level+1) * sizeof(SkipListNode) + keyLen;
	SkipListNode *node = (SkipListNode*)malloc(nodeSize);
	
	if(node == NULL)
	{
		return NULL;
	}

	node->level = level;
	node->value = value;
	node->next  = (SkipListNode**)(node + 1);
	node->key   = (char*)(node->next + (level+1));
	node->hash  = 0;

	memset(node->next, 0, (level+1) * sizeof(SkipListNode));
	memcpy(node->key, key, keyLen);

	return node;
}

/**
 * \brief Create/Initialize skip list.
 *
 * \param list  Skip list to initialize.
 * \return HASHTABLE_OK if the skip list was successfully created.
 *         HASHTABLE_MEMORY_ISSUE if an error occured.
 */
HASHTABLE_ERR SLCreate(SkipList* list)
{
	list->maxLevel = MAX_LEVEL;
	list->level    = 0;

	list->head = SLCreateNode(list->maxLevel, " ", 2, 0);
	if(list->head == NULL)
	{
		return HASHTABLE_MEMORY_ISSUE;
	}
	return HASHTABLE_OK;
}

/**
 * \brief Release memory used by the current skip list.
 *
 * \param list  Skip list to destroy.
 */
void SLDestroy(SkipList* list)
{
	SkipListNode *current, *next;

	for(current=list->head; current!=NULL;)
	{
		next = current->next[0];
		free(current);
		current = next;
	}

	list->head = NULL;
	list->maxLevel = 0;
	list->level    = 0;
}

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
HASHTABLE_ERR SLAdd(SkipList* list, const char* key, size_t keyLen, uintptr_t value)
{
	SkipListNode *update[MAX_LEVEL];
	SkipListNode *current = list->head, *node;
	uint32_t hash;
	int i, level;

	memset(update, 0, MAX_LEVEL * sizeof(SkipListNode*));

	hash = Murmur3(key, keyLen, HASH_SEED);

	for(i=current->level-1; i>=0; i--)
	{
		while((current->next[i] != NULL) && (current->next[i]->hash < hash))
		{
			current = current->next[i];
		}
		update[i] = current;
	}

	if((current->next[0] != NULL) && (current->next[0]->hash == hash))
	{
		return HASHTABLE_ID_FOUND;
	}

	level = RandomLevel();
	if(level > list->level)
	{
		update[list->level++] = list->head;
		level = list->level;
	}

	node  = SLCreateNode(level, key, keyLen, value);
	if(node == NULL)
	{
		return HASHTABLE_MEMORY_ISSUE;
	}
	node->hash = hash;
	
	for(i=0; i<=level; i++)
	{
		node->next[i] = update[i]->next[i];
		update[i]->next[i] = node;
	}

	return HASHTABLE_OK;
}

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
HASHTABLE_ERR SLDelete(SkipList* list, const char* key, size_t keyLen, uintptr_t *value)
{
	SkipListNode *update[MAX_LEVEL];
	SkipListNode *current = list->head;
	uint32_t hash;
	int i;

	memset(update, 0, MAX_LEVEL * sizeof(SkipListNode*));

	hash = Murmur3(key, keyLen, HASH_SEED);
	
	for(i=current->level-1; i>=0; i--)
	{
		while((current->next[i] != NULL) && (current->next[i]->hash < hash))
		{
			current = current->next[i];
		}
		update[i] = current;
	}

	current = current->next[0];

	if((current != NULL) && (current->hash == hash))
	{
		*value = current->value;

		for(i=0; i<list->level; i++)
		{
			if(update[i]->next[i] != current)
				break;
			update[i]->next[i] = current->next[i];
		}

		free(current);

		for(; (list->level) && (list->head->next[list->level] == NULL); list->level--)
		{}

		return HASHTABLE_OK;
	}

	*value = 0;
	return HASHTABLE_UNKNOWN_ID;
}

/**
 * \brief Find the value associated to the key.
 *
 * \param list   Skip list where the value is supposed to be stored.
 * \param key    Hash key.
 * \param keyLen Length of the hash key.
 * \param result Retrieved value.
 * \return HASHTABLE_OK if the element was found.
 *         HASHTABLE_UNKNOWN_ID if the element was not found.
 */
HASHTABLE_ERR SLFind(const SkipList* list, const char* key, size_t keyLen, uintptr_t* result)
{
	SkipListNode *current;
	int i;
	uint32_t hash;

	hash = Murmur3(key, keyLen, HASH_SEED);

	current = list->head;
	for(i=list->level-1; i>=0; i--)
	{
		while((current->next[i] != NULL) && (current->next[i]->hash < hash))
		{
			current = current->next[i];
		}
	}

	current = current->next[0];
	if((current != NULL) && (current->hash == hash))
	{
		*result = current->value;
		return HASHTABLE_OK;
	}

	*result = 0;
	return HASHTABLE_UNKNOWN_ID;
}