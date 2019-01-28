/*
    This file is part of Etripator,
    copyright (c) 2009--2019 Vincent Cruz.

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
#ifndef ETRIPATOR_DECODE_H
#define ETRIPATOR_DECODE_H

#include "config.h"
#include "labels.h"
#include "section.h"
#include "memorymap.h"

/**
 *
 */
int label_extract(section_t *section, memmap_t *map, label_repository_t *repository);

/**
 *
 */
int data_extract(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository);

// [todo] output : int data_extract(section_t *section, memmap_t *map, label_repository_t *repository);

#endif // ETRIPATOR_DECODE_H
