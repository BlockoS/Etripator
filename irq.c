/*
    This file is part of Etripator,
    copyright (c) 2009--2012 Vincent Cruz.

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
#include "irq.h"
#include "message.h"

static char* IRQNames[5] = {
	"irq_2.asm",
	"irq_1.asm",
	"irq_timer.asm",
	"irq_nmi.asm",
	"irq_reset.asm"
};

/**
 * Get irq code offsets from rom.
 * \param [in]  iRomFile Rom file.
 * \param [out] iSectionArray Section array.
 * \return 
 *		0 if an error occured.
 *		1 otherwise.
 */
int getIRQSections(FILE* iRomFile, Section* iSectionArray)
{
	int err, i;
	size_t nRead, size;
	uint8_t offset[2];
	unsigned int headerJump = 0;
	
	/* Header ? */
	fseek(iRomFile, 0, SEEK_END);
	size  = ftell(iRomFile);
	fseek(iRomFile, 0, SEEK_SET);
	size -= ftell(iRomFile);
	
	headerJump = size & 0x1fff;
	
	/* Jump to irq offsets */
	err = fseek(iRomFile, headerJump + 0x1ff6, SEEK_SET);
	if(err < 0)
		return 0;

	for(i=0; i<5; ++i) {
		/* Read offset */
		nRead = fread(offset, 1, 2, iRomFile);
		if(nRead != 2)
			return 0;

		/* Initialize section */
		iSectionArray[i].name  = IRQNames[i];
		iSectionArray[i].type  = CODE;
		iSectionArray[i].bank  = 0;
		iSectionArray[i].org   = (offset[1] << 8) | offset[0];
		iSectionArray[i].type  = CODE;
		iSectionArray[i].start = ((offset[1] & 0x1f) << 8) | offset[0];
		iSectionArray[i].size  = 0;
		iSectionArray[i].id    = i;

		INFO_MSG("%s found at %04x (%lx)\n", iSectionArray[i].name, iSectionArray[i].org, iSectionArray[i].start);
	}
	
	return 1;
}
