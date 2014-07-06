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
#include "format.h"

namespace Etripator {

/// Constructor.
Format::Format()
    : _identifier()
    , _argument()
{}
/// Destructor.
Format::~Format()
{}
/// Clear format.
void Format::clear()
{
    _identifier = "";
    _argument.clear();
}
/// Add argument.
/// \param [in] name     Argument name.
/// \param [in] bitSize  Argument size in bits.
/// \return false if the format already contains an entry with the
///         specified argument name.
bool Format::add(const std::string& name, size_t bitSize)
{
    for(size_t i=0; i<_argument.size(); i++)
    {
        if(_argument[i].first == name)
        {
            return false;
        }
    }
    _argument.push_back(std::pair<std::string, size_t>(name, bitSize));
    return true;
}
/// Set identifier.
void Format::identifier(const std::string& id)
{
    _identifier = id;
}
/// Format id.
const std::string& Format::identifier() const
{
    return _identifier;
}
/// Argument count.
size_t Format::argumentCount() const
{
    return _argument.size();
}
/// Retrieve a given argument.
/// \param [in]  i       Argument index.
/// \param [out] name    Argument name.
/// \param [out] bitSize Argument size in bits.
/// \return false if the index is out of bound.
bool Format::argument(size_t i, std::string& name, size_t& bitSize) const
{
    if(i >= _argument.size())
    {
        return false;
    }
    name    = _argument[i].first;
    bitSize = _argument[i].second;
    return true;
}
/// Retrieve size of the specified argument.
/// \param [in]  name    Argument name.
/// \param [out] bitSize Argument size in bits.
/// \return false if the argument was not found.
bool Format::argument(const std::string& name, size_t& bitSize) const
{
    for(size_t i=0; i<_argument.size(); i++)
    {
        if(name == _argument[i].first)
        {
            bitSize = _argument[i].second;
            return true;
        }
    }
    return false;
}

}
