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
#include "storage.h"

namespace Etripator
{
	//! \defgroup STORAGE_FLAGS Storage flag bit masks
	//! @{
	//! Data can be read from storage.
	const uint64_t Storage::READ     = 0x00000001U;
	//! Data can be written to storage.
	const uint64_t Storage::WRITE    = 0x00000002U;
	//! Storage is volatile.
	const uint64_t Storage::VOLATILE = 0x00000004U;
	//! @}
	
	//! Constructor
	Storage::Storage()
		: m_size(0)
		, m_flag(0)
	{}
	//! Constructor
	//! \param [in] size  Size in bytes
	//! \param [in] flag  \see STORAGE_FLAGS
	Storage::Storage(uint64_t size, uint64_t flag)
		: m_size(size)
		, m_flag(flag)
	{}
	//! Destructor
	Storage::~Storage()
	{}

	//! Test if the storage s is readable.
	bool Storage::IsReadable() const
	{
		return (m_flag & Storage::READ) == Storage::READ;
	}
	//! Test if the storage s is writable.
	bool Storage::IsWritable() const
	{
		return (m_flag & Storage::WRITE) == Storage::WRITE;
	}
	//! Test if the storage s is volatile.
	bool Storage::IsVolatile() const
	{
		return (m_flag & Storage::VOLATILE) == Storage::VOLATILE;
	}
	//! Storage size in bytes.
	uint64_t Storage::Size() const
	{
		return m_size;
	}
};