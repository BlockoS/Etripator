#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <winmmap.h>
#else
#include <sys/mman.h>
#endif
#include <fcntl.h>
#include <string.h>
#include "csv.h"
#include "section.h"
#include "labels.h"

#define TOKEN_COUNT 5

/*
 * Extract sections from a CSV file
 */ 
int readSectionsFromCSV(char* iFileName, char iSeparator, Section** iSection, size_t* iSectionCount) {
  int fd, i, found, line, ok;
  struct stat sb;
  char *data;
  Section* ptr;
  CSVState state;
  CSVToken token[6];
  size_t reserved;

  /* Set token types */
  token[0].type = STRING;
  token[1].type = STRING;
  token[2].type = STRING;
  token[3].type = STRING;	
  token[4].type = STRING;

  for(i=0; i<TOKEN_COUNT; ++i) {
  	token[i].value.string = NULL;
  }

  ok = 1;

  /* Some sections at the beginning of the array may be reserved */
  reserved = *iSectionCount;

  /* Open file */
  if((fd = open(iFileName, O_RDONLY)) < 0) {
  	fprintf(stderr, "Error: Unable to open %s.\n", iFileName);
  	return 0;
  }

  /* Get infos */
  if(fstat(fd, &sb) < 0) {
  	fprintf(stderr, "Error: Could not stat %s.\n",iFileName);
  	close(fd);
  	return 0;
  }

  if(sb.st_size < TOKEN_COUNT) {
  	fprintf(stderr, "Error: %s is too small.\n",iFileName);
  	close(fd);
  	return 0;
  }

  /* Mmap it */
  data = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if(data == MAP_FAILED) {
  	fprintf(stderr, "Error: Could not mmap %s.\n",iFileName);
  	close(fd);
  	return 0;
  }
	
  /* Initalize csv state */
  CSVSetState(data, data + sb.st_size, &state);
  
  /* Count lines */
  for(; CSVJumpToNextLine(&state); ++*iSectionCount);
  ++*iSectionCount;

  /* Allocate sctions */
  *iSection = (Section*)malloc(*iSectionCount * sizeof(Section));
  
  /* Reset state */
  CSVSetState(data, data + sb.st_size, &state);
  
  line = 0;
  ptr = *iSection + reserved;
  do {
  	/* Extract tokens */
	found = CSVExtractTokensFromLine(&state, iSeparator, token, TOKEN_COUNT);

	/* Not enough tokens found */
	if(found < TOKEN_COUNT) {
		fprintf(stderr, "Error : too few tokens on line %d\n", line);
		ok = 0;
	} 
	else {
		/* Process tokens :			*/
		/*		1. type				*/
		if(strcmp(token[0].value.string, "code") == 0) {
			ptr[line].type = CODE;
		}
		else if(strcmp(token[0].value.string, "inc_data") == 0) {
			ptr[line].type = INC_DATA;
		}
		else if(strcmp(token[0].value.string, "bin_data") == 0) {
			ptr[line].type = BIN_DATA;
		}
		else {
			fprintf(stderr, "Error : Unknow section type : %s. Switching to default (bin data)\n",
					token[0].value.string);
			ptr[line].type = BIN_DATA;
		}
		
		/*		2. name				*/
		ptr[line].name = strdup(token[1].value.string);
		/*		3. rom bank			*/
		ptr[line].bank = (uint8_t)strtol(token[2].value.string, (char**)NULL, 16);
		/*		4. org				*/
		ptr[line].org = (uint16_t)strtol(token[3].value.string, (char**)NULL, 16);
		/*		5. section size		*/
		ptr[line].size   = strtol(token[4].value.string, (char**)NULL, 16);
		
		ptr[line].start  = (ptr[line].bank << 13) | (ptr[line].org & 8191);
		
		/* Data validation */
		if((ptr[line].type != CODE) && (ptr[line].size <= 0))
		{
			fprintf(stderr, "Error in section %s : Automatic section size detection doesn't work for data.\n",
					ptr[line].name);
			ok = 0;
		}
	}

	/* Cleanup strings */
	for(i=0; i<found; ++i) {
		if((token[i].type == STRING) &&
		   (token[i].value.string != NULL) ) {
			free(token[i].value.string);
			token[i].value.string = NULL;
		}
	}
	
	++line;
  }while(CSVJumpToNextLine(&state) && ok);
	
  /* Release map */
  if(munmap(data, sb.st_size) < 0) {
    fprintf(stderr, "Error: Could not mmap %s.\n",iFileName);
  	close(fd);
  	return 0;
  }

  /* Close file */
  close(fd);

  return ok;
}

/*
 * Initialize section processor
 */
void initializeSectionProcessor(SectionProcessor* iProcessor)
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
	
	iProcessor->labelIndex  = 0;
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
}

/*
 * Process data section
 */
int processDataSection(SectionProcessor* iProcessor)
{
	int j;
	char eor;
	off_t fileEnd;
	
	fileEnd = iProcessor->processed->start + iProcessor->processed->size;
	
	if(iProcessor->processed->type == INC_DATA)
	{
		uint8_t data[8];
					
		while(!eor) {
			fprintf(iProcessor->out, "  .db ");
			/* Read data */
			fread(data, 1, 8, iProcessor->in);

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
		size_t  nRead, size;
		int32_t total;
	
		/* Allocate buffer if needed */
		if(iProcessor->buffer == NULL)
		{
			iProcessor->buffer = (uint8_t*)malloc(256);
			if(iProcessor->buffer == NULL)
			{
				fprintf(stderr, "Not enough memory\n");
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

/* Maximum number of characters per line */
#define MAX_CHAR_PER_LINE 80

/*
 * Process opcode
 */
char processOpcode(struct LabelRepository_* iLabelRepository, SectionProcessor* iProcessor) {
	int i, delta;
	uint8_t inst, data[6], isJump, page;
	char line[MAX_CHAR_PER_LINE], eor, *ptr;
	uint16_t offset, nextOrgOffset;

	eor = 0;
	ptr = line;

	/* Opcode */
	fread(&inst, 1, 1, iProcessor->in);

	/* Get label index */
	getLabelIndex(iLabelRepository + iProcessor->page, iProcessor);

	nextOrgOffset = iProcessor->orgOffset + pce_opcode[inst].size;
	
	/* Is there a label ? */
	if((iProcessor->labelIndex < iLabelRepository[iProcessor->page].last) &&
	   ((iLabelRepository[iProcessor->page].labels[iProcessor->labelIndex].offset >= iProcessor->orgOffset) &&
	    (iLabelRepository[iProcessor->page].labels[iProcessor->labelIndex].offset < nextOrgOffset)))
	{
		/* Print label*/
		sprintf(line, "l_%04x: ", iLabelRepository[iProcessor->page].labels[iProcessor->labelIndex].offset);

		/* Add displacement */
		if(iLabelRepository[iProcessor->page].labels[iProcessor->labelIndex].offset != iProcessor->orgOffset)
		{
			iLabelRepository[iProcessor->page].labels[iProcessor->labelIndex].displacement = iLabelRepository[iProcessor->page].labels[iProcessor->labelIndex].offset - iProcessor->orgOffset;
		}
	}
	else
	{
		memset(line, ' ', 8 * sizeof(char));
	}

	ptr += 8;

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
		fread(data, 1, pce_opcode[inst].size-1, iProcessor->in);
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
		for(i=0; i < iLabelRepository[iProcessor->page].last; ++i)
		{
			if(iLabelRepository[iProcessor->page].labels[i].offset == offset)
			{
				/* Displacement may not exceed 7 (maximum opcode data size) */
				if(iLabelRepository[iProcessor->page].labels[i].displacement != 0)
				{
					ptr[0] = '+';
					ptr[1] = '0' + iLabelRepository[iProcessor->page].labels[i].displacement;
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

	return eor;
}

