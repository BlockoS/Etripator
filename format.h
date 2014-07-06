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
#ifndef ETRIPATOR_FORMAT_H
#define ETRIPATOR_FORMAT_H

#include <string>
#include <vector>
#include <utility>

namespace Etripator
{
    /// Instruction format.
    /// Enumerates the number and the size of an instruction and its arguments.
    class Format
    {
        public:
            /// Constructor.
            Format();
            /// Destructor.
            ~Format();
            /// Clear format.
            void clear();
            /// Add argument.
            /// \param [in] name     Argument name.
            /// \param [in] bitSize  Argument size in bits.
            /// \return false if the format already contains an entry with the
            ///         specified argument name.
            bool add(const std::string& name, size_t bitSize);            
            /// Set identifier.
            void identifier(const std::string& id);
            /// Format id.
            const std::string& identifier() const;
            /// Argument count.
            size_t argumentCount() const;
            /// Retrieve argument.
            /// \param [in]  i       Argument index.
            /// \param [out] name    Argument name.
            /// \param [out] bitSize Argument size in bits.
            /// \return false if the index is out of bound.
            bool argument(size_t i, std::string& name, size_t& bitSize) const;
            /// Retrieve size of the specified argument.
            /// \param [in]  name    Argument name.
            /// \param [out] bitSize Argument size in bits.
            /// \return false if the argument was not found.
            bool argument(const std::string& name, size_t& bitSize) const;
            
        protected:
            /// Identifier.
            std::string _identifier;
            /// Argument array.
            std::vector< std::pair<std::string, size_t> > _argument;
    };
}

#endif // ETRIPATOR_FORMAT_H
