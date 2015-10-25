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
#include "labels.h"

/**
 * Inialize label extraction loop.
 * Returns 1 upon success or 0 if and error occured.
 */
static int pce_labelExtractorInitialize(void *callback_data)
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
    /* Push the section org */
    if(0 == addLabel(decoder->labelRepository, decoder->section->name, decoder->logicalAddr, decoder->physicalAddr))
    {
        return 0;
    }
    decoder->offset = 0;
    printf("\n%s:\n", decoder->section->name);
    return 1;
}

/**
 * [todo]
 */
static int pce_labelExtractorIterate(void *callback_data)
{
    int eor, i;

    Decoder *decoder = (Decoder*)callback_data;
    LabelRepository *repository;
    Section *section;

    uint32_t physical;
    uint16_t logical;

    uint8_t inst;
    
    uint8_t data[6];
    char buffer[32];

    /* Sanity check. */
    if(NULL == decoder)
    {
        return -1;
    }
    repository = decoder->labelRepository;
    section = decoder->section;
    if((NULL == repository) || (NULL == section))
    {
        return -1;
    }
    
    physical = decoder->physicalAddr + decoder->offset;
    logical  = decoder->logicalAddr  + decoder->offset;
    
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
            return -1;
        }
        
        printf("%04x short jump to %04x (%08x)\n", logical, logical+delta, physical+delta);
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
                return -1;
            }
            
            printf("%04x long jump to %04x (%08zx) \n", logical, jump, physicalJump);
        }
    }

    decoder->offset += pce_opcode[inst].size;
    
    /* Search end of section */
    if(section->size == 0)
    {
        if((inst == 0x40) || (inst == 0x60))
        {
            section->size = decoder->offset;
            eor = 1;
        }
    }
    else if(decoder->offset >= section->size)
    {
        eor = 1;
    }       

    return (1 == eor);
}

/**
 * [todo]
 */
static int pce_labelExtractorFinalize(void *callback_data)
{
    Decoder *decoder = (Decoder*)callback_data;
    if(NULL == decoder)
    {
        return 0;
    }
    decoder->offset = 0;
    // [todo]
    return 1;
}

/**
 * [todo]
 */
int createLabelExtractionPass(Pass *pass)
{
    pass->initialize = pce_labelExtractorInitialize;
    pass->iterate    = pce_labelExtractorIterate;
    pass->finalize   = pce_labelExtractorFinalize;
    return 1;
}