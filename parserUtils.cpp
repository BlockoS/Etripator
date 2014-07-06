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
#include "parserUtils.h"

namespace Etripator {
    // Multiplier configuration.
    //    b : bit size (1)
    //    B : byte size (8)
    static const struct
    {
        char name; 
        size_t value;
    } multiplierConfig[] =
    {
        {'b', 1}, 
        {'B', 8}
    };
    /// Parse size string.
    /// The string must match the following regex ([0-9]+)([bB]?)
    /// The character following the number is the size multiplier.
    ///     - 'b' or none: the size is in bits (multiplier=1).
    ///     - 'B': the size is in bytes (multiplier=8).
    /// \param [in]  value  String to parse.
    /// \param [out] size   Decoded size (in bits).
    /// \return false if the parsing failed.
    bool parseSize(const std::string& value, size_t &size)
    {
        size_t start, end;
        size_t len = value.size();
        // Chomp string
        for(start=0; (start<len) && isspace(value[start]); start++)
        {}
        for(end=len-1; (end>0) && isspace(value[end]); end--)
        {}
        if((start == len) || (start >= end))
        {
            return false;
        }
        // Parse number
        for(size=0; start<=end; start++)
        {
            if((value[start]<'0') || (value[start]>'9'))
            {
                break;
            }
            size = (size*10) + (value[start] - '0');
        }
        if(start != end)
        {
            return false;
        }
        // Parse multiplier
        size_t count = sizeof(multiplierConfig)/sizeof(multiplierConfig[0]);
        for(size_t i=0; i<count; i++)
        {
            if(value[start] == multiplierConfig[i].name)
            {
                size *= multiplierConfig[i].value;
                return true;
            }
        }
        return false;
    }
}

