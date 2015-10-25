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
#include "message.h"
#include "decode.h"
#include "opcodes.h"

/**
 * Initialize decoder.
 * \param [out] decoder Decoder.
 * \return 1 upon success, 0 if an error occured.
 */
int initializeDecoder(Decoder *decoder)
{
    decoder->section = NULL;
    decoder->memmap  = NULL;
    decoder->out     = NULL;

    decoder->physicalAddr = 0;
    decoder->logicalAddr = 0;
    decoder-> offset = 0;

    decoder->buffer = NULL;

    decoder->labelRepository = createLabelRepository();
    if(NULL == decoder->labelRepository)
    {
        ERROR_MSG("An error occured while creating label repository.");
        return 0;
    }
    return 1;
}

/**
 * Reset decoder.
 * \param [in]  memmap  Memory map.
 * \param [in]  out     File output.
 * \param [in]  section Current section being decoded.
 * \param [out] decoder Decoder.
 */
void resetDecoder(MemoryMap* memmap, FILE *out, Section *section, Decoder *decoder)
{
    decoder->section = section;
    decoder->memmap  = memmap;
    decoder->out     = out;

    decoder->physicalAddr = (section->bank << 13) | (section->org & 0x1fff);
    decoder->logicalAddr  = section->org;
    decoder->offset = 0;
}
  
/**
 * Delete decoder.
 * \param [in,out] decoder Decoder.
 */
void deleteDecoder(Decoder *decoder)
{
    if(NULL != decoder->buffer)
    {
        free(decoder->buffer);
        decoder->buffer = NULL;
    }
    if(NULL != decoder->labelRepository)
    {
        deleteLabelRepository(decoder->labelRepository);
        decoder->labelRepository = NULL;
    }
}

/**
 * Process data section
 * \param [in,out] decoder Decoder.
 * \return 1 upon success, 0 if an error occured.
 */
int processDataSection(Decoder *decoder)
{
    uint8_t data;
    int     i;
    off_t   addr;
    if(decoder->section->type == INC_DATA)
    {
        int j;
        fprintf(decoder->out, "%s:\n", decoder->section->name);
        for(i=decoder->section->size, addr=decoder->physicalAddr; i>0; )
        {
            fprintf(decoder->out, "    .db ");

            for(j=0; (i>0) && (j<8); j++, i--)
            {
                data = readByte(decoder->memmap, addr++);
                fprintf(decoder->out, "$%02x%c", data, ((j<7) && (i>1)) ? ',' : '\n');
            }
        }
    }
    else
    {
        for(i=decoder->section->size, addr=decoder->physicalAddr; i>0; i--)
        {
            data = readByte(decoder->memmap, addr++);
            fwrite(&data, 1, 1, decoder->out);
        }
    }
    return 1;
}

/**
 * Parse section to identify potential labels 
 * \param [in,out] decoder Decoder.
 */
int extractLabels(Decoder *decoder)
{
    int eor, i;
    uint8_t inst;
    uint8_t data[6];
    char buffer[32];

    size_t   offset;
    uint16_t logical;
    size_t   physical;
    
    LabelRepository *repository = decoder->labelRepository;
    Section *section = decoder->section;

    if(CODE != section->type)
    {
        return 1;
    }
    
    offset   = 0;
    logical  = decoder->logicalAddr;
    physical = decoder->physicalAddr;

    printf("\n%s:\n", section->name);

    /* Push the section org */
    if(0 == addLabel(repository, section->name, logical, physical))
    {
        return 0;
    }
    /* Walk along section */
    eor = 0;
    while(!eor)
    {
        /* Read instruction */
        inst = readByte(decoder->memmap, physical);
        /* Read data (if any) */
        for(i=0; i<(pce_opcode[inst].size-1); i++)
        {
            data[i] = readByte(decoder->memmap, physical+i+1);
        }

        if(isLocalJump(inst))
        {
            int delta;
            /* For BBR* and BBS* displacement is stored in the 2nd byte */
            i = (((inst) & 0x0F) == 0x0F) ? 1 : 0;
            /* Detect negative number */
            if(data[i] & 128)
            {
                delta = - ((data[i] - 1) ^ 0xff);
            }
            else
            {
                delta = data[i];
            }
            delta += pce_opcode[inst].size;

            /* Create label name */
            snprintf(buffer, 32, "l%04x_%02d", logical+delta, decoder->section->id);

            /* Insert offset to repository */
            if(0 == addLabel(repository, buffer, logical+delta, physical+delta))
            {
                return 0;
            }
            
            printf("%04x short jump to %04x (%08zx)\n", logical, logical+delta, physical+delta);
        }
        else 
        {
            if(isFarJump(inst))
            {
                uint16_t jump = data[0] | (data[1] << 8);
                size_t   physicalJump = INVALID_PHYSICAL_ADDRESS;
                
                /* Create label name */
                snprintf(buffer, 32, "l%04x_%02d", jump, decoder->section->id);
                /* Try to compute physical address if the logical jump address is in the range of current logical page*/
                if((jump >> 13) == (logical >> 13))
                {
                    physicalJump = (physical & 0xffffe000) | (jump & 0x1fff);
                }
                /* Insert offset to repository */
                if(0 == addLabel(repository, buffer, jump, physicalJump))
                {
                    return 0;
                }
                
                printf("%04x long jump to %04x (%08zx) \n", logical, jump, physicalJump);
            }
        }

        logical  += pce_opcode[inst].size;
        physical += pce_opcode[inst].size;
        offset   += pce_opcode[inst].size;
        
        /* Search end of section */
        if(section->size == 0)
        {
            if((inst == 0x40) || (inst == 0x60))
            {
                section->size = offset;
                eor = 1;
            }
        }
        else if(offset >= section->size)
        {
            eor = 1;
        }       
    }
    return 1;
}

static const char* spacing = "          ";

/**
 * Process opcode
 * \param [in,out] decoder Decoder.
 */
char processOpcode(Decoder *decoder)
{
    int i, delta;
    uint8_t inst, data[6], isJump;
    char eor, *name;
    uint16_t offset;
    uint16_t logical, nextLogical;
    size_t physical;
    
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
    return eor;
}
