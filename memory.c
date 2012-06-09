#include "memory.h"

/**
 * Compute 8kB offset from logical address.
 * \param [in] addr Logical 16 bits address.
 * \return Offset in the 8kb.
 */
uint16_t GetOffset(uint16_t addr)	
{
	return addr & 0x1fff;
}

/**
 * Compute MPR id from logical address.
 * \param [in] addr Logical 16 bits address.
 * \return MPR id.
 */
uint8_t GetMPRId(uint16_t addr)
{
	return (addr >> 13) & 0x07;
}

/**
 * Retrieve bank used by current logical address.
 * \param [in] mem Current memory layout.
 * \param [in] addr Logical 16 bits address.
 * \return Bank number.
 */
uint8_t GetBank(Memory* mem, uint16_t addr)
{
	return 	mem->mpr[GetMPRId(addr)];
}

/**
 * Compute physical address.
 * \param [in] mem Current memory layout.
 * \param [in] addr Logical 16 bits address.
 * \return 21 bits long physical address.
 */
uint32_t GetPhysicalAddress(Memory* mem, uint16_t addr)
{
	return (GetBank(mem, addr) << 13) | GetOffset(addr);
}
