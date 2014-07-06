/*
    This file is part of Etripator,
    copyright (c) 2009--2014 Vincent Cruz.

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
#ifndef ETRIPATOR_PARSER_H
#define ETRIPATOR_PARSER_H

#include <jansson.h>

#include "parserUtils.h"
#include "format.h"

namespace Etripator {
    /// Parse instruction format from JSON.
    /// \param [in]  id   Format identifier.
    /// \param [in]  root JSON element.
    /// \param [out] out  Decoded instruction format.
    /// \return true if the instruction format was sucessfully parsed.
    bool FormatParseJSON(const std::string& id, json_t* root, Format& out);
}

#endif // ETRIPATOR_PARSER_H
