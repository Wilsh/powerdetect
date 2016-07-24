/*******************************************************************************
* Author:      Andrew Seligman
* Date:        March 18, 2016
* File:        logger.h
* Purpose:     This file contains function prototypes for logger.c, which
*               provides an interface for logging program events. A program
*               may use this interface to append a message to a file by calling
*               logEvent(file, message). The date and time of the message is
*               automatically prepended, and a newline character is 
*               automatically appended.
*******************************************************************************/

#ifndef LOGGER
#define LOGGER

/*******************************************************************************
* Function name:  logEvent
*                                                                             
* Description:    The only publicly-accessible fuction. Appends the given 
*                  message to the given file
*                                                                             
* Parameters:     char* filename - IMPORT - file to write to
*                 char* message  - IMPORT - message to write
*                                                                             
* Return Value:   none
*******************************************************************************/
extern void logEvent(char* filename, char* message);

/*******************************************************************************
* Function name:  writeEvent
*                                                                             
* Description:    Writes a message to an open file. The message will be 
*                  prepended with the current date and time and appended with a
*                  newline character
*                                                                             
* Parameters:     char* message - IMPORT - message to write
*                 char* fPtr    - IMPORT - pointer to file where message will 
*                                          be appended
*                                                                             
* Return Value:   none
*******************************************************************************/
static void writeEvent(char* message, FILE* fPtr);

/*******************************************************************************
* Function name:  getDate
*                                                                             
* Description:    Returns a character pointer to the current time
*                                                                             
* Parameters:     none
*                                                                             
* Return Value:   Current time in ctime() format
*******************************************************************************/
static char* getDate();

#endif
