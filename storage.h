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
#ifndef STORAGE_H
#define STORAGE_H

#include "config.h"
#include <vector>

namespace Etripator
{
	//!
	//! Generic storage structure. Can be either RAM, ROM, floppy disk...
	//!
	class Storage
	{
		public:
			//! \defgroup STORAGE_FLAGS Storage flag bit masks
			//! @{
			//! Data can be read from storage.
			static const uint64_t READ;
			//! Data can be written to storage.
			static const uint64_t WRITE;
			//! Storage is volatile.
			static const uint64_t VOLATILE;
			//! @}
		public:
			//! Constructor
			Storage();
			//! Constructor
			//! \param [in] size  Size in bytes
			//! \param [in] flag  \see STORAGE_FLAGS
			Storage(uint64_t size, uint64_t flag);
			//! Destructor
			virtual ~Storage();

			//! Read bytes from storage
			//! \param [in]  address  Physical address.
			//! \param [in]  len      Length in bytes.
			//! \param [out] output   Output buffer.
			virtual bool Read(uint64_t address, size_t len, std::vector<uint8_t>& out) const = 0;

			//! Write bytes from storage
			//! \param [in] address  Physical address.
			//! \param [in] len      Length in bytes.
			//! \param [in] input    Input buffer.
			virtual bool Write(uint64_t address, size_t len, const std::vector<uint8_t>& input) = 0;

			//! Test if the storage s is readable.
			bool IsReadable() const;
			//! Test if the storage s is writable.
			bool IsWritable() const;
			//! Test if the storage s is volatile.
			bool IsVolatile() const;

			//! Storage size in bytes.
			uint64_t Size() const;

		protected:
			//! Size in bytes.
			uint64_t m_size;
			//! Flag \see STORAGE_FLAGS
			uint64_t m_flag;
	};
};

#endif // STORAGE_H