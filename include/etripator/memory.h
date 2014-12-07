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
#ifndef ETRIPATOR_MEMORY_H
#define ETRIPATOR_MEMORY_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace etripator {
    /// Generic memory structure. Can be either RAM, ROM, floppy disk...
    class Memory   
    {
    public:
	/// Memory access flag bit masks
	enum AccessMode
	{
	    /// Memory is read-only.
	    ReadOnly,
	    /// Memory is write-only.
	    WriteOnly,
	    /// Memory is read/write.
	    ReadWrite
	};
	/// Memory wrapping mode
	/// If the current pointer is out of the memory boundary, it will
	/// either be clamped (pointer doens't move but returns 0xff),
	/// repeated (goes back to the beginning).
	enum WrapMode
	{
	    Clamp,
	    Repeat
	};
    public:
	/// Default constructor.
	/// Word size, count are zero. Access mode is set to Read Only. Wrap mode 
	/// is set to Clamp.
	Memory();
	/// Constructor.
	/// \param [in] wordSize  Word size in bytes.
	/// \param [in] wordCount Word count.
	/// \param [in] access    Access mode.
	/// \param [in] wrap      Wrapping mode.
	Memory(size_t wordSize, size_t wordCount, AccessMode access, WrapMode wrap);
	/// Copy constructor.
	/// \param [in] mem    Input memory.
	Memory(Memory const& mem);
	/// Destructor.
	~Memory();
	/// Copy operator.
	/// \param [in] mem    Input memory.
	Memory& operator=(Memory const& mem);
	/// Read bytes from memory.
	/// \param [in]  address  Physical address.
	/// \param [in]  len      Length in bytes.
	/// \param [out] output   Output buffer.
	/// \return false if the memory is write only.
	bool read(uint64_t address, size_t len, std::vector<uint8_t>& out) const;
	/// Write bytes to memory.
	/// \param [in] address  Physical address.
	/// \param [in] len      Length in bytes.
	/// \param [in] input    Input buffer.
	bool write(uint64_t address, size_t len, std::vector<uint8_t> const& input);
	/// Word size in bytes.
	size_t wordSize() const;
	/// Word count.
	size_t wordCount() const;
	/// Memory size in bytes.
	uint64_t size() const;
	/// Memory access mode.
	AccessMode access() const;
	/// Memory wrap mode.
	WrapMode wrap() const;
    protected:
	/// Word size.
	size_t m_wordSize;
	/// Word count.
	size_t m_wordCount;
	/// Access mode.
	AccessMode m_access;
	/// Wrap mode.
	WrapMode m_wrap;
	/// Buffer.
	std::vector<uint8_t> m_buffer;
    };
}; // etripator

#endif // ETRIPATOR_MEMORY_H
