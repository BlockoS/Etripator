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
#ifndef STORAGE_H
#define STORAGE_H

#include "config.h"
#include "error.h"

/** \defgroup STORAGE_FLAGS Storage flag bit masks */
/** @{ */
/** Data can be read from storage. */
#define STORAGE_READ      0x00000001U
/** Data can be written to storage. */
#define STORAGE_WRITE     0x00000002U
/** Storage is volatile. */
#define STORAGE_VOLATILE  0x00000004U
/** @} */

/** Test if a given flag is set. */
#define storage_flag_test(s,bit) (((s).flag & (bit)) == (bit))

/** Test if the storage s is readable. */
#define storage_is_readable(s) storage_flag_test(s, STORAGE_READ)
/** Test if the storage s is writable. */
#define storage_is_writable(s) storage_flag_test(s, STORAGE_WRITE)
/** Test if the storage s is volatile. */
#define storage_is_volatile(s) storage_flag_test(s, STORAGE_VOLATILE)

/**
 * Generic storage structure. Can be either RAM, ROM, floppy disk...
 */
struct storage_t
{
	/** Size in bytes. */
	uint64_t size;
	/** Flag \see STORAGE_FLAGS */
	uint64_t flag;

	error_t (*read )(struct storage_t* s, uint64_t address, size_t len, uint8_t* output, size_t *nread);
	error_t (*write)(struct storage_t* s, uint64_t address, size_t len, uint8_t* input, size_t* nwritten);
};

#endif // STORAGE_H