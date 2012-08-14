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
#ifndef MISSING_H
#define MISSING_H

#include <inttypes.h>

/**
 * Perform a bitwise rotation.
 */
static inline uint32_t _rotl(uint32_t r, uint8_t by)
{
	__asm __volatile__ 
	(
		"rol %%cl, %%eax"
		: "=a" (r)
		: "a" (r), "c" (by)
	);
	return r;
}

#endif
