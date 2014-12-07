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
#include <etripator/parser/utils.h>

namespace etripator {
namespace parser    {
    /// \brief Remove leading and trailing space.
    /// White space characters are the same as the ones defined by the isspace function
    /// (' ', '\f', '\v', '\t', '\r', '\n'). 
    /// \param [in]  value    Input string.
    /// \param [out] start    Index of the first non-space character.
    /// \param [out] end      Index of the last  non-space character.
    void chomp(const std::string& input, size_t& start, size_t& end)
    {
        size_t len = input.size();
        // Chomp string.
        for(start=0; (start<len) && isspace(input[start]); start++)
        {}
        for(end=len-1; (end>0) && isspace(input[end]); end--)
        {}
    }
    // Multiplier configuration.
    struct MultiplierConfig
    {    char keyword;
         size_t value;
    };
    // Bit multiplier configuration.
    //    B : bit    ( 1)
    //    b : byte   ( 8)
    #define BIT_MULCFG_COUNT 2
    static const MultiplierConfig g_bitConfig[BIT_MULCFG_COUNT] = 
    {
        {'b', 1},
        {'B', 8}
    };
    // Decimal multiplier configuration.
    //    K : 1024
    //    M : 1024^2
    //    G : 1024^3
    //    T : 1024^4
    //    P : 1024^5
    //    E : 1024^6
    //    Z : 1024^7
    //    Y : 1024^8
    #define DEC_MULCFG_COUNT 6
    static const MultiplierConfig g_decimalConfig[DEC_MULCFG_COUNT] = 
    {
      {'K', 0x400},
      {'M', 0x100000},
      {'G', 0x40000000},
      {'T', 0x10000000000},
      {'P', 0x4000000000000},
      {'E', 0x1000000000000000}
      // Below this point we hit the 64 bits limit.
      // {'Z', 0x400000000000000000},
      // {'Y', 0x10000000000000000000}
    };
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
    bool parseSize(const std::string& value, size_t& size)
    {
        size_t start, end;
        // Chomp string.
        chomp(value, start, end);
        // Check for empty string.
        if((start == value.size()) || (start > end))
        {
            return false;
        }
	size_t offset;
        // Parse number.
        for(size=0, offset=start; offset<=end; offset++)
        {
            if((value[offset]<'0') || (value[offset]>'9'))
            {
                break;
            }
            size = (size*10) + (static_cast<uint8_t>(value[offset]) - '0');
        }
	// We must have read a number.
	if(offset == start)
	{
	    return false;
	}
        // Parse decimal multiplier.
	for(size_t i=0; (offset<=end) && (i<DEC_MULCFG_COUNT); i++)
	{
	    if(value[offset] == g_decimalConfig[i].keyword)
	    {
	        size *= g_decimalConfig[i].value;
		offset++;
                break;
	    }
	} 
	// Parse bit mulplier.
        for(size_t i=0; (offset<=end) && (i<BIT_MULCFG_COUNT); i++)
        {
            if(value[offset] == g_bitConfig[i].keyword)
            {
                size *= g_bitConfig[i].value;
		offset++;
		break;
            }
        }
	// There must be not char left is the string is valid.
	if(offset <= end)
	{
            return false;
	}
	return true;
    }
} // parser
} // etripator

