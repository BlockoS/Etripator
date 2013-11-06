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
#include "config.h"
#include "message.h"
#include "arch/pce.h"
#include "decode.h"

/*
 * Initialize section processor
 */
int initializeSectionProcessor(SectionProcessor* iProcessor)
{
	iProcessor->processed = NULL;
	iProcessor->in        = NULL;
	iProcessor->out       = NULL;
	
	iProcessor->fileOffset = 0;
	iProcessor->orgOffset  = 0;
	iProcessor->page       = 0;

	iProcessor->instruction = 0,
	memset(iProcessor->data, 0, 6);
	
	iProcessor->buffer      = NULL;
	iProcessor->labelIndex  = 0;
	return initializeLabelRepository(&iProcessor->labelRepository);
}

/*
 * Reset section processor
 */
void resetSectionProcessor(FILE* iIn, FILE* iOut, Section* iProcessed, SectionProcessor* iProcessor)
{
	iProcessor->processed = iProcessed;
	iProcessor->in        = iIn;
	iProcessor->out       = iOut;

	iProcessor->fileOffset = iProcessed->start;
	iProcessor->orgOffset  = iProcessed->org;
	iProcessor->page       = (iProcessed->org >> 13) & 0x07;
	fseek(iIn, iProcessed->start, SEEK_SET);

	iProcessor->instruction = 0,
	memset(iProcessor->data, 0, 6);

	iProcessor->labelIndex = 0;
	resetLabelRepository(&iProcessor->labelRepository);
}
  
/*
 * Delete section processor
 */
void deleteSectionProcessor(SectionProcessor* iProcessor)
{
	if(iProcessor->buffer != NULL)
	{
		free(iProcessor->buffer);
		iProcessor->buffer = NULL;
	}

	deleteLabelRepository(&iProcessor->labelRepository);
}

/*
 * Process data section
 */
int processDataSection(SectionProcessor* iProcessor)
{
	int j;
	char eor = 0;
	off_t fileEnd;
	size_t nRead;
	
	fileEnd = iProcessor->processed->start + iProcessor->processed->size;
	
	if(iProcessor->processed->type == INC_DATA)
	{
		uint8_t data[8];
					
		while(!eor) {
			fprintf(iProcessor->out, "  .db ");
			/* Read data */
			nRead = fread(data, 1, 8, iProcessor->in);
			if(nRead != 8)
			{
				return 0;
			}
			
			j=0;
			while(1)
			{
				fprintf(iProcessor->out, "$%02x", data[j]);

				++j;
				++iProcessor->fileOffset;
				if((j<8) && (iProcessor->fileOffset < fileEnd))
					fprintf(iProcessor->out, ", ");
				else
				{
					fprintf(iProcessor->out, "\n");
					break;
				}
			}

			if(iProcessor->fileOffset >= fileEnd)
				eor  = 1;
		}
	}
	else
	{
		size_t  nRead, size, total;
	
		/* Allocate buffer if needed */
		if(iProcessor->buffer == NULL)
		{
			iProcessor->buffer = (uint8_t*)malloc(256);
			if(iProcessor->buffer == NULL)
			{
				ERROR_MSG("Allocation failed : %s", strerror(errno));
				return 0;
			}
		}

		/* Copy data to binary file */
		for(total=0; total < iProcessor->processed->size; total += nRead)
		{
			size = iProcessor->processed->size - total;
			if(size > 256)
				size = 256;
			if(size == 0)
				break;
		
			nRead = fread(iProcessor->buffer, 1, size, iProcessor->in);
			if(nRead == 0)
				break;
			fwrite(iProcessor->buffer, 1, nRead, iProcessor->out);
		}
	}
	
	return 1;
}

/* Parse section to identify potential labels */
int getLabels(SectionProcessor* iProcessor)
{
	size_t nBytesProcessed;
	unsigned char inst, data[6], eor;
	uint16_t labelOffset, offset;
	int delta,i=0;
	size_t nRead;
	
	LabelRepository *repository = &(iProcessor->labelRepository);
	Section *section = iProcessor->processed;
	FILE    *in      = iProcessor->in;

	if(section->type != CODE)
		return 1;
		
	printf("\n%s:\n", section->name);

	nBytesProcessed = 0;
	offset          = section->org;
	
	/* Jump to section start */
	fseek(in, section->start, SEEK_SET);
		
	/* Push the section org */
	if(pushLabel(repository, offset) == 0)
		return 0;

	/* Walk along section */
	eor    = 0;
	while(!eor)
	{
		/* Read instruction */
		nRead = fread(&inst, 1, 1, iProcessor->in);
		if(nRead != 1)
		{
			return 0;
		}
		
		/* Read data */
		if(pce_opcode[inst].size > 1)
		{
			nRead = fread(data, 1, pce_opcode[inst].size-1, in);
			if(nRead != (size_t)(pce_opcode[inst].size-1))
			{
				return 0;
			}
		}
		
		nBytesProcessed += pce_opcode[inst].size;
		offset          += pce_opcode[inst].size;

		if(isLocalJump(inst))
		{

			/* For BBR* and BBS* displacement is stored in the 2nd byte */
			i = (((inst) & 0x0F) == 0x0F) ? 1 : 0;

			/* Detect negative number */
			if(data[i] & 128)
				delta = - ((data[i] - 1) ^ 0xff);
			else
				delta = data[i];

			labelOffset = offset + delta;
			/* Insert offset to repository */
			if(pushLabel(repository, labelOffset) == 0)
				return 0;

			printf("%04x short jump to %04x\n", offset, labelOffset);
		}
		else 
		{
			if(isFarJump(inst))
			{
				labelOffset = data[0] | (data[1] << 8);
				/* Insert offset to repository */
				if(pushLabel(repository, labelOffset) == 0)
					return 0;

				printf("%04x long jump to %04x\n", offset, labelOffset);
			}
		}
		
		/* Search end of section */
		if(section->size == 0)
		{
			if((inst == 0x40) || (inst == 0x60))
			{
				section->size = nBytesProcessed;
				eor            = 1;
			}
		}
		else if( nBytesProcessed >= section->size )
		{
			eor = 1;
		}		
	}

	finalizeLabelRepositoty(repository);

	return 1;
}

/* Initialize label index so that it points to the label close to current org offset */
void getLabelIndex(SectionProcessor* iProcessor)
{
	int i;
	
	/* Room for huge improvments */
	for(i=iProcessor->labelIndex; i<iProcessor->labelRepository.last; ++i)
	{
		if(iProcessor->labelRepository.labels[i].offset >= iProcessor->orgOffset)
		{
			break;
		}
	}
	
	iProcessor->labelIndex = i;
}

/* Maximum number of characters per line */
#define MAX_CHAR_PER_LINE 80

/*
 * Process opcode
 */
char processOpcode(SectionProcessor* iProcessor) {
	int i, delta;
	uint8_t inst, data[6], isJump, page;
	char line[MAX_CHAR_PER_LINE], eor, *ptr;
	uint16_t offset, nextOrgOffset;
	size_t nRead;
	
	eor = 0;
	ptr = line;

	/* Opcode */
	nRead = fread(&inst, 1, 1, iProcessor->in);
	if(nRead != 1)
	{
		return 0;
	}
	
	/* Get label index */
	getLabelIndex(iProcessor);

	nextOrgOffset = iProcessor->orgOffset + pce_opcode[inst].size;
	
	/* Is there a label ? */
	if( (iProcessor->labelIndex < iProcessor->labelRepository.last) &&
	    (iProcessor->labelRepository.labels[iProcessor->labelIndex].offset >= iProcessor->orgOffset) &&
	    (iProcessor->labelRepository.labels[iProcessor->labelIndex].offset < nextOrgOffset) )
	{
		/* Print label*/
		sprintf(line, "l%04xx: ", iProcessor->labelRepository.labels[iProcessor->labelIndex].offset);

		/* Add displacement */
		if(iProcessor->labelRepository.labels[iProcessor->labelIndex].offset != iProcessor->orgOffset)
		{
			iProcessor->labelRepository.labels[iProcessor->labelIndex].displacement = iProcessor->labelRepository.labels[iProcessor->labelIndex].offset - iProcessor->orgOffset;
		}
	}
	else
	{
		memset(line, ' ', 10 * sizeof(char));
	}

	ptr += 10;

	/* Print opcode sting */
	memcpy(ptr, pce_opcode[inst].name, 4 * sizeof(char));
	ptr += 4;

	/* Add spacing */  
	memset(ptr, ' ', 4 * sizeof(char));
	ptr += 4;

	/* End Of Routine (eor) is set to 1 if the instruction is RTI or RTS */
	if((inst == 64) || (inst == 96)) eor = 1;

	/* Data */
	if(pce_opcode[inst].size > 1)
	{
		nRead = fread(data, 1, pce_opcode[inst].size-1, iProcessor->in);
		if(nRead != (size_t)(pce_opcode[inst].size-1))
		{
			return 0;
		}
	}

	iProcessor->orgOffset   = nextOrgOffset;
	iProcessor->fileOffset += pce_opcode[inst].size;
    
	/* Swap LSB and MSB for words */
	if(pce_opcode[inst].size > 2)
	{
		uint8_t swap;
		/* Special case : TST, BBR*, BBS*. The first byte is zp var */
		i = ((pce_opcode[inst].type ==  3) ||
		     (pce_opcode[inst].type ==  4) ||
		     (pce_opcode[inst].type ==  5) ||
		     (pce_opcode[inst].type ==  6) ||
		     (pce_opcode[inst].type == 13) ||
		     (pce_opcode[inst].type == 20)) ? 1 : 0;

		for(; i<(pce_opcode[inst].size-2); i+=2)
		{
			swap      = data[i];
			data[i]   = data[i+1];
			data[i+1] = swap;
		}
	}

	/* Handle special cases (jumps, tam/tma and unsupported opcodes ) */
	isJump = 0;
	if(isLocalJump(inst)) {
		isJump = 1;

		i = (((inst) & 0x0F) == 0x0F) ? 1 : 0;

		/* Detect negative number */
		if(data[i] & 128)
			delta = - ((data[i] - 1) ^ 0xff);
		else
			delta = data[i];

		offset = iProcessor->orgOffset + delta;
		data[i  ] = offset >> 8;
		data[i+1] = offset & 0xff;
		
		page = iProcessor->page;
	}
	else 
	{
		if(isFarJump(inst))
		{
			isJump = 1;
			/* Extract page */
			page  = (data[1] >> 5) & 0xff;
			/* Build offset */
			offset = (data[1] << 8) | data[0];
			
		}
		else 
		{
			page   = 0;
			offset = 0;
			
			/* Unknown instructions are output as raw data 
			 * (ie) .db XX
			 */
			if(pce_opcode[inst].type == 22)
			{
				data[0] = inst;
			}
		}
	}

	if(pce_opcode[inst].type == 1) {
		*(ptr++) = 'A';
	}
	else {
		if((inst == 0x43) || (inst == 0x53))
		{
			/* tam and tma */
			/* Compute log base 2 of data */
			for(i=0; (i<8) && ((data[0] & 1) == 0); ++i, data[0] >>= 1);
			data[0] = i;		
		}

		/* Print data */
		if(pce_opcode[inst].type)
		{
			for(i=0; pce_opstring[pce_opcode[inst].type][i] != NULL; ++i)
			{
				sprintf(ptr, pce_opstring[pce_opcode[inst].type][i], data[i]);
				ptr += strlen(ptr);
			}
		}
	}

	/* Add displacement to jump offset ? */
	if(isJump)
	{
		/* Search in label database ( todo : dicotomic search ) */
		for(i=0; i < iProcessor->labelRepository.last; ++i)
		{
			if(iProcessor->labelRepository.labels[i].offset == offset)
			{
				/* Displacement may not exceed 7 (maximum opcode data size) */
				if(iProcessor->labelRepository.labels[i].displacement != 0)
				{
					ptr[0] = '+';
					ptr[1] = '0' + iProcessor->labelRepository.labels[i].displacement;
					ptr += 2;
				}
				break;
			}
		}
	}

	*(ptr++) = '\n';
	*ptr = '\0';

	/* Output line */
	fwrite(line, sizeof(char), ptr-line, iProcessor->out);

    /* Avoid annoying warning. */
    (void)page;
    
	return eor;
}
