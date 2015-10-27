/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

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
#include "cfg.h"

#include "memorymap.h"
#include "section.h"
#include "decode.h"
#include "irq.h"
#include "rom.h"
#include "cd.h"
#include "options.h"
#include "labelsloader.h"

/* [todo] */
#include "arch/pce/labels.h"
#include "arch/pce/disassembly.h"

/*
  exit callback
 */
void exit_callback(void)
{
    PrintMsgClose();
}

/* Main */
int main(int argc, char** argv)
{
    FILE* out;
    FILE* mainFile;

    unsigned int i;
    int ret, failure;
    SECTION_ERR sectErr;

    CommandLineOptions cmdOptions;

    Section* section;
    size_t sectionCount;

    Decoder decoder;

    MemoryMap memmap;

    /* [todo] */
    Pass pass[2];

    atexit(exit_callback);

    PrintMsgOpenFile(NULL);

    /* Extract command line options */
    ret = getCommandLineOptions(argc, argv, &cmdOptions);
    if(ret <= 0)
    {
        usage();
        return (ret < 0);
    }

    failure = 1;

    /* Read cfg file */
    if(cmdOptions.extractIRQ)
    {
        sectionCount = 5;
        section = (Section*)malloc(sectionCount * sizeof(Section));
        if(NULL == section)
        {
            ERROR_MSG("Failed to allocate memory: %s", strerror(errno));
            goto error_1;
        }
    }
    else
    {
        sectionCount = 0;
        section      = NULL;
    }

    if(cmdOptions.cfgFileName)
    {
        sectErr = readSectionsFromCFG(cmdOptions.cfgFileName, &section, &sectionCount);
        if(SECTION_OK != sectErr)
        {
            ERROR_MSG("Unable to read %s", cmdOptions.cfgFileName);
            goto error_1;
        }
    }

    /* Initialize memory map. */
    ret = initializeMemoryMap(&memmap);
    if(0 == ret)
    {
        goto error_1;
    }

    /* Read ROM */
    if(0 == cmdOptions.cdrom)
    {
        ret = loadROM(cmdOptions.romFileName, &memmap);
        if(0 == ret)
        {
            goto error_2;
        }

        /* Get irq offsets */
        if(cmdOptions.extractIRQ)
        {
            ret = getIRQSections(&memmap, section);
            if(0 == ret)
            {
                ERROR_MSG("An error occured while reading irq vector offsets");
                goto error_2;
            }
        }
    }
    else
    {
        ret = addCDRAMMemoryMap(&memmap);
        if(0 == ret)
        {
            goto error_2;
        }
        /*  Data will be loaded during section disassembly */
    }

    /* Initialize decoder */
    initializeDecoder(&decoder);

    /* Load labels */
    if(NULL != cmdOptions.labelsFileName)
    {
        ret = loadLabels(cmdOptions.labelsFileName, decoder.labelRepository);
        if(0 == ret)
        {
            ERROR_MSG("An error occured while loading labels from %s : %s", cmdOptions.labelsFileName, strerror(errno));
            goto error_4;
        }
    }

    /* For each section reset every existing files */
    for(i=0; i<sectionCount; ++i)
    {
        out = fopen(section[i].filename, "wb");
        if(NULL == out)
        {
            ERROR_MSG("Can't open %s : %s", section[i].filename, strerror(errno));
            goto error_4;
        }
        fclose(out);
    }

    /* Add section name to label repository. */
    for(i=0; i<sectionCount; ++i)
    {
        ret = addLabel(decoder.labelRepository, section[i].name, section[i].org, (section[i].bank << 13) | (section[i].org & 0x1fff));
        if(0 == ret)
        {
            ERROR_MSG("Failed to add section name (%s) to labels", section[i].name);
            goto error_4;
        }
    }

    decoder.automatic = cmdOptions.extractIRQ;
    /* [todo] test */
    initializeLabelExtractionPass(&decoder, &pass[0]);
    initializeDisassemblyPass    (&decoder, &pass[1]);

    /* Disassemble and output */
    for(i=0; i<sectionCount; ++i)
    {
        out = fopen(section[i].filename, "ab");
        if(NULL == out)
        {
            ERROR_MSG("Can't open %s : %s", section[i].filename, strerror(errno));
            goto error_4;
        }

        if(0 != cmdOptions.cdrom)
        {
            /* Copy CDROM data */
            ret = loadCD(cmdOptions.romFileName, section[i].start, section[i].size, section[i].bank, section[i].org, &memmap);
            if(0 == ret)
            {
                ERROR_MSG("Failed to load CD data (section %d)", i);
                goto error_4;
            }
        }

        if(section[i].type != BIN_DATA)
        {
            /* Print header */
            fprintf(out, "\t.%s\n"
                         "\t.bank %x\n"
                         "\t.org $%04x\n",
                         (section[i].type == CODE) ? "code" : "data", section[i].bank,
                          section[i].org);
        }

        /* Reset decoder */
        resetDecoder(&memmap, out, &section[i], &decoder);
        
        if(CODE == section[i].type)
        {
            /* Extract labels */
            ret = perform(&pass[0]);
            if(0 == ret)
            {
                goto error_4;
            }

            /* Process opcodes */
            ret = perform(&pass[1]);
            if(0 == ret)
            {
                goto error_4;
            }
        }
        else
        {
            ret = processDataSection(&decoder);
            if(0 == ret)
            {
                // [todo]   
            }
        }
        fputc('\n', decoder.out);

        fclose(out);
        out = NULL;
    }

    /* Open main asm file */
    mainFile = fopen(cmdOptions.mainFileName, "w");
    if(NULL == mainFile)
    {
        ERROR_MSG("Unable to open %s : %s", cmdOptions.mainFileName, strerror(errno));
        goto error_4;
    }
  
    if(cmdOptions.extractIRQ)
    {
        fprintf(mainFile, "\n\t.data\n\t.bank 0\n\t.org $FFF6\n");
        for(i=0; i<5; ++i)
        {
            fprintf(mainFile, "\t.dw $%04x\n", section[i].org);
        }
    }

    fclose(mainFile);
    failure = 0;

error_4:
    deleteDecoder(&decoder);
error_2:
    destroyMemoryMap(&memmap);
error_1:
    for(i=0; i<sectionCount; ++i)
    {
        free(section[i].name);
        free(section[i].filename);
        section[i].name = NULL;
    }
    free(section);

    return failure;
}
