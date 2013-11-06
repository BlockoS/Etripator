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
#include <assert.h>

#include "config.h"
#include "message.h"
#include "pce.h"

#define PCE_REGISTER_COUNT 6
#define PCE_INSTRUCTION_MAX_DATA_SIZE 6
#define PCE_TYPE_COUNT 25
#define PCE_MAX_OPERAND_COUNT 4

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
 * 19: OPC l_hhll (label, short jump)
 * 20: OPC l_hhll (label, long jump)
 * 21: OPC ZZ, l_hhll (label)
 * 22: OPC [hhll, X] 
 * 23: OPC #nn (tam/tma version)
 * 24: .db OPC (unsupported opcode output as raw binary data)
 */
static const uint32_t pce_addressing_mode_helpers[][PCE_MAX_OPERAND_COUNT] =
{
	{               NONE,              NONE,           NONE, NONE },
	{     PCE_REGISTER_A,              NONE,           NONE, NONE },
	{      PCE_IMMEDIATE,              NONE,           NONE, NONE },
	{      PCE_IMMEDIATE,     PCE_ZERO_PAGE,           NONE, NONE },
	{      PCE_IMMEDIATE,     PCE_ZERO_PAGE, PCE_REGISTER_X, NONE },
	{      PCE_IMMEDIATE,      PCE_ABSOLUTE,           NONE, NONE },
	{      PCE_IMMEDIATE,      PCE_ABSOLUTE, PCE_REGISTER_X, NONE },
	{      PCE_ZERO_PAGE,              NONE,           NONE, NONE },
	{      PCE_ZERO_PAGE,    PCE_REGISTER_X,           NONE, NONE },
	{      PCE_ZERO_PAGE,    PCE_REGISTER_Y,           NONE, NONE },
	{      PCE_ZERO_PAGE,              NONE,           NONE, NONE },
	{      PCE_ZERO_PAGE,    PCE_REGISTER_X,           NONE, NONE },
	{      PCE_ZERO_PAGE,    PCE_REGISTER_Y,           NONE, NONE },
	{      PCE_ZERO_PAGE,      PCE_ABSOLUTE,           NONE, NONE },
	{       PCE_ABSOLUTE,              NONE,           NONE, NONE },
	{       PCE_ABSOLUTE,              NONE,           NONE, NONE },
	{       PCE_ABSOLUTE,              NONE,           NONE, NONE },
	{       PCE_ABSOLUTE,    PCE_REGISTER_X,           NONE, NONE },
	{       PCE_ABSOLUTE,    PCE_REGISTER_Y,           NONE, NONE },
	{       PCE_ABSOLUTE,      PCE_ABSOLUTE,   PCE_ABSOLUTE, NONE },
	{  PCE_RELATIVE_JUMP,              NONE,           NONE, NONE },
	{  PCE_ABSOLUTE_JUMP,              NONE,           NONE, NONE },
	{      PCE_ZERO_PAGE, PCE_RELATIVE_JUMP,           NONE, NONE },
	{       PCE_ABSOLUTE,    PCE_REGISTER_X,           NONE, NONE },
	{            PCE_MPR,              NONE,           NONE, NONE },
	{               NONE,              NONE,           NONE, NONE }
};

static const struct
{
	const char *name;
	size_t size;
	off_t addressing_mode;
} pce_opcode_helper[] = 
{
  /* 00 */  { "BRK ", 1,  0 },  /* BRK                 */  
  /* 01 */  { "ORA ", 2, 11 },  /* ORA  (ZZ, X)        */
  /* 02 */  { "SXY ", 1,  0 },  /* SXY                 */
  /* 03 */  { "ST0 ", 2,  2 },  /* STO  #nn            */
  /* 04 */  { "TSB ", 2,  7 },  /* TSB  ZZ             */
  /* O5 */  { "ORA ", 2,  7 },  /* ORA  ZZ             */
  /* 06 */  { "ASL ", 2,  7 },  /* ASL  ZZ             */
  /* 07 */  { "RMB0", 2,  7 },  /* RMB0 ZZ             */
  /* 08 */  { "PHP ", 1,  0 },  /* PHP                 */
  /* O9 */  { "ORA ", 2,  2 },  /* ORA  #nn            */
  /* 0A */  { "ASL ", 1,  1 },  /* ASL  A              */
  /* 0B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 0C */  { "TSB ", 3, 14 },  /* TRB  hhll           */
  /* 0D */  { "ORA ", 3, 14 },  /* ORA  hhll           */
  /* 0E */  { "ASL ", 3, 14 },  /* ASL  hhll           */
  /* 0F */  { "BBR0", 3, 13 },  /* BBR0 ZZ, l_hhll     */
  /* 10 */  { "BPL ", 2, 19 },  /* BPL  hhll           */
  /* 11 */  { "ORA ", 2, 12 },  /* ORA  (ZZ), Y        */
  /* 12 */  { "ORA ", 2, 10 },  /* ORA  (ZZ)           */
  /* 13 */  { "ST1 ", 2,  2 },  /* ST1  #nn            */
  /* 14 */  { "TRB ", 2,  7 },  /* TRB  ZZ             */
  /* 15 */  { "ORA ", 2,  8 },  /* ORA  ZZ, X          */
  /* 16 */  { "ASL ", 2,  8 },  /* ASL  ZZ, X          */
  /* 17 */  { "RMB1", 2,  7 },  /* RMB1 ZZ             */
  /* 18 */  { "CLC ", 1,  0 },  /* CLC                 */
  /* 19 */  { "ORA ", 3, 17 },  /* ORA  hhll, Y        */
  /* 1A */  { "INC ", 1,  1 },  /* INC  A              */
  /* 1B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 1C */  { "TRB ", 3, 14 },  /* TRB  hhll           */
  /* 1D */  { "ORA ", 3, 16 },  /* ORA  hhll, X        */
  /* 1E */  { "ASL ", 3, 16 },  /* ASL  hhll, X        */
  /* 1F */  { "BBR1", 3, 13 },  /* BBR1 ZZ, l_hhll     */
  /* 20 */  { "JSR ", 3, 20 },  /* JSR  hhll           */
  /* 21 */  { "AND ", 2, 11 },  /* AND  (ZZ, X)        */
  /* 22 */  { "SAX ", 1,  0 },  /* SAX                 */
  /* 23 */  { "ST2 ", 2,  2 },  /* ST2  #nn            */
  /* 24 */  { "BIT ", 2,  7 },  /* BIT  ZZ             */
  /* 25 */  { "AND ", 2,  7 },  /* AND  ZZ             */
  /* 26 */  { "ROL ", 2,  7 },  /* ROL  ZZ             */
  /* 27 */  { "RMB2", 2,  7 },  /* RMB2 ZZ             */
  /* 28 */  { "PLP ", 1,  0 },  /* PLP                 */
  /* 29 */  { "AND ", 2,  2 },  /* AND  #nn            */
  /* 2A */  { "ROL ", 1,  1 },  /* ROL  A              */
  /* 2B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 2C */  { "BIT ", 3, 14 },  /* BIT  hhll           */
  /* 2D */  { "AND ", 3, 14 },  /* AND  hhll           */
  /* 2E */  { "ROL ", 3, 14 },  /* ROL  hhll           */
  /* 2F */  { "BBR2", 3, 13 },  /* BBR2 ZZ, l_hhll     */
  /* 30 */  { "BMI ", 2, 19 },  /* BMI  hhll           */
  /* 31 */  { "AND ", 2, 12 },  /* AND  (ZZ), Y        */
  /* 32 */  { "AND ", 2, 10 },  /* AND  (ZZ)           */
  /* 33 */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 34 */  { "BIT ", 2,  8 },  /* BIT  ZZ, X          */
  /* 35 */  { "AND ", 2,  8 },  /* AND  ZZ, X          */
  /* 36 */  { "ROL ", 2,  7 },  /* ROL  ZZ             */
  /* 37 */  { "RMB3", 2,  7 },  /* RMB3 ZZ             */
  /* 38 */  { "SEC ", 1,  0 },  /* SEC                 */
  /* 39 */  { "AND ", 3, 17 },  /* AND  hhll, Y        */
  /* 3A */  { "DEC ", 1,  1 },  /* DEC  A              */
  /* 3B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 3C */  { "BIT ", 3, 16 },  /* BIT  hhll, X        */
  /* 3D */  { "AND ", 3, 16 },  /* AND  hhll, X        */
  /* 3E */  { "ROL ", 3, 16 },  /* ROL  hhll, X        */
  /* 3F */  { "BBR3", 3, 13 },  /* BBR3 ZZ, l_hhll     */
  /* 40 */  { "RTI ", 1,  0 },  /* RTI                 */
  /* 41 */  { "EOR ", 2, 11 },  /* EOR  (ZZ, X)        */ 
  /* 42 */  { "SAY ", 1,  0 },  /* SAY                 */
  /* 43 */  { "TMA ", 2, 23 },  /* TMA  #nn            */ 
  /* 44 */  { "BSR ", 2, 19 },  /* BSR  hhll           */  
  /* 45 */  { "EOR ", 2,  7 },  /* EOR  ZZ             */
  /* 46 */  { "LSR ", 2,  7 },  /* LSR  ZZ             */
  /* 47 */  { "RMB4", 2,  7 },  /* RMB4 ZZ             */
  /* 48 */  { "PHA ", 1,  0 },  /* PHA                 */
  /* 49 */  { "EOR ", 2,  2 },  /* EOR  #nn            */
  /* 4A */  { "LSR ", 1,  1 },  /* LSR  A              */
  /* 4B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 4C */  { "JMP ", 3, 20 },  /* JMP  hhll           */
  /* 4D */  { "EOR ", 3, 14 },  /* EOR  hhll           */
  /* 4E */  { "LSR ", 3, 14 },  /* LSR  hhll           */
  /* 4F */  { "BBR4", 3, 13 },  /* BBR4 ZZ, l_hhll     */
  /* 50 */  { "BVC ", 2, 19 },  /* BVC  hhll           */
  /* 51 */  { "EOR ", 2, 12 },  /* EOR  (ZZ), Y        */ 
  /* 52 */  { "EOR ", 2, 10 },  /* EOR  (ZZ)           */ 
  /* 53 */  { "TAM ", 2, 23 },  /* TAM  #nn            */ 
  /* 54 */  { "CSL ", 1,  0 },  /* CSL                 */
  /* 55 */  { "EOR ", 2,  8 },  /* EOR  ZZ, X          */ 
  /* 56 */  { "LSR ", 2,  8 },  /* LSR  ZZ, X          */
  /* 57 */  { "RMB5", 2,  7 },  /* RMB5 ZZ             */
  /* 58 */  { "CLI ", 1,  0 },  /* CLI                 */
  /* 59 */  { "EOR ", 3, 17 },  /* EOR  hhll, Y        */
  /* 5A */  { "PHY ", 1,  0 },  /* PHY                 */
  /* 5B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 5C */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 5D */  { "EOR ", 3, 16 },  /* EOR  hhll, X        */
  /* 5E */  { "LSR ", 3, 16 },  /* LSR  hhll, X        */
  /* 5F */  { "BBR5", 3, 13 },  /* BBR5 ZZ, l_hhll     */
  /* 60 */  { "RTS ", 1,  0 },  /* RTS                 */
  /* 61 */  { "ADC ", 2, 11 },  /* ADC  (ZZ, X)        */
  /* 62 */  { "CLA ", 1,  0 },  /* CLA                 */
  /* 63 */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 64 */  { "STZ ", 2,  7 },  /* STZ  ZZ             */
  /* 65 */  { "ADC ", 2,  7 },  /* ADC  ZZ             */
  /* 66 */  { "ROR ", 2,  7 },  /* ROR  ZZ             */
  /* 67 */  { "RMB6", 2,  7 },  /* RMB6 ZZ             */
  /* 68 */  { "PLA ", 1,  0 },  /* PLA                 */
  /* 69 */  { "ADC ", 2,  2 },  /* ADC  #nn            */
  /* 6A */  { "ROR ", 1,  1 },  /* ROR  A              */
  /* 6B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 6C */  { "JMP ", 3, 15 },  /* JMP  (hhll)         */
  /* 6D */  { "ADC ", 3, 14 },  /* ADC  hhll           */
  /* 6E */  { "ROR ", 3, 14 },  /* ROR  hhll           */
  /* 6F  */ { "BBR6", 3, 13 },  /* BBR6 ZZ, l_hhll     */
  /* 70 */  { "BVS ", 2, 19 },  /* BVS  hhll           */
  /* 71 */  { "ADC ", 2, 12 },  /* ADC  (ZZ), Y        */
  /* 72 */  { "ADC ", 2, 10 },  /* ADC  (ZZ)           */
  /* 73 */  { "TII ", 7, 18 },  /* TII  shsl,dhdl,lhll */
  /* 74 */  { "STZ ", 2,  8 },  /* STZ  ZZ, X          */
  /* 75 */  { "ADC ", 2,  8 },  /* ADC  ZZ, X          */
  /* 76 */  { "ROR ", 2,  8 },  /* ROR  ZZ, X          */
  /* 77 */  { "RMB7", 2,  7 },  /* RMB7 ZZ             */
  /* 78 */  { "SEI ", 1,  0 },  /* SEI                 */
  /* 79 */  { "ADC ", 3, 17 },  /* ADC  hhll, Y        */
  /* 7A */  { "PLY ", 1,  0 },  /* PLY                 */
  /* 7B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 7C */  { "JMP ", 3, 22 },  /* JMP  (hhll, X)      */
  /* 7D */  { "ADC ", 3, 16 },  /* ADC  hhll, X        */
  /* 7E */  { "ROR ", 3, 16 },  /* ROR  hhll, X        */
  /* 7F */  { "BBR7", 3, 13 },  /* BBR7 ZZ, l_hhll     */
  /* 80 */  { "BRA ", 2, 19 },  /* BRA  hhll           */
  /* 81 */  { "STA ", 2, 11 },  /* STA  (ZZ, X)        */
  /* 82 */  { "CLX ", 1,  0 },  /* CLX                 */
  /* 83 */  { "TST ", 3,  4 },  /* TST  #nn, ZZ        */
  /* 84 */  { "STY ", 2,  7 },  /* STY  ZZ             */
  /* 85 */  { "STA ", 2,  7 },  /* STA  ZZ             */
  /* 86 */  { "STX ", 2,  7 },  /* STX  ZZ             */
  /* 87 */  { "SMB0", 2,  7 },  /* SMB0 ZZ             */
  /* 88 */  { "DEY ", 1,  0 },  /* DEY                 */
  /* 89 */  { "BIT ", 2,  2 },  /* BIT  #nn            */
  /* 8A */  { "TXA ", 1,  0 },  /* TXA                 */
  /* 8B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 8C */  { "STY ", 3, 14 },  /* STY  hhll           */
  /* 8D */  { "STA ", 3, 14 },  /* STA  hhll           */
  /* 8E */  { "STX ", 3, 14 },  /* STX  hhll           */
  /* 8F */  { "BBS0", 3, 13 },  /* BBS0 ZZ, l_hhll     */
  /* 90 */  { "BCC ", 2, 19 },  /* BCC  hhll           */
  /* 91 */  { "STA ", 2, 12 },  /* STA  (ZZ), Y        */
  /* 92 */  { "STA ", 2, 10 },  /* STA  (ZZ)           */
  /* 93 */  { "TST ", 4,  5 },  /* TST  #nn, hhll      */
  /* 94 */  { "STY ", 2,  8 },  /* STY  ZZ, X          */
  /* 95 */  { "STA ", 2,  8 },  /* STA  ZZ, X          */
  /* 96 */  { "STX ", 2,  9 },  /* STX  ZZ, Y          */
  /* 97 */  { "SMB1", 2,  7 },  /* SMB1 ZZ             */
  /* 98 */  { "TYA ", 1,  0 },  /* TYA                 */
  /* 99 */  { "STA ", 3, 17 },  /* STA  hhll, Y        */
  /* 9A */  { "TXS ", 1,  0 },  /* TXS                 */
  /* 9B */  { ".db ", 1, 24 },  /* UNUSED              */
  /* 9C */  { "STZ ", 3, 14 },  /* STZ  hhll           */
  /* 9D */  { "STA ", 3, 16 },  /* STA  hhll, X        */
  /* 9E */  { "STZ ", 3, 16 },  /* STZ  hhll, X        */
  /* 9F */  { "BBS1", 3, 13 },  /* BBS1 ZZ, l_hhll     */
  /* A0 */  { "LDY ", 2,  2 },  /* LDY  #nn            */
  /* A1 */  { "LDA ", 2, 11 },  /* LDA  (ZZ, X)        */
  /* A2 */  { "LDX ", 2,  2 },  /* LDA  #nn            */
  /* A3 */  { "TST ", 3,  4 },  /* TST  #nn, ZZ, X     */
  /* A4 */  { "LDY ", 2,  7 },  /* LDY  ZZ             */
  /* A5 */  { "LDA ", 2,  7 },  /* LDA  ZZ             */
  /* A6 */  { "LDX ", 2,  7 },  /* LDX  ZZ             */
  /* A7 */  { "SMB2", 2,  7 },  /* SMB2 ZZ             */
  /* A8 */  { "TAY ", 1,  0 },  /* TAY                 */
  /* A9 */  { "LDA ", 2,  2 },  /* LDA  #nn            */
  /* AA */  { "TAX ", 1,  0 },  /* TAX                 */
  /* AB */  { ".db ", 1, 24 },  /* UNUSED              */
  /* AC */  { "LDY ", 3, 14 },  /* LDY  hhll           */
  /* AD */  { "LDA ", 3, 14 },  /* LDA  hhll           */
  /* AE */  { "LDX ", 3, 14 },  /* LDX  hhll           */
  /* AF */  { "BBS2", 3, 13 },  /* BBS2 ZZ, l_hhll     */
  /* B0 */  { "BCS ", 2, 19 },  /* BCS  hhll           */
  /* B1 */  { "LDA ", 2, 12 },  /* LDA  (ZZ), Y        */
  /* B2 */  { "LDA ", 2, 10 },  /* LDA  (ZZ)           */
  /* B3 */  { "TST ", 4,  6 },  /* TST  #nn, hhll, X   */
  /* B4 */  { "LDY ", 2,  8 },  /* LDY  ZZ, X          */
  /* B5 */  { "LDA ", 2,  8 },  /* LDA  ZZ, X          */
  /* B6 */  { "LDX ", 2,  9 },  /* LDX  ZZ, Y          */
  /* B7 */  { "SMB3", 2,  7 },  /* SMB3 ZZ             */
  /* B8 */  { "CLV ", 1,  0 },  /* CLV                 */
  /* B9 */  { "LDA ", 3, 17 },  /* LDA  hhll, Y        */
  /* BA */  { "TSX ", 1,  0 },  /* TSX                 */
  /* BB */  { ".db ", 1, 24 },  /* UNUSED              */
  /* BC */  { "LDY ", 3, 16 },  /* LDY  hhll, X        */
  /* BD */  { "LDA ", 3, 16 },  /* LDA  hhll, X        */
  /* BE */  { "LDX ", 3, 17 },  /* LDX  hhll, Y        */
  /* BF */  { "BBS3", 3, 13 },  /* BBS3 ZZ, l_hhll     */
  /* C0 */  { "CPY ", 2,  2 },  /* CPY  #nn            */
  /* C1 */  { "CMP ", 2, 11 },  /* CMP  (ZZ, X)        */
  /* C2 */  { "CLY ", 1,  0 },  /* CLY                 */
  /* C3 */  { "TDD ", 7, 18 },  /* TDD  shsl,dhdl,lhlh */
  /* C4 */  { "CPY ", 2,  7 },  /* CPY  ZZ             */
  /* C5 */  { "CMP ", 2,  7 },  /* CMP  ZZ             */
  /* C6 */  { "DEC ", 2,  7 },  /* DEC  ZZ             */
  /* C7 */  { "SMB4", 2,  7 },  /* SMB4 ZZ             */
  /* C8 */  { "INY ", 1,  0 },  /* INY                 */
  /* C9 */  { "CMP ", 2,  2 },  /* CMP  #nn            */
  /* CA */  { "DEX ", 1,  0 },  /* DEX                 */
  /* CB */  { ".db ", 1, 24 },  /* UNUSED              */
  /* CC */  { "CPY ", 3, 14 },  /* CPY  hhll           */      
  /* CD */  { "CMP ", 3, 14 },  /* CMP  hhll           */
  /* CE */  { "DEC ", 3, 14 },  /* DEC  hhll           */
  /* CF */  { "BBS4", 3, 13 },  /* BBS4 ZZ, l_hhll     */
  /* D0 */  { "BNE ", 2, 19 },  /* BNE  hhll           */
  /* D1 */  { "CMP ", 2, 12 },  /* CMP  (ZZ), Y        */
  /* D2 */  { "CMP ", 2, 10 },  /* CMP  (ZZ)           */
  /* D3 */  { "TIN ", 7, 18 },  /* TIN  shsl,dhdl,lhlh */
  /* D4 */  { "CSH ", 1,  0 },  /* CSH                 */
  /* D5 */  { "CMP ", 2,  8 },  /* CMP  ZZ, X          */
  /* D6 */  { "DEC ", 2,  8 },  /* DEC  ZZ, X          */
  /* D7 */  { "SMB5", 2,  7 },  /* SMB5 ZZ             */
  /* D8 */  { "CLD ", 1,  0 },  /* CLD                 */
  /* D9 */  { "CMP ", 3, 17 },  /* CMP  hhll, Y        */
  /* DA */  { "PHX ", 1,  0 },  /* PHX                 */
  /* DB */  { ".db ", 1, 24 },  /* UNUSED              */
  /* DC */  { ".db ", 1, 24 },  /* UNUSED              */
  /* DD */  { "CMP ", 3, 16 },  /* CMP  hhll, X        */
  /* DE */  { "DEC ", 3, 16 },  /* DEC  hhll, X        */
  /* DF */  { "BBS5", 3, 13 },  /* BBS5 ZZ, l_hhll     */
  /* E0 */  { "CPX ", 2,  2 },  /* CPX  #nn            */
  /* E1 */  { "SBC ", 2, 11 },  /* SBC  (ZZ, X)        */
  /* E2 */  { ".db ", 1, 24 },  /* UNUSED              */
  /* E3 */  { "TIA ", 7, 18 },  /* TIA  shsl,dhdl,lhlh */
  /* E4 */  { "CPX ", 2,  7 },  /* CPX  ZZ             */
  /* E5 */  { "SBC ", 2,  7 },  /* SBC  ZZ             */
  /* E6 */  { "INC ", 2,  7 },  /* INC  ZZ             */
  /* E7 */  { "SMB6", 2,  7 },  /* SMB6 ZZ             */
  /* E8 */  { "INX ", 1,  0 },  /* INX                 */
  /* E9 */  { "SBC ", 2,  2 },  /* SBC  #nn            */
  /* EA */  { "NOP ", 1,  0 },  /* NOP                 */
  /* EB */  { ".db ", 1, 24 },  /* UNUSED              */
  /* EC */  { "CPX ", 3, 14 },  /* CPX  hhll           */
  /* ED */  { "SBC ", 3, 14 },  /* SBC  hhll           */
  /* EE */  { "INC ", 3, 14 },  /* INC  hhll           */
  /* EF */  { "BBS6", 3, 13 },  /* BBS6 ZZ, l_hhll     */  
  /* F0 */  { "BEQ ", 2, 19 },  /* BEQ  hhll           */
  /* F1 */  { "SBC ", 2, 12 },  /* SBC  (ZZ), Y        */
  /* F2 */  { "SBC ", 2, 10 },  /* SBC  (ZZ)           */
  /* F3 */  { "TAI ", 7, 18 },  /* TAI  shsl,dhdl,lhlh */
  /* F4 */  { "SET ", 1,  0 },  /* SET                 */ 
  /* F5 */  { "SBC ", 2,  8 },  /* SBC  ZZ, X          */
  /* F6 */  { "INC ", 2,  8 },  /* INC  ZZ, X          */
  /* F7 */  { "SMB7", 2,  7 },  /* SMB7 ZZ             */
  /* F8 */  { "SED ", 1,  0 },  /* SED                 */
  /* F9 */  { "SBC ", 3, 17 },  /* SBC  hhll, Y        */
  /* FA */  { "PLX ", 1,  0 },  /* PLX                 */
  /* FB */  { ".db ", 1, 24 },  /* UNUSED              */
  /* FC */  { ".db ", 1, 24 },  /* UNUSED              */
  /* FD */  { "SBC ", 3, 16 },  /* SBC  hhll, X        */
  /* FE */  { "INC ", 3, 16 },  /* INC  hhll, X        */
  /* FF */  { "BBS7", 3, 13 }   /* BBS7 ZZ, l_hhll     */
};

/** 
 * PC-Engine register names.
 */
static const char* pce_registers_name[] =
{
	"A", "X", "Y", "P", "PC", "SP"
};

/**
 * [todo]
 **/
static size_t pce_decode(struct architecture_t *arch, off_t storage_id, off_t storage_offset, struct instruction_t* instruction)
{
	error_t err;

	struct storage_t *storage;

	const uint32_t *addressing_mode;

	uint8_t buffer[PCE_INSTRUCTION_MAX_DATA_SIZE];
	uint8_t instruction_id;

	size_t nread;
	size_t current_buffer_byte;
	size_t i;

	assert(NULL != arch);
	assert(storage_id<(off_t)arch->storage_count);
	
	storage = arch->storages+storage_id;
	if(NULL == storage)
	{ 
		return e_storage_id;
	}

	// Read a single byte. This will give us the instruction.
	err = storage->read(storage, storage_offset, 1, &instruction_id, &nread);
	if(e_ok != err) { return err; }
	storage_offset += nread;

	// Read data.
	err = storage->read(storage, storage_offset, pce_opcode_helper[instruction_id].size-1, buffer, &nread);
	if(e_ok != err) { return err; }
	storage_offset += nread;
	
	instruction->id = instruction_id;
	instruction->operand = (struct operand_t*)malloc(PCE_MAX_OPERAND_COUNT * sizeof(struct operand_t));
	if(NULL == instruction->operand)
	{
		return e_not_enough_memory;
	}

	current_buffer_byte = 0;
	addressing_mode = pce_addressing_mode_helpers[pce_opcode_helper[instruction_id].addressing_mode];
	for(i=0; (i<PCE_MAX_OPERAND_COUNT) && (addressing_mode[i] != NONE); i++)
	{
		instruction->operand[i].type = addressing_mode[i];
		if(current_buffer_byte >= (pce_opcode_helper[instruction_id].size-1))
		{
			return e_operand_inconsistency;
		}
		switch(addressing_mode[i])
		{
			case PCE_IMMEDIATE:
			case PCE_ZERO_PAGE:
				instruction->operand[i].data = buffer[current_buffer_byte++];
				break;
			case PCE_MPR:
				instruction->operand[i].data = buffer[current_buffer_byte++]; // [todo]
				break;
			case PCE_ABSOLUTE:
				instruction->operand[i].data = buffer[current_buffer_byte++] | (buffer[current_buffer_byte++] << 8);
				break;
			case PCE_RELATIVE_JUMP:
				// [todo] manage label now?
				instruction->operand[i].data = buffer[current_buffer_byte++];
				break;
			case PCE_ABSOLUTE_JUMP:
				// [todo] manage label now?
				instruction->operand[i].data = buffer[current_buffer_byte++] | (buffer[current_buffer_byte++] << 8);
				break;
			default:
				break;
		};
	}
	instruction->operand_count = i;
	// [todo] instruction->logical_address
	// [todo] instruction->physical_address
	// [todo] => change storage_id/storage_offset to a more detailed structure (like an uber visitor)
	return e_ok;
}

/**
 * [todo]
 */
static error_t pce_build_opcode_list(struct architecture_t *arch)
{
	size_t i;
	// Build opcode list.
	arch->opcode_count = sizeof(pce_opcode_helper) / sizeof(pce_opcode_helper[0]);
	arch->opcodes = (struct opcode_t*)malloc(arch->opcode_count * sizeof(struct opcode_t));
	if(NULL == arch->opcodes)
	{
		return e_not_enough_memory;
	}

	for(i=0; i<arch->opcode_count; i++)
	{
		arch->opcodes[i].name   = pce_opcode_helper[i].name;
		arch->opcodes[i].size   = pce_opcode_helper[i].size;
		arch->opcodes[i].decode = pce_decode;
	}
	
	// Create registers.
	arch->register_count = PCE_REGISTER_COUNT;
	arch->registers = (struct register_t*)malloc(arch->register_count * sizeof(struct register_t));
	if(NULL == arch->registers)
	{
		return e_not_enough_memory;
	}

	for(i=0; i<arch->register_count; i++)
	{
		arch->registers[i].name = pce_registers_name[i];
		arch->registers[i].data = 0;
		arch->registers[i].size = 1;
	}

	arch->max_register_size = 1;

	// Address sizes
	arch->logical_address_size  = 16;
	arch->physical_address_size = 21;
	
	// [todo] arch->translate = ... ;
	// [todo] arch->storage_count = ... ;
	// [todo] arch->storages = ... ;
	
	return e_ok;
}

// [todo] define/create storages (hucard, ram, cdrom?)