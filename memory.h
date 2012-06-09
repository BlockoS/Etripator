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
#ifndef MEMORY_H
#define MEMORY_H

#include "config.h"

#define PCE_MPR_COUNT 8

/**
 * Memory layout structure.
 */
struct Memory_
{
	uint8_t mpr[PCE_MPR_COUNT];
};
typedef struct Memory_ Memory;

/**
 * Compute 8kB offset from logical address.
 * \param [in] addr Logical 16 bits address.
 * \return Offset in the 8kb.
 */
uint16_t GetOffset(uint16_t addr);

/**
 * Compute MPR id from logical address.
 * \param [in] addr Logical 16 bits address.
 * \return MPR id.
 */
uint8_t GetMPRId(uint16_t addr);

/**
 * Retrieve bank used by current logical address.
 * \param [in] mem Current memory layout.
 * \param [in] addr Logical 16 bits address.
 * \return Bank number.
 */
uint8_t GetBank(Memory* mem, uint16_t addr);

/**
 * Compute physical address.
 * \param [in] mem Current memory layout.
 * \param [in] addr Logical 16 bits address.
 * \return 21 bits long physical address.
 */
uint32_t GetPhysicalAddress(Memory* mem, uint16_t addr);

#endif // MEMORY_H
