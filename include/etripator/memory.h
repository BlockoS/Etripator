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
	    /// Any byte read or written outside of the memory boundaries
	    /// will be equal to 0xff.
	    Clamp,
	    /// The offset will loop through the memory boundaries.
	    Repeat
	};
	/// Memory operation result.
	struct Result
	{
	    /// Number of bytes read or written outside of the memory area.
	    size_t overflow;
	    /// Access validity.
	    /// @b true if the access right was valid regarding the requested
	    /// operation. For example, this field will be false for @b write 
	    /// operations on a @b ReadOnly memory.
	    bool   access;
	    /// Total number of bytes read or written.
	    size_t bytes;
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
	/// \param [out] output   Output data pointer.
	/// \return Result.
	Result read(uint64_t address, size_t len, void* output) const;
	/// Write bytes to memory.
	/// \param [in] address  Physical address.
	/// \param [in] len      Length in bytes.
	/// \param [in] input    Input data pointer.
	/// \return Result.
        Result write(uint64_t address, size_t len, const void* input);
	/// Flash memory.
	/// Access rights are ignored.
	/// \param [in] in  Pointer to source data.
	/// \param [in] len Number of bytes to be written.
	/// \return Number of bytes written.
	size_t flash(size_t len, const void* in);
	/// Dump whole memory.
	/// Access rights are ignores.
	/// \param [out] out Output byte vector.
	void dump(std::vector<uint8_t>& out);
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
