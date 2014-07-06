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
#ifndef ETRIPATOR_PARSER_UTILS_H
#define ETRIPATOR_PARSER_UTILS_H

#include <string>

namespace Etripator {
    /// Parse size string.
    /// The string must match the following regex ([0-9]+)([bB]?)
    /// The character following the number is the size multiplier.
    ///     - 'b' or none: the size is in bits (multiplier=1).
    ///     - 'B': the size is in bytes (multiplier=8).
    /// \param [in]  value  String to parse.
    /// \param [out] size   Decoded size (in bits).
    /// \return false if the parsing failed.
    bool parseSize(const std::string& value, size_t &size);
}

#endif // ETRIPATOR_PARSER_UTILS_H
