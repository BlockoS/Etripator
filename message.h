#ifndef MESSAGE_H
#define MESSAGE_H

#include "config.h"

/**
 * \brief Message types
 */
typedef enum
{
	MSG_TYPE_ERROR=0,
	MSG_TYPE_WARNING,
	MSG_TYPE_INFO
} MessageType;

/**
 * \brief Print message hook.
 *
 * \param type      Message type.
 * \param file      Name of the file where the print message command was issued.
 * \param line      Line number in the file where the print message command was issued.
 * \param function  Function where the print message command was issued.
 * \param format    Format string.
 */
typedef void (*PrintMsgProc)(MessageType type, const char* file, size_t line, const char* function, const char* format, ...);

/**
 * \brief Delete, clean up resources used by the print message hook.
 */
typedef void (*PrintMsgCloseProc)();


extern PrintMsgProc PrintMsg;
extern PrintMsgCloseProc PrintMsgClose;

#define ERROR_MSG(format, ...) PrintMsg(MSG_TYPE_ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define WARNING_MSG(format, ...) PrintMsg(MSG_TYPE_WARNING, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define INFO_MSG(format, ...) PrintMsg(MSG_TYPE_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

/**
 * \brief Open the file where the message will be printed. And set hook to print into a file.
 *
 * \param filename  Name of the file where the message will be printed.
 * \return 0 if an error occured, 1 on success.
*/
int PrintMsgOpenFile(const char* filename);

#endif // MESSAGE_H