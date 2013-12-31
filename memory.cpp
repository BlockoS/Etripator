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
#include "memory.h"
// [todo] Access wrapping (mirror, clamp, repeat...)

namespace Etripator
{
	//! \defgroup MEMORY_ACCESS_FLAGS Storage flag bit masks
	//! @{
	//! Data can be read from memory.
	const uint64_t Memory::READ     = 0x00000001U;
	//! Data can be written to memory.
	const uint64_t Memory::WRITE    = 0x00000002U;
	//! @}
	
	//! Constructor
	Memory::Memory()
		: m_wordSize(0)
		, m_wordCount(0)
		, m_access(0)
		, m_mode(Memory::CLAMP)
	{}
	//! Constructor
	//! \param [in] wordSize  Word size in bytes
	//! \param [in] wordCount Word count
	//! \param [in] flag      \see MEMORY_ACCESS_FLAGS
	//! \param [in] mode      Wrapping mode
	Memory::Memory(size_t wordSize, size_t wordCount, uint64_t flag, WrapMode mode)
		: m_wordSize(wordSize)
		, m_wordCount(wordCount)
		, m_access(flag)
		, m_mode(mode)
	{
		m_buffer.resize(m_wordSize * m_wordCount);
	}
	//! Constructor
	Memory::Memory(const Memory& mem)
		: m_wordSize(mem.m_wordSize)
		, m_wordCount(mem.m_wordCount)
		, m_access(mem.m_access)
		, m_mode(mem.m_mode)
		, m_buffer(mem.m_buffer)
	{}
	//! Destructor
	Memory::~Memory()
	{}

	//! Copy operator
	Memory& Memory::operator=(const Memory& mem)
	{
		m_wordSize  = mem.m_wordSize;
		m_wordCount = mem.m_wordCount;
		m_access = mem.m_access;
		m_mode   = mem.m_mode;
		m_buffer = mem.m_buffer;
		return *this;
	}

	//! Read bytes from memory
	//! \param [in]  address  Physical address.
	//! \param [in]  len      Length in bytes.
	//! \param [out] output   Output buffer.
	bool Memory::Read(uint64_t address, size_t len, std::vector<uint8_t>& out) const
	{
		if(!IsReadable())
		{
			return false;
		}

		out.resize(len);

		// [todo]

		return true;
	}

	//! Write bytes to memory
	//! \param [in] address  Physical address.
	//! \param [in] len      Length in bytes.
	//! \param [in] input    Input buffer.
	bool Memory::Write(uint64_t address, size_t len, const std::vector<uint8_t>& input)
	{
		if(!IsWritable())
		{
			return false;
		}

		// [todo]

		return true;
	}

	//! Word size in bytes.
	size_t Memory::WordSize() const
	{
		return m_wordSize;
	}
	//! Word count.
	size_t Memory::WordCount() const
	{
		return m_wordCount;
	}
	//! Memory size in bytes.
	uint64_t Memory::Size() const
	{
		return m_buffer.size();
	}

	//! Test if the memoryis readable.
	bool Memory::IsReadable() const
	{
		return ((m_access & Memory::READ) == Memory::READ);
	}
	//! Test if the memory is writable.
	bool Memory::IsWritable() const
	{
		return ((m_access & Memory::WRITE) == Memory::WRITE);
	}

};