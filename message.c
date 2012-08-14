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
#include "message.h"

static const char* g_MessageTypeLabel[] =
{
	"[Error]",
	"[Warning] ",
	"[Information] "
};

static FILE *g_Stream;

/**
 * \brief Open the file where the message will be printed. And set hook to print into a file.
 *
 * \param filename  Name of the file where the message will be printed.
 * \return 0 if an error occured, 1 on success.
*/
int PrintMsgOpenFile(const char* filename)
{
	g_Stream = stderr;
	if(filename != NULL)
	{
		FILE *out;
		out = fopen(filename, "wb+");
		if(out == NULL)
		{
			ERROR_MSG("An error occured while trying to open %s : %s", filename, strerror(errno));
			return 0;
		}
		g_Stream = out;
	}
	return 1;
}

/**
 * \brief Close file used for printing messages.
*/
static void PrintMsgCloseFile()
{
	if((g_Stream != NULL) && (g_Stream != stderr))
	{
		fclose(g_Stream);
	}
}

/**
 * \brief Print message to a file.
 *
 * \param type      Message type.
 * \param file      Name of the file where the print message command was issued.
 * \param line      Line number in the file where the print message command was issued.
 * \param function  Function where the print message command was issued.
 * \param format    Format string.
 */
static void PrintMsgFile(MessageType msgType, const char* file, size_t line, const char* function, const char* format, ...)
{
	va_list argList;

	fprintf(g_Stream, "%s %s:%lu %s : ", g_MessageTypeLabel[msgType], file, line, function);

	va_start(argList, format);
	vfprintf(g_Stream, format, argList);
	va_end(argList);
	
	fputc('\n', g_Stream);
	fflush(g_Stream);
}

PrintMsgProc      PrintMsg      = PrintMsgFile;
PrintMsgCloseProc PrintMsgClose = PrintMsgCloseFile; 
