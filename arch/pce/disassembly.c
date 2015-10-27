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
#include "disassembly.h"

/**
 * Inialize code disassembler pass.
 * \param [in] callback_data Decoder.
 * \return 1 upon success or 0 if and error occured.
 */
static int pce_disassemblyInitialize(void *callback_data)
{
    Decoder *decoder = (Decoder*)callback_data;
    /* Sanity check. */
    if(NULL == decoder)
    {
        return 0;
    }
    if(CODE != decoder->section->type)
    {
        return 0;
    }
    /* Reset offset. */
    decoder->offset = 0;
    return 1;
}

static const char* spacing = "          ";

/**
 * Disassembly loop iteration.
 * \param [in] callback_data Decoder.
 * \return
 *     < 0 if an error occured.
 *     = 0 upon success and if no more iteration needs to be performed. 
 *     > 0 upon success and if another iteration needs to be performed.
 */
static int pce_disassemblyIterate(void *callback_data)
{
    int i, delta;
    uint8_t inst, data[6], isJump;
    char eor, *name;
    uint16_t offset;
    uint16_t logical, nextLogical;
    size_t physical;
    Decoder *decoder;

    /* Sanity check. */
    decoder = (Decoder*)callback_data;
    if(NULL == decoder)
    {
        return -1;
    }
    if(CODE != decoder->section->type)
    {
        return 0;
    }
    
    eor = 0;

    memset(data, 0, 6);

    physical = decoder->physicalAddr + decoder->offset;
    logical  = decoder->logicalAddr  + decoder->offset;

    /* Opcode */
    inst = readByte(decoder->memmap, physical);
    
    nextLogical = logical + pce_opcode[inst].size;
    
    /* Is there a label ? */
    if(findLabelByPhysicalAddress(decoder->labelRepository, physical, &name))
    {
        /* Print label*/
        fprintf(decoder->out, "%s:\n", name);
    }

    /* Front spacing */
    fwrite(spacing, 1, 10, decoder->out);
    /* Print opcode sting */
    fwrite(pce_opcode[inst].name, 1, 4, decoder->out);
    /* Add spacing */  
    fwrite(spacing, 1, 4, decoder->out);

    /* End Of Routine (eor) is set to 1 if the instruction is RTI or RTS */
    eor = ((inst == 64) || (inst == 96));

    /* Data */
    if(pce_opcode[inst].size > 1)
    {
        for(i=0; i<(pce_opcode[inst].size-1); i++)
        {
            data[i] = readByte(decoder->memmap, physical + i + 1);
        }
    }

    decoder->offset += pce_opcode[inst].size;
    logical = nextLogical;

    /* Swap LSB and MSB for words */
    if(pce_opcode[inst].size > 2)
    {
        uint8_t swap;
        /* Special case : TST, BBR*, BBS*. The first byte is zp var */
        i = ((pce_opcode[inst].type ==  3) ||
             (pce_opcode[inst].type ==  4) ||
             (pce_opcode[inst].type ==  5) ||
             (pce_opcode[inst].type ==  6) ||
             (pce_opcode[inst].type == 13) ||
             (pce_opcode[inst].type == 20)) ? 1 : 0;

        for(; i<(pce_opcode[inst].size-2); i+=2)
        {
            swap      = data[i];
            data[i]   = data[i+1];
            data[i+1] = swap;
        }
    }

    /* Handle special cases (jumps, tam/tma and unsupported opcodes ) */
    isJump = 0;
    if(isLocalJump(inst))
    {
        isJump = 1;

        i = (((inst) & 0x0F) == 0x0F) ? 1 : 0;

        /* Detect negative number */
        if(data[i] & 128)
            delta = - ((data[i] - 1) ^ 0xff);
        else
            delta = data[i];

        offset = logical + delta;
        data[i  ] = offset >> 8;
        data[i+1] = offset & 0xff;
    }
    else if(isFarJump(inst))
    {
        isJump = 1;
        /* Build offset */
        offset = (data[0] << 8) | data[1];
    }
    else 
    {
        offset = 0;
        /* Unknown instructions are output as raw data 
         * (ie) .db XX
         */
        if(pce_opcode[inst].type == 22)
        {
            data[0] = inst;
        }
    }

    if(pce_opcode[inst].type == 1)
    {
        fputc('A', decoder->out);
    }
    else if(isJump)
    {
        // [todo] This may change if we keep track of the MPR registers
        if((offset & 0xffffe000) == (logical & 0xffffe000))
        {
            findLabelByPhysicalAddress(decoder->labelRepository, (physical & 0xffffe000) | (offset & 0x1fff), &name);
        }
        else
        {
            findLabelByLogicalAddress(decoder->labelRepository, offset, &name);
        }
     
        /* BBR* and BBS* */
     	if((inst & 0x0F) == 0x0F)
        {
            fprintf(decoder->out, pce_opstring[pce_opcode[inst].type][0], data[0]);
        }
        fwrite(name, 1, strlen(name), decoder->out);
    }
    else
    {
        if((inst == 0x43) || (inst == 0x53))
        {
            /* tam and tma */
            /* Compute log base 2 of data */
            for(i=0; (i<8) && ((data[0] & 1) == 0); ++i, data[0] >>= 1)
            {}
            data[0] = i;        
        }

        /* Print data */
        if(pce_opcode[inst].type)
        {
            for(i=0; pce_opstring[pce_opcode[inst].type][i] != NULL; ++i)
            {
                fprintf(decoder->out, pce_opstring[pce_opcode[inst].type][i], data[i]);
            }
        }        
    }
    fputc('\n', decoder->out);

    if(!decoder->automatic)
    {
        if(decoder->offset >= decoder->section->size)
            eor = 1;
        else
            eor = 0;
    }
    return (1 != eor);
}

/**
 * Finalize disassembly.
 * \param [in] callback_data Decoder.
 * \return 1 upon success or 0 if and error occured.
 */
static int pce_disassemblyFinalize(void *callback_data)
{
    // [todo]
    (void)callback_data;
    return 1;
}

/**
 * Initialize disassembly pass.
 * \param [in]  decoder Decoder.
 * \param [out] pass Pass.
 * \return always 1.
 */
int initializeDisassemblyPass(Decoder *decoder, Pass *pass)
{
    pass->initialize = pce_disassemblyInitialize;
    pass->iterate    = pce_disassemblyIterate;
    pass->finalize   = pce_disassemblyFinalize;
    pass->data       = decoder;
    pass->name       = "Disassembly pass";
    return 1;
}