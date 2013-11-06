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
#ifndef OPTIONS_H
#define OPTIONS_H

#include "config.h"

/**
 * Command line options.
 */
struct CommandLineOptions_
{
	uint8_t extractIRQ;  /**< Automatic IRQ extraction flag. */
	uint8_t cdrom;       /**< CDROM disassembly flag. */
	char *cfgFileName;   /**< CFG/Ini filename. */
	char *romFileName;   /**< ROM filename. */
	char *mainFileName;  /**< Main assembly filename. */
	char *logFileName;   /**< Log filename. */
};
typedef struct CommandLineOptions_ CommandLineOptions;

/**
 * Print usage.
 */
void usage();

/**
 * Parse command line options.
 * \param [in] argc Command line argument count.
 * \param [in] argv Command line argument string array.
 * \param [out] iOptions Command line options.
 * \return 
 *		<= 0 if an error occured.
 *		> 0 on success.
 */
int getCommandLineOptions(int argc, char** argv, CommandLineOptions* iOptions);

#endif // OPTIONS_H
