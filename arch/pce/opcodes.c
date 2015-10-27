/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

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
#include "opcodes.h"

char* pce_opstring[PCE_TYPE_COUNT][PCE_ARG_COUNT] = {
  {  NULL,         NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  {  NULL,         NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "#$%02x",      NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "#$%02x, ",   "<$%02x",    NULL,     NULL,     NULL,    NULL,  NULL },
  { "#$%02x, ",   "<$%02x, X", NULL,     NULL,     NULL,    NULL,  NULL },
  { "#$%02x, ",   "$%02x",    "%02x",    NULL,     NULL,    NULL,  NULL },
  { "#$%02x, ",   "$%02x",    "%02x, X", NULL,     NULL,    NULL,  NULL },
  { "<$%02x",      NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "<$%02x, X",   NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "<$%02x, Y",   NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "[$%02x]",     NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "[$%02x, X]",  NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "[$%02x], Y",  NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "<$%02x, ",   "$%02x",    "%02x",    NULL,     NULL,    NULL,  NULL },
  { "$%02x",      "%02x",      NULL,     NULL,     NULL,    NULL,  NULL },
  { "[$%02x",     "%02x]",     NULL,     NULL,     NULL,    NULL,  NULL },
  { "$%02x",      "%02x, X",   NULL,     NULL,     NULL,    NULL,  NULL },
  { "$%02x",      "%02x, Y",   NULL,     NULL,     NULL,    NULL,  NULL },
  { "$%02x",      "%02x, ",   "$%02x",  "%02x, ", "$%02x", "%02x", NULL },
  { "l%02x",      "%02x",     "_%02x",   NULL,     NULL,    NULL,  NULL },
  { "<$%02x, ",   "l%02x",    "%02x",    "_%02x",  NULL,    NULL,  NULL },
  { "[$%02x",     "%02x, X]",  NULL,     NULL,     NULL,    NULL,  NULL },
  { "$%02x",       NULL,       NULL,     NULL,     NULL,    NULL,  NULL }
};

Opcode pce_opcode[256] = {
  /* 00 */  { "BRK ", 1 ,  0 },  /* BRK                 */  
  /* 01 */  { "ORA ", 2 , 11 },  /* ORA  (ZZ, X)        */
  /* 02 */  { "SXY ", 1 ,  0 },  /* SXY                 */
  /* 03 */  { "ST0 ", 2 ,  2 },  /* STO  #nn            */
  /* 04 */  { "TSB ", 2 ,  7 },  /* TSB  ZZ             */
  /* O5 */  { "ORA ", 2 ,  7 },  /* ORA  ZZ             */
  /* 06 */  { "ASL ", 2 ,  7 },  /* ASL  ZZ             */
  /* 07 */  { "RMB0", 2 ,  7 },  /* RMB0 ZZ             */
  /* 08 */  { "PHP ", 1 ,  0 },  /* PHP                 */
  /* O9 */  { "ORA ", 2 ,  2 },  /* ORA  #nn            */
  /* 0A */  { "ASL ", 1 ,  1 },  /* ASL  A              */
  /* 0B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 0C */  { "TSB ", 3 , 14 },  /* TRB  hhll           */
  /* 0D */  { "ORA ", 3 , 14 },  /* ORA  hhll           */
  /* 0E */  { "ASL ", 3 , 14 },  /* ASL  hhll           */
  /* 0F */  { "BBR0", 3 , 20 },  /* BBR0 ZZ, lhhll      */
  /* 10 */  { "BPL ", 2 , 19 },  /* BPL  lhhll          */
  /* 11 */  { "ORA ", 2 , 12 },  /* ORA  (ZZ), Y        */
  /* 12 */  { "ORA ", 2 , 10 },  /* ORA  (ZZ)           */
  /* 13 */  { "ST1 ", 2 ,  2 },  /* ST1  #nn            */
  /* 14 */  { "TRB ", 2 ,  7 },  /* TRB  ZZ             */
  /* 15 */  { "ORA ", 2 ,  8 },  /* ORA  ZZ, X          */
  /* 16 */  { "ASL ", 2 ,  8 },  /* ASL  ZZ, X          */
  /* 17 */  { "RMB1", 2 ,  7 },  /* RMB1 ZZ             */
  /* 18 */  { "CLC ", 1 ,  0 },  /* CLC                 */
  /* 19 */  { "ORA ", 3 , 17 },  /* ORA  hhll, Y        */
  /* 1A */  { "INC ", 1 ,  1 },  /* INC  A              */
  /* 1B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 1C */  { "TRB ", 3 , 14 },  /* TRB  hhll           */
  /* 1D */  { "ORA ", 3 , 16 },  /* ORA  hhll, X        */
  /* 1E */  { "ASL ", 3 , 16 },  /* ASL  hhll, X        */
  /* 1F */  { "BBR1", 3 , 20 },  /* BBR1 ZZ, lhhll      */
  /* 20 */  { "JSR ", 3 , 19 },  /* JSR  lhhll          */
  /* 21 */  { "AND ", 2 , 11 },  /* AND  (ZZ, X)        */
  /* 22 */  { "SAX ", 1 ,  0 },  /* SAX                 */
  /* 23 */  { "ST2 ", 2 ,  2 },  /* ST2  #nn            */
  /* 24 */  { "BIT ", 2 ,  7 },  /* BIT  ZZ             */
  /* 25 */  { "AND ", 2 ,  7 },  /* AND  ZZ             */
  /* 26 */  { "ROL ", 2 ,  7 },  /* ROL  ZZ             */
  /* 27 */  { "RMB2", 2 ,  7 },  /* RMB2 ZZ             */
  /* 28 */  { "PLP ", 1 ,  0 },  /* PLP                 */
  /* 29 */  { "AND ", 2 ,  2 },  /* AND  #nn            */
  /* 2A */  { "ROL ", 1 ,  1 },  /* ROL  A              */
  /* 2B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 2C */  { "BIT ", 3 , 14 },  /* BIT  hhll           */
  /* 2D */  { "AND ", 3 , 14 },  /* AND  hhll           */
  /* 2E */  { "ROL ", 3 , 14 },  /* ROL  hhll           */
  /* 2F */  { "BBR2", 3 , 20 },  /* BBR2 ZZ, lhhll      */
  /* 30 */  { "BMI ", 2 , 19 },  /* BMI  lhhll          */
  /* 31 */  { "AND ", 2 , 12 },  /* AND  (ZZ), Y        */
  /* 32 */  { "AND ", 2 , 10 },  /* AND  (ZZ)           */
  /* 33 */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 34 */  { "BIT ", 2 ,  8 },  /* BIT  ZZ, X          */
  /* 35 */  { "AND ", 2 ,  8 },  /* AND  ZZ, X          */
  /* 36 */  { "ROL ", 2 ,  7 },  /* ROL  ZZ             */
  /* 37 */  { "RMB3", 2 ,  7 },  /* RMB3 ZZ             */
  /* 38 */  { "SEC ", 1 ,  0 },  /* SEC                 */
  /* 39 */  { "AND ", 3 , 17 },  /* AND  hhll, Y        */
  /* 3A */  { "DEC ", 1 ,  1 },  /* DEC  A              */
  /* 3B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 3C */  { "BIT ", 3 , 16 },  /* BIT  hhll, X        */
  /* 3D */  { "AND ", 3 , 16 },  /* AND  hhll, X        */
  /* 3E */  { "ROL ", 3 , 16 },  /* ROL  hhll, X        */
  /* 3F */  { "BBR3", 3 , 20 },  /* BBR3 ZZ, lhhll      */
  /* 40 */  { "RTI ", 1 ,  0 },  /* RTI                 */
  /* 41 */  { "EOR ", 2 , 11 },  /* EOR  (ZZ, X)        */ 
  /* 42 */  { "SAY ", 1 ,  0 },  /* SAY                 */
  /* 43 */  { "TMA ", 2 ,  2 },  /* TMA  #nn            */ 
  /* 44 */  { "BSR ", 2 , 19 },  /* BSR  lhhll          */  
  /* 45 */  { "EOR ", 2 ,  7 },  /* EOR  ZZ             */
  /* 46 */  { "LSR ", 2 ,  7 },  /* LSR  ZZ             */
  /* 47 */  { "RMB4", 2 ,  7 },  /* RMB4 ZZ             */
  /* 48 */  { "PHA ", 1 ,  0 },  /* PHA                 */
  /* 49 */  { "EOR ", 2 ,  2 },  /* EOR  #nn            */
  /* 4A */  { "LSR ", 1 ,  1 },  /* LSR  A              */
  /* 4B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 4C */  { "JMP ", 3 , 19 },  /* JMP  lhhll          */
  /* 4D */  { "EOR ", 3 , 14 },  /* EOR  hhll           */
  /* 4E */  { "LSR ", 3 , 14 },  /* LSR  hhll           */
  /* 4F */  { "BBR4", 3 , 20 },  /* BBR4 ZZ, lhhll      */
  /* 50 */  { "BVC ", 2 , 19 },  /* BVC  lhhll          */
  /* 51 */  { "EOR ", 2 , 12 },  /* EOR  (ZZ),Y         */ 
  /* 52 */  { "EOR ", 2 , 10 },  /* EOR  (ZZ)           */ 
  /* 53 */  { "TAM ", 2 ,  2 },  /* TAM  #nn            */ 
  /* 54 */  { "CSL ", 1 ,  0 },  /* CSL                 */
  /* 55 */  { "EOR ", 2 ,  8 },  /* EOR  ZZ, X          */ 
  /* 56 */  { "LSR ", 2 ,  8 },  /* LSR  ZZ, X          */
  /* 57 */  { "RMB5", 2 ,  7 },  /* RMB5 ZZ             */
  /* 58 */  { "CLI ", 1 ,  0 },  /* CLI                 */
  /* 59 */  { "EOR ", 3 , 17 },  /* EOR  hhll, Y        */
  /* 5A */  { "PHY ", 1 ,  0 },  /* PHY                 */
  /* 5B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 5C */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 5D */  { "EOR ", 3 , 16 },  /* EOR  hhll, X        */
  /* 5E */  { "LSR ", 3 , 16 },  /* LSR  hhll, X        */
  /* 5F */  { "BBR5", 3 , 20 },  /* BBR5 ZZ, lhhll      */
  /* 60 */  { "RTS ", 1 ,  0 },  /* RTS                 */
  /* 61 */  { "ADC ", 2 , 11 },  /* ADC  (ZZ, X)        */
  /* 62 */  { "CLA ", 1 ,  0 },  /* CLA                 */
  /* 63 */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 64 */  { "STZ ", 2 ,  7 },  /* STZ  ZZ             */
  /* 65 */  { "ADC ", 2 ,  7 },  /* ADC  ZZ             */
  /* 66 */  { "ROR ", 2 ,  7 },  /* ROR  ZZ             */
  /* 67 */  { "RMB6", 2 ,  7 },  /* RMB6 ZZ             */
  /* 68 */  { "PLA ", 1 ,  0 },  /* PLA                 */
  /* 69 */  { "ADC ", 2 ,  2 },  /* ADC  #nn            */
  /* 6A */  { "ROR ", 1 ,  1 },  /* ROR  A              */
  /* 6B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 6C */  { "JMP ", 3 , 15 },  /* JMP  (hhll)         */
  /* 6D */  { "ADC ", 3 , 14 },  /* ADC  hhll           */
  /* 6E */  { "ROR ", 3 , 14 },  /* ROR  hhll           */
  /* 6F  */ { "BBR6", 3 , 20 },  /* BBR6 ZZ, lhhll      */
  /* 70 */  { "BVS ", 2 , 19 },  /* BVS  lhhll          */
  /* 71 */  { "ADC ", 2 , 12 },  /* ADC  (ZZ), Y        */
  /* 72 */  { "ADC ", 2 , 10 },  /* ADC  (ZZ)           */
  /* 73 */  { "TII ", 7 , 18 },  /* TII  shsl,dhdl,lhll */
  /* 74 */  { "STZ ", 2 ,  8 },  /* STZ  ZZ, X          */
  /* 75 */  { "ADC ", 2 ,  8 },  /* ADC  ZZ, X          */
  /* 76 */  { "ROR ", 2 ,  8 },  /* ROR  ZZ, X          */
  /* 77 */  { "RMB7", 2 ,  7 },  /* RMB7 ZZ             */
  /* 78 */  { "SEI ", 1 ,  0 },  /* SEI                 */
  /* 79 */  { "ADC ", 3 , 17 },  /* ADC  hhll, Y        */
  /* 7A */  { "PLY ", 1 ,  0 },  /* PLY                 */
  /* 7B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 7C */  { "JMP ", 3 , 21 },  /* JMP  (hhll, X)      */
  /* 7D */  { "ADC ", 3 , 16 },  /* ADC  hhll, X        */
  /* 7E */  { "ROR ", 3 , 16 },  /* ROR  hhll, X        */
  /* 7F */  { "BBR7", 3 , 20 },  /* BBR7 ZZ, lhhll      */
  /* 80 */  { "BRA ", 2 , 19 },  /* BRA  lhhll          */
  /* 81 */  { "STA ", 2 , 11 },  /* STA  (ZZ, X)        */
  /* 82 */  { "CLX ", 1 ,  0 },  /* CLX                 */
  /* 83 */  { "TST ", 3 ,  3 },  /* TST  #nn, ZZ        */
  /* 84 */  { "STY ", 2 ,  7 },  /* STY  ZZ             */
  /* 85 */  { "STA ", 2 ,  7 },  /* STA  ZZ             */
  /* 86 */  { "STX ", 2 ,  7 },  /* STX  ZZ             */
  /* 87 */  { "SMB0", 2 ,  7 },  /* SMB0 ZZ             */
  /* 88 */  { "DEY ", 1 ,  0 },  /* DEY                 */
  /* 89 */  { "BIT ", 2 ,  2 },  /* BIT  #nn            */
  /* 8A */  { "TXA ", 1 ,  0 },  /* TXA                 */
  /* 8B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 8C */  { "STY ", 3 , 14 },  /* STY  hhll           */
  /* 8D */  { "STA ", 3 , 14 },  /* STA  hhll           */
  /* 8E */  { "STX ", 3 , 14 },  /* STX  hhll           */
  /* 8F */  { "BBS0", 3 , 20 },  /* BBS0 ZZ, lhhll      */
  /* 90 */  { "BCC ", 2 , 19 },  /* BCC  lhhll          */
  /* 91 */  { "STA ", 2 , 12 },  /* STA  (ZZ), Y        */
  /* 92 */  { "STA ", 2 , 10 },  /* STA  (ZZ)           */
  /* 93 */  { "TST ", 4 ,  5 },  /* TST  #nn, hhll      */
  /* 94 */  { "STY ", 2 ,  8 },  /* STY  ZZ, X          */
  /* 95 */  { "STA ", 2 ,  8 },  /* STA  ZZ, X          */
  /* 96 */  { "STX ", 2 ,  9 },  /* STX  ZZ, Y          */
  /* 97 */  { "SMB1", 2 ,  7 },  /* SMB1 ZZ             */
  /* 98 */  { "TYA ", 1 ,  0 },  /* TYA                 */
  /* 99 */  { "STA ", 3 , 17 },  /* STA  hhll, Y        */
  /* 9A */  { "TXS ", 1 ,  0 },  /* TXS                 */
  /* 9B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 9C */  { "STZ ", 3 , 14 },  /* STZ  hhll           */
  /* 9D */  { "STA ", 3 , 16 },  /* STA  hhll, X        */
  /* 9E */  { "STZ ", 3 , 16 },  /* STZ  hhll, X        */
  /* 9F */  { "BBS1", 3 , 20 },  /* BBS1 ZZ, lhhll      */
  /* A0 */  { "LDY ", 2 ,  2 },  /* LDY  #nn            */
  /* A1 */  { "LDA ", 2 , 11 },  /* LDA  (ZZ, X)        */
  /* A2 */  { "LDX ", 2 ,  2 },  /* LDA  #nn            */
  /* A3 */  { "TST ", 3 ,  4 },  /* TST  #nn, ZZ, X     */
  /* A4 */  { "LDY ", 2 ,  7 },  /* LDY  ZZ             */
  /* A5 */  { "LDA ", 2 ,  7 },  /* LDA  ZZ             */
  /* A6 */  { "LDX ", 2 ,  7 },  /* LDX  ZZ             */
  /* A7 */  { "SMB2", 2 ,  7 },  /* SMB2 ZZ             */
  /* A8 */  { "TAY ", 1 ,  0 },  /* TAY                 */
  /* A9 */  { "LDA ", 2 ,  2 },  /* LDA  #nn            */
  /* AA */  { "TAX ", 1 ,  0 },  /* TAX                 */
  /* AB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* AC */  { "LDY ", 3 , 14 },  /* LDY  hhll           */
  /* AD */  { "LDA ", 3 , 14 },  /* LDA  hhll           */
  /* AE */  { "LDX ", 3 , 14 },  /* LDX  hhll           */
  /* AF */  { "BBS2", 3 , 20 },  /* BBS2 ZZ, lhhll      */
  /* B0 */  { "BCS ", 2 , 19 },  /* BCS  lhhll         */
  /* B1 */  { "LDA ", 2 , 12 },  /* LDA  (ZZ), Y        */
  /* B2 */  { "LDA ", 2 , 10 },  /* LDA  (ZZ)           */
  /* B3 */  { "TST ", 4 ,  6 },  /* TST  #nn, hhll, X   */
  /* B4 */  { "LDY ", 2 ,  8 },  /* LDY  ZZ, X          */
  /* B5 */  { "LDA ", 2 ,  8 },  /* LDA  ZZ, X          */
  /* B6 */  { "LDX ", 2 ,  9 },  /* LDX  ZZ, Y          */
  /* B7 */  { "SMB3", 2 ,  7 },  /* SMB3 ZZ             */
  /* B8 */  { "CLV ", 1 ,  0 },  /* CLV                 */
  /* B9 */  { "LDA ", 3 , 17 },  /* LDA  hhll, Y        */
  /* BA */  { "TSX ", 1 ,  0 },  /* TSX                 */
  /* BB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* BC */  { "LDY ", 3 , 16 },  /* LDY  hhll, X        */
  /* BD */  { "LDA ", 3 , 16 },  /* LDA  hhll, X        */
  /* BE */  { "LDX ", 3 , 17 },  /* LDX  hhll, Y        */
  /* BF */  { "BBS3", 3 , 20 },  /* BBS3 ZZ, lhhll      */
  /* C0 */  { "CPY ", 2 ,  2 },  /* CPY  #nn            */
  /* C1 */  { "CMP ", 2 , 11 },  /* CMP  (ZZ, X)        */
  /* C2 */  { "CLY ", 1 ,  0 },  /* CLY                 */
  /* C3 */  { "TDD ", 7 , 18 },  /* TDD  shsl,dhdl,lhlh */
  /* C4 */  { "CPY ", 2 ,  7 },  /* CPY  ZZ             */
  /* C5 */  { "CMP ", 2 ,  7 },  /* CMP  ZZ             */
  /* C6 */  { "DEC ", 2 ,  7 },  /* DEC  ZZ             */
  /* C7 */  { "SMB4", 2 ,  7 },  /* SMB4 ZZ             */
  /* C8 */  { "INY ", 1 ,  0 },  /* INY                 */
  /* C9 */  { "CMP ", 2 ,  2 },  /* CMP  #nn            */
  /* CA */  { "DEX ", 1 ,  0 },  /* DEX                 */
  /* CB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* CC */  { "CPY ", 3 , 14 },  /* CPY  hhll           */      
  /* CD */  { "CMP ", 3 , 14 },  /* CMP  hhll           */
  /* CE */  { "DEC ", 3 , 14 },  /* DEC  hhll           */
  /* CF */  { "BBS4", 3 , 20 },  /* BBS4 ZZ, lhhll      */
  /* D0 */  { "BNE ", 2 , 19 },  /* BNE  lhhll          */
  /* D1 */  { "CMP ", 2 , 12 },  /* CMP  (ZZ), Y        */
  /* D2 */  { "CMP ", 2 , 10 },  /* CMP  (ZZ)           */
  /* D3 */  { "TIN ", 7 , 18 },  /* TIN  shsl,dhdl,lhlh */
  /* D4 */  { "CSH ", 1 ,  0 },  /* CSH                 */
  /* D5 */  { "CMP ", 2 ,  8 },  /* CMP  ZZ, X          */
  /* D6 */  { "DEC ", 2 ,  8 },  /* DEC  ZZ, X          */
  /* D7 */  { "SMB5", 2 ,  7 },  /* SMB5 ZZ             */
  /* D8 */  { "CLD ", 1 ,  0 },  /* CLD                 */
  /* D9 */  { "CMP ", 3 , 17 },  /* CMP  hhll, Y        */
  /* DA */  { "PHX ", 1 ,  0 },  /* PHX                 */
  /* DB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* DC */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* DD */  { "CMP ", 3 , 16 },  /* CMP  hhll, X        */
  /* DE */  { "DEC ", 3 , 16 },  /* DEC  hhll, X        */
  /* DF */  { "BBS5", 3 , 20 },  /* BBS5 ZZ, lhhll      */
  /* E0 */  { "CPX ", 2 ,  2 },  /* CPX  #nn            */
  /* E1 */  { "SBC ", 2 , 11 },  /* SBC  (ZZ,X)         */
  /* E2 */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* E3 */  { "TIA ", 7 , 18 },  /* TIA  shsl,dhdl,lhlh */
  /* E4 */  { "CPX ", 2 ,  7 },  /* CPX  ZZ             */
  /* E5 */  { "SBC ", 2 ,  7 },  /* SBC  ZZ             */
  /* E6 */  { "INC ", 2 ,  7 },  /* INC  ZZ             */
  /* E7 */  { "SMB6", 2 ,  7 },  /* SMB6 ZZ             */
  /* E8 */  { "INX ", 1 ,  0 },  /* INX                 */
  /* E9 */  { "SBC ", 2 ,  2 },  /* SBC  #nn            */
  /* EA */  { "NOP ", 1 ,  0 },  /* NOP                 */
  /* EB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* EC */  { "CPX ", 3 , 14 },  /* CPX  hhll           */
  /* ED */  { "SBC ", 3 , 14 },  /* SBC  hhll           */
  /* EE */  { "INC ", 3 , 14 },  /* INC  hhll           */
  /* EF */  { "BBS6", 3 , 20 },  /* BBS6 ZZ, lhhll      */  
  /* F0 */  { "BEQ ", 2 , 19 },  /* BEQ  lhhll          */
  /* F1 */  { "SBC ", 2 , 12 },  /* SBC  (ZZ), Y        */
  /* F2 */  { "SBC ", 2 , 10 },  /* SBC  (ZZ)           */
  /* F3 */  { "TAI ", 7 , 18 },  /* TAI  shsl,dhdl,lhlh */
  /* F4 */  { "SET ", 1 ,  0 },  /* SET                 */ 
  /* F5 */  { "SBC ", 2 ,  8 },  /* SBC  ZZ, X          */
  /* F6 */  { "INC ", 2 ,  8 },  /* INC  ZZ, X          */
  /* F7 */  { "SMB7", 2 ,  7 },  /* SMB7 ZZ             */
  /* F8 */  { "SED ", 1 ,  0 },  /* SED                 */
  /* F9 */  { "SBC ", 3 , 17 },  /* SBC  hhll, Y        */
  /* FA */  { "PLX ", 1 ,  0 },  /* PLX                 */
  /* FB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* FC */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* FD */  { "SBC ", 3 , 16 },  /* SBC  hhll, X        */
  /* FE */  { "INC ", 3 , 16 },  /* INC  hhll, X        */
  /* FF */  { "BBS7", 3 , 20 }   /* BBS7 ZZ, lhhll      */
};
