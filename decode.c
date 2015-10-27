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

    decoder->automatic = 0;

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
