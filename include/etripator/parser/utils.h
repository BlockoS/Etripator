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

namespace etripator {
namespace parser    {
    /// \brief Remove leading and trailing space.
    /// White space characters are the same as the ones defined by the isspace function
    /// (' ', '\f', '\v', '\t', '\r', '\n'). 
    /// \param [in]  value    Input string.
    /// \param [out] start    Index of the first non-space character.
    /// \param [out] end      Index of the last  non-space character.
    void chomp(const std::string& input, size_t& start, size_t& end);

    /// \brief Parse JEDEC size string.
    ///
    /// The string must match the following regex <b>([0-9]+)([KMGTPEZY]?)([bB]?)</b>.\n
    /// Where the second match is the decimal multiplier and the last match is
    /// the bit multiplier. \n
    /// For example "256Kb" is 256 kylobits (256*1024*1 bits), and "256KB" is 
    /// 256 kylobytes (256*1024*8 bits). \n
    /// Here is the decimal multiplier table for bytes. \n
    /// |  Value             | String | Name   |
    /// | :----------------: | :----: | :----: |
    /// | 1024               | K      | kilo   |
    /// | 1024<SUP>2</SUP>   | M      | mega   |
    /// | 1024<SUP>3</SUP>   | G      | giga   |
    /// | 1024<SUP>4</SUP>   | T      | tera   |
    /// | 1024<SUP>5</SUP>   | P      | peta   |
    /// | 1024<SUP>6</SUP>   | E      | exa    |
    /// | 1024<SUP>7</SUP>   | Z      | zetta  |
    /// | 1024<SUP>8</SUP>   | Y      | yotta  |
    /// As the value is stored in 64 bit, Zetta and Yotta are not supported. \n
    /// If no multiplier is specified, the size is considered to be in bits.
    /// \param [in]  value  String to parse.
    /// \param [out] size   Decoded size (in bits).
    /// \return false if the parsing failed. 
    bool parseSize(const std::string& value, size_t& size);
} // parser
} // etripator

#endif // ETRIPATOR_PARSER_UTILS_H
