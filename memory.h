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
#ifndef MEMORY_H
#define MEMORY_H

#include "config.h"
#include <vector>

namespace Etripator
{
	//!
	//! Generic memory structure. Can be either RAM, ROM, floppy disk...
	//!
	class Memory
	{
		public:
			//! \defgroup MEMORY_ACCESS_FLAGS Memory access flag bit masks
			//! @{
			//! Data can be read from memory.
			static const uint64_t READ;
			//! Data can be written to memory.
			static const uint64_t WRITE;
			//! @}

			//! Memory wrapping mode
			//! If the current pointer is out of the memory boundary, it will
			//! either be clamped (pointer doens't move but returns 0xff),
			//! repeated (goes back to the beginning) or mirrored.
			enum WrapMode
			{
				CLAMP,
				REPEAT,
				MIRROR
			};
		public:
			//! Constructor
			Memory();
			//! Constructor
			//! \param [in] wordSize  Word size in bytes
			//! \param [in] wordCount Word count
			//! \param [in] flag      \see MEMORY_ACCESS_FLAGS
			//! \param [in] mode      Wrapping mode
			Memory(size_t wordSize, size_t wordCount, uint64_t flag, WrapMode mode);
			//! Constructor
			Memory(const Memory& mem);
			//! Destructor
			~Memory();

			//! Copy operator
			Memory& operator=(const Memory& mem);

			//! Read bytes from memory
			//! \param [in]  address  Physical address.
			//! \param [in]  len      Length in bytes.
			//! \param [out] output   Output buffer.
			bool Read(uint64_t address, size_t len, std::vector<uint8_t>& out) const;

			//! Write bytes to memory
			//! \param [in] address  Physical address.
			//! \param [in] len      Length in bytes.
			//! \param [in] input    Input buffer.
			bool Write(uint64_t address, size_t len, const std::vector<uint8_t>& input);

			//! Test if the memory s is readable.
			bool IsReadable() const;
			//! Test if the memory s is writable.
			bool IsWritable() const;
			
			//! Word size in bytes.
			size_t WordSize() const;
			//! Word count.
			size_t WordCount() const;
			//! Memory size in bytes.
			uint64_t Size() const;

		protected:
			//! Word size
			size_t m_wordSize;
			//! Word count
			size_t m_wordCount;
			//! Flag \see MEMORY_ACCESS_FLAGS
			uint64_t m_access;
			//! Mode
			WrapMode m_mode;
			//! Buffer
			std::vector<uint8_t> m_buffer;
	};
};

#endif // MEMORY_H