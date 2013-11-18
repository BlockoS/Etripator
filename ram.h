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
#ifndef RAM_H
#define RAM_H

#include "storage.h"

namespace Etripator
{
	//!
	//! Random access memory.
	//!
	class RAM : public Storage
	{
		public:
			//! Constructor
			RAM(size_t wordSize, size_t wordCount);
			//! Destructor
			virtual ~RAM();
						
			//! Read bytes from storage
			//! \param [in]  address  Physical address.
			//! \param [in]  len      Length in bytes.
			//! \param [out] output   Output buffer.
			virtual bool Read(uint64_t address, size_t len, std::vector<uint8_t>& out) const;

			//! Write bytes from storage
			//! \param [in] address  Physical address.
			//! \param [in] len      Length in bytes.
			//! \param [in] input    Input buffer.
			virtual bool Write(uint64_t address, size_t len, const std::vector<uint8_t>& input);

			//! Word size in bytes.
			size_t WordSize() const;
			//! Word count.
			size_t WordCount() const;

		protected:
			//! Word size
			size_t m_wordSize;
			//! Word count
			size_t m_wordCount;
			//! Buffer
			std::vector<uint8_t> m_buffer;
	};

};

#endif // RAM_H