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
#ifndef PCE_DISASSEMBLY_H
#define PCE_DISASSEMBLY_H

#include "../../decode.h"   // [todo] absolute include
#include "../../pass.h"     // [todo] absolute include
#include "opcodes.h"

/**
 * Initialize disassembly pass.
 * \param [in]  decoder Decoder.
 * \param [out] pass Pass.
 * \return always 1.
 */
int initializeDisassemblyPass(Decoder *decoder, Pass *pass);

#endif // PCE_DISASSEMBLY_H