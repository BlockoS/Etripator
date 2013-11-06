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
#ifndef PCE_H
#define PCE_H

#include "architecture.h"

#define PCE_ZERO_PAGE_PHYSICAL_ADDRESS 0x1f0000
#define PCE_STACK_PHYSICAL_ADDRESS     0x1f0100
#define PCE_BSS_PHYSICAL_ADDRESS       0x1f0200

enum pce_operand_type
{
	PCE_REGISTER_A    = REGISTER,
	PCE_REGISTER_X,
	PCE_REGISTER_Y,
	PCE_IMMEDIATE     = LITERAL,
	PCE_MPR,
	PCE_ZERO_PAGE     = ADDRESS,
	PCE_ABSOLUTE,
	PCE_RELATIVE_JUMP = JUMP,
	PCE_ABSOLUTE_JUMP,
};

#endif // PCE_H
