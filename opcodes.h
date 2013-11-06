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
#ifndef OPCODES_H
#define OPCODES_H

struct parameter_
{
	unsigned int type;
	unsigned int size;
};
typedef struct parameter_ Parameter;

/**
 * Opcode.
 */
struct opcode_ {
  const char    *name; /**< Name. */
  unsigned int  size;  /**< Size in bytes. */
  unsigned int  type;  /**< Type. Select which decoder to use. */
};
typedef struct opcode_ Opcode;

/**
 * Decoder context.
 */
struct decoderContext_
{
	uint8_t  *buffer;   /**< Byte buffer. */
	size_t   size;      /**< Buffer size in bytes. */
	off_t    current;   /**< Current offset in byte buffer. */	
};
typedef struct decoderContext_ DecoderContext;

struct architecture_;
typedef struct architecture_ Architecture;

typedef int (*OpcodeDecoderProc)(Opcode *op, Architecture* arch, DecoderContext *ctx);

/**
 * Architecture.
 */
struct architecture_ {
	size_t opcodesCount;
	Opcode *opcodes;
	size_t decodersCount;
	OpcodeDecoderProc *decoders;
};

#endif // OPCODES_H
