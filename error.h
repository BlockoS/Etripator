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
#ifndef ERROR_H
#define ERROR_H

#include "enum.h"

BEGIN_ENUM(error)
	ENUM(e_ok, "success")
	ENUM(e_invalid_parameter, "invalid parameter")
	ENUM(e_not_enough_memory, "not enough memory")
END_ENUM(error)

#endif // ERROR_H
