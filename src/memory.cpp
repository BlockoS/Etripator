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
    /// \param [out] output   Output buffer.
    /// \return false if the memory is write only.
    bool Memory::read(uint64_t address, size_t len, std::vector<uint8_t>& out) const
    {
	out.resize(len);
	uint64_t src;
	uint64_t dst;
	src = address;
	for(dst=0; dst<len; )
	{
	    if(src < m_buffer.size())
	    {
		size_t srcLen = m_buffer.size() - src;
		size_t dstLen = len - dst;
		size_t count  = (srcLen < dstLen) ? srcLen : dstLen;
		std::copy_n(m_buffer.begin()+src, count, out.begin()+dst);
		src += count;
		dst += count;
	    }
	    
	    if(dst < len)
	    {
	       	if(Memory::Clamp == m_wrap)
		{
		    std::fill(out.begin()+dst, out.end(), 0xff);
		    dst = len;
	        }
	        else if(Memory::Repeat == m_wrap)
	        {
		    src = 0;
	        }
	    }
	}
	return (Memory::WriteOnly != m_access);
    }
    /// Write bytes to memory.
    /// \param [in] address  Physical address.
    /// \param [in] len      Length in bytes.
    /// \param [in] input    Input buffer.
    bool Memory::write(uint64_t address, size_t len, std::vector<uint8_t> const& input)
    {
	uint64_t src;
	uint64_t dst;
	for(src=0, dst=address; src<len; )
	{
	    if(dst < m_buffer.size())
	    {
		size_t srcLen = input.size() - src;
		size_t dstLen = m_buffer.size() - dst;
		size_t count  = (srcLen < dstLen) ? srcLen : dstLen;
		std::copy_n(input.begin()+src, count, m_buffer.begin()+dst);
		src += count;
		dst += count;
	    }
	    if(dst >= m_buffer.size())
	    {
		if(Memory::Clamp == m_wrap)
		{
		    src = len;
		}
		else if(Memory::Repeat == m_wrap)
		{
		    dst = 0;
		}
	    }
	}
	return (Memory::ReadOnly != m_access);
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
