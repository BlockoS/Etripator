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
#include "ram.h"

namespace Etripator
{
	//! Constructor
	RAM::RAM(size_t wordSize, size_t wordCount)
		: Storage(wordSize*wordCount, Storage::READ | Storage::WRITE | Storage::VOLATILE)
		, m_wordSize(wordSize)
		, m_wordCount(wordCount)
	{
		m_buffer.reserve(static_cast<unsigned int>(m_size));
	}
	//! Destructor
	RAM::~RAM()
	{}
						
	//! Read bytes from storage
	//! \param [in]  address  Physical address.
	//! \param [in]  len      Length in bytes.
	//! \param [out] output   Output buffer.
	bool RAM::Read(uint64_t address, size_t len, std::vector<uint8_t>& out) const
	{
		if(address >= m_size)
		{
			out.clear();
			return false;
		}

		bool ret = true;
		uint64_t last = address + (len-1);
		if(last >= m_size)
		{
			len  = static_cast<size_t>(m_size - address);
			last = len  - 1;
			ret = false;
		}

		out.resize(len);
		std::copy(m_buffer.begin()+static_cast<off_t>(address), m_buffer.begin()+len, out.begin());

		return ret;
	}

	//! Write bytes from storage
	//! \param [in] address  Physical address.
	//! \param [in] len      Length in bytes.
	//! \param [in] input    Input buffer.
	bool RAM::Write(uint64_t address, size_t len, const std::vector<uint8_t>& input)
	{
		if(address >= m_size)
		{
			return false;
		}
		bool ret = true;
		uint64_t last = address + (len-1);
		if(last >= m_size)
		{
			len  = static_cast<size_t>(m_size - address);
			last = len  - 1;
			ret = false;
		}
		std::copy(input.begin(), input.begin()+len, m_buffer.begin());
		return ret;
	}

	//! Word size in bytes.
	size_t RAM::WordSize() const
	{
		return m_wordSize;
	}
	//! Word count.
	size_t RAM::WordCount() const
	{
		return m_wordCount;
	}
};
