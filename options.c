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
#include "options.h"

/** 
 * Print usage.
 */
void usage()
{
	fprintf(stderr, "Usage : etripator [options] <cfg> <in>\n"
			"--irq-detect or -i : automatically detect and extract irq vectors.\n"
			"--cd or -c         : cdrom image disassembly. Irq detection and rom\n"
            "                     header jump are not performed.\n"
			"--help or -h       : displays this message.\n"
			"--out or -o <file> : main asm file containing includes for all sections \n"
			"--log or -l <file> : log output file (optional)\n"
			"                     as long the irq vector table if the irq-detect\n"
			"                     option is enabled.\n");
}

/**
 * Parse command line options.
 * \param [in] argc Command line argument count.
 * \param [in] argv Command line argument string array.
 * \param [out] iOptions Command line options.
 * \return 
 *		<= 0 if an error occured.
 *		> 0 on success.
 */
int getCommandLineOptions(int argc, char** argv, CommandLineOptions* iOptions)
{
	char *shortOptions = "ichl:o:";
	struct option longOptions[] = {
		{"irq-detect", 0, 0, 'i'},
		{"cd",	       0, 0, 'c'},
		{"help",       0, 0, 'h'},
		{"log",        1, 0, 'l'},
		{"out",        1, 0, 'o'},
		{ 0,           0, 0,  0 }
	};
	int idx, opt;

    /* Reset options */
	iOptions->extractIRQ   = 0;
	iOptions->cdrom        = 0;
	iOptions->cfgFileName  = NULL;
	iOptions->romFileName  = NULL;
	iOptions->mainFileName = "main.asm";
	iOptions->logFileName  = NULL;

	/* Note : IRQ detection is disabled with the cdrom option */
	while ((opt = getopt_long (argc, argv, shortOptions, longOptions, &idx)) > 0)
	{
		switch(opt)
		{
			case 'i':
				if(!iOptions->cdrom)
					iOptions->extractIRQ = 1;
				break;

			case 'c':
				iOptions->cdrom      = 1;
				iOptions->extractIRQ = 0;
				break;

			case 'o':
				iOptions->mainFileName = optarg;
				break;

			case 'l':
				iOptions->logFileName = optarg;
				break;

			case 'h':
				return 0;

			default:
				return -1;
		}		
	}

	if((argc - optind) != 2)
	{
		iOptions->cfgFileName  = iOptions->romFileName = NULL;
		return -1;
	}

	iOptions->cfgFileName = argv[optind];
	iOptions->romFileName = argv[optind+1];

	return 1;
}
