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
#include <etripator/memory.h>
#include <algorithm>

namespace etripator {
    /// Default constructor.
    /// Word size, count are zero. Access mode is set to Read Only. Wrap mode 
    /// is set to Clamp.
    Memory::Memory()
	: m_wordSize(0)
	, m_wordCount(0)
	, m_access(Memory::ReadOnly)
	, m_wrap(Memory::Clamp)
    {}
    /// Constructor.
    /// \param [in] wordSize  Word size in bytes.
    /// \param [in] wordCount Word count.
    /// \param [in] access    Access mode.
    /// \param [in] wrap      Wrapping mode.
    Memory::Memory(size_t wordSize, size_t wordCount, Memory::AccessMode access, Memory::WrapMode wrap)
	: m_wordSize(wordSize)
	, m_wordCount(wordCount)
	, m_access(access)
	, m_wrap(wrap)
    {
	m_buffer.resize(m_wordSize * m_wordCount);
    }
    /// Copy Constructor.
    /// \param [in] mem   Input memory.
    Memory::Memory(Memory const& mem)
	: m_wordSize(mem.m_wordSize)
	, m_wordCount(mem.m_wordCount)
	, m_access(mem.m_access)
	, m_wrap(mem.m_wrap)
	, m_buffer(mem.m_buffer)
    {}
    /// Destructor.
    Memory::~Memory()
    {}
    /// Copy operator.
    Memory& Memory::operator=(const Memory& mem)
    {
	m_wordSize  = mem.m_wordSize;
	m_wordCount = mem.m_wordCount;
	m_access = mem.m_access;
	m_wrap   = mem.m_wrap;
	m_buffer = mem.m_buffer;
	return *this;
    }
    /// Read bytes from memory.
    /// \param [in]  address  Physical address.
    /// \param [in]  len      Length in bytes.
    /// \param [out] output   Output data pointer.
    /// \return Result.
    Memory::Result Memory::read(uint64_t address, size_t len, void* out) const
    {
	Memory::Result res;

	res.overflow = 0;
	res.bytes    = 0;
	res.access   = (Memory::WriteOnly != m_access);

	if(false == res.access)
	{
	    return res;
	}
	res.bytes = len;
	uint8_t* ptr = reinterpret_cast<uint8_t*>(out);
	if((address + len) > m_buffer.size())
	{
	    size_t count;
	    if(address  >= m_buffer.size())
	    {
		count = 0;
	    }
	    else
	    {
		count = m_buffer.size() - address;
		std::copy_n(m_buffer.begin()+address, count, ptr);
	    }
	    res.overflow = len - count;	    
	    if(Memory::Clamp == m_wrap)
	    {
		std::fill(ptr+count, ptr+len, 0xff);
	    }
	    else if(Memory::Repeat == m_wrap)
	    {
		size_t src = (address + count) % m_buffer.size();
		size_t dst;
		for(dst=count; dst<len; dst+=count)
		{
		    count = (len - dst);
		    count = ((src+count) > m_buffer.size()) ? (m_buffer.size()-src) : count;
		    std::copy_n(m_buffer.begin()+src, count, ptr+dst);
		    src = (src + count) % m_buffer.size(); 
		}
	    }
	}
	else
	{
	    res.overflow = 0;
	    std::copy_n(m_buffer.begin() + address, len, ptr);
	}
	return res;
    }
    /// Write bytes to memory.
    /// \param [in] address  Physical address.
    /// \param [in] len      Length in bytes.
    /// \param [in] input    Input data pointer.
    /// \return Result.
    Memory::Result Memory::write(uint64_t address, size_t len, const void* input)
    {
	Memory::Result res;
	res.overflow = 0;
	res.bytes    = 0;
	res.access   = (Memory::ReadOnly != m_access);
	
	if(false == res.access)
	{
	    return res;
	}

	res.bytes = len;

	const uint8_t* ptr = reinterpret_cast<const uint8_t*>(input);
	if((address+len) > m_buffer.size())
	{
	    if(Memory::Repeat == m_wrap)
	    {
		size_t count = (address + len) % m_buffer.size();
		uint64_t offset = len - count;
		std::copy_n(ptr+offset, count, m_buffer.begin());

		if(offset > m_buffer.size())
		{
		    std::copy_n(ptr, m_buffer.size()-count, m_buffer.begin()+count);
		}
		else
		{
		    offset = m_buffer.size() - offset;
		    std::copy_n(ptr, len-count, m_buffer.begin()+offset);
		}
		if(address  > m_buffer.size())
		{
		    res.overflow = len;
		}
		else
		{
		    res.overflow = len - (m_buffer.size() - address);
		}
	    }
	    else
	    {
		size_t count;
		if(address >= m_buffer.size())
		{
		    count = 0;
		}
		else
		{
		    count = m_buffer.size() - address;
		    std::copy_n(ptr, count, m_buffer.begin() + address);
		}
		res.overflow = len - count;
	    }
	}
	else
	{
	    std::copy_n(ptr, len, m_buffer.begin());
	}
	return res;
    }
    /// Flash memory.
    /// Access rights are ignored.
    /// \param [in] in  Pointer to source data.
    /// \param [in] len Number of bytes to be written.
    /// \return Number of bytes written.
    size_t Memory::flash(size_t len, const void* in)
    {
	size_t count = (len > m_buffer.size()) ? m_buffer.size() : len;
	std::copy_n(reinterpret_cast<const uint8_t*>(in), count, m_buffer.begin());
	return count;
    }
    /// Dump whole memory.
    /// Access rights are ignores.
    /// \param [out] out Output byte vector.
    void Memory::dump(std::vector<uint8_t>& out)
    {
	out.resize(m_buffer.size());
	std::copy(m_buffer.begin(), m_buffer.end(), out.begin());
    }
    /// Word size in bytes.
    size_t Memory::wordSize() const
    {
	return m_wordSize;
    }
    /// Word count.
    size_t Memory::wordCount() const
    {
	return m_wordCount;
    }
    /// Memory size in bytes.
    uint64_t Memory::size() const
    {
	return m_buffer.size();
    }
    /// Memory access mode.
    Memory::AccessMode Memory::access() const
    {
	return m_access;
    }
    /// Memory wrap mode.
    Memory::WrapMode Memory::wrap() const
    {
	return m_wrap;
    }
}; // etripator
