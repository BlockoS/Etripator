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

/**
 * Opcode.
 */
struct opcode_ {
  char          name[5]; /**< Name. */
  unsigned char size;    /**< Size in bytes. */
  unsigned char type;    /**< Type \see pce_opstring. */
};
typedef struct opcode_ Opcode;

#define PCE_TYPE_COUNT 23
#define PCE_ARG_COUNT 7

/**
 * Opcode output format.
 * Opcode type : 
 *
 * 00: OPC 
 * 01: OPC A
 * 02: OPC #nn
 * 03: OPC #nn, ZZ
 * 04: OPC #nn, ZZ, X
 * 05: OPC #nn, hhll
 * 06: OPC #nn, hhll, X
 * 07: OPC ZZ
 * 08: OPC ZZ, X
 * 09: OPC ZZ, Y
 * 10: OPC (ZZ)
 * 11: OPC (ZZ, X)
 * 12: OPC (ZZ), Y
 * 13: OPC ZZ, hhll   
 * 14: OPC hhll
 * 15: OPC (hhll)
 * 16: OPC hhll, X
 * 17: OPC hhll, Y
 * 18: OPC shsl,dhdl,hhll 
 * 19: OPC l_hhll (label)
 * 20: OPC ZZ, l_hhll (label)
 * 21: OPC [hhll, X] 
 * 22: .db OPC (unsupported opcode output as raw binary data)
 */
extern char* pce_opstring[PCE_TYPE_COUNT][PCE_ARG_COUNT];

/** PC engine opcodes. */
extern Opcode pce_opcode[256];

#endif // OPCODES_H
