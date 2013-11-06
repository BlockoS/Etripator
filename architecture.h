/*
    This file is part of Etripator,
    copyright (c) 2009--2013 Vincent Cruz.

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
#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include "config.h"
#include "error.h"
#include "storage.h"

struct architecture_t;

/* [todo]
		- comments
		- instruction blocks
		- data blocks
		- jumps
*/

/**
 * Decode instruction from storage.
 * \param [in]  arch			Architecture.
 * \param [in]  storage_id		Storage where the instruction and its arguments will be read.
 * \param [in]  storage_offset	Offset (in byte) in the current storage.
 * \param [out] instruction		Instruction (+arg) decoded from current storage location.
 * \return Number of bytes decoded.
 */
// [todo] use error
typedef size_t (*decoder_t)(struct architecture_t *arch, off_t storage_id, off_t storage_offset, struct instruction_t* instruction);

/**
 * Opcode description.
 */
struct opcode_t
{
	/** 
	 * Instruction name.
	 * It may not necessary be unique. For example
	 * \verbatim
	 * lda #$ab
	 * lda $2000, X
	 * \verbatim
	 * are the same instruction (lda, load into register A) but they don't have the same opcode.
	 */
	const char *name;
	/** Size in bytes. */
	size_t size;
	/** Decoder. */
	decoder_t decode;
};

/**
 * Register description.
 */
struct register_t
{
	/** Register name **/
	const char *name;
	/** Size in bytes **/
	size_t size;
	/** Register data **/
	uintptr_t data;
};

/**
 * System architecture.
 */
struct architecture_t
{
	/** Number of supported instructions **/
	size_t opcode_count;
	/** Instructions **/
	struct opcode_t* opcodes;
	
	/** Number of available storages **/
	size_t storage_count;
	/** 
	 * Storages.
	 * It can be RAM, ROM, floppy disk, etc...
	 **/
	struct storage_t *storages;

	size_t register_count;
	struct register_t *registers;

	size_t max_register_size;

	size_t logical_address_size;
	size_t physical_address_size;

	int (*translate)(uintptr_t logical_address, off_t* storage_id, off_t storage_offset);
};

enum operand_type
{
	NONE     = 0,
	REGISTER = 0x80000000,
	LITERAL  = 0x40000000,
	ADDRESS  = 0x20000000,
	JUMP     = 0x10000000
};

inline int operand_is_register(uint32_t operand)
{ return (operand & REGISTER); }

inline int operand_is_literal(uint32_t operand)
{ return (operand & LITERAL); }

inline int operand_is_address(uint32_t operand)
{ return (operand & ADDRESS); }

inline int operand_is_jump(uint32_t operand)
{ return (operand & JUMP); }

struct operand_t
{
	uint32_t type;
	uint64_t data;
};

struct instruction_t
{
	off_t     id;
	uintptr_t logical_address;
	uintptr_t physical_address;
	size_t operand_count;
	struct operand_t *operand;
};


#endif // ARCHITECTURE_H