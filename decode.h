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
#ifndef DECODE_H
#define DECODE_H

#include "config.h"
#include "labels.h"
#include "section.h"
#include "memorymap.h"

/**
 * Decoder.
 */
typedef struct
{
     /** Section being processed. */
    Section *section;
    /** Memory map. */
    MemoryMap *memmap;
    /** Output. */
    FILE *out;
    /** Current physical address. */
    size_t physicalAddr;
    /** Curren logical address. */
    uint16_t logicalAddr;
    /** Current offset (number of bytes processed so far). */
    size_t offset;
    /** Label repository. */
    LabelRepository* labelRepository;
    /** Data buffer
     *  Used for raw binary data output.
     */
    uint8_t *buffer;
} Decoder;

/**
 * Initialize decoder.
 * \param [out] decoder Decoder.
 * \return 1 upon success, 0 if an error occured.
 */
int initializeDecoder(Decoder *decoder);

/**
 * Reset decoder.
 * \param [in]  memmap  Memory map.
 * \param [in]  out     File output.
 * \param [in]  section Current section being decoded.
 * \param [out] decoder Decoder.
 */
void resetDecoder(MemoryMap* memmap, FILE *out, Section *section, Decoder *decoder);

/**
 * Delete decoder.
 * \param [in,out] decoder Decoder.
 */
void deleteDecoder(Decoder *decoder);

/**
 * Process data section
 * \param [in,out] decoder Decoder.
 * \return 1 upon success, 0 if an error occured.
 */
int processDataSection(Decoder *decoder);

/**
 * Parse section to identify potential labels 
 * \param [in,out] decoder Decoder.
 */
int extractLabels(Decoder *decoder);

/**
 * Process opcode
 * \param [in,out] decoder Decoder.
 */
char processOpcode(Decoder *decoder);

#endif // DECODE_H
