/*******************************************************************************
* Author:      Andrew Seligman
* Date:        March 18, 2016
* File:        logger.c
* Purpose:     This file contains the implementation of logger.h. See logger.h 
*               for documentation.
*******************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <logger.h>

void logEvent(char* filename, char* message) {
	FILE* fPtr;
	
	if(fPtr = fopen(filename, "a")) {
		writeEvent(message, fPtr);
		fclose(fPtr);
	}
	else
		perror("logger: open file");
}

void writeEvent(char* message, FILE* fPtr) {
	if(fprintf(fPtr, "%.24s", getDate()) < 0) {
		perror("logger: write date");
		return;
	}
	if(fprintf(fPtr, "%s", "  ") < 0) {
		perror("logger: write space");
		return;
	}
	if(fprintf(fPtr, "%s", message) < 0) {
		perror("logger: write message");
		return;
	}
	if(fprintf(fPtr, "%s", "\n") < 0)
		perror("logger: write newline");
}

char* getDate() {
	time_t timer;
	
	time(&timer);
	return ctime(&timer);
}

/* functionality test
int main(void) {
	logEvent("test.txt", "a test message");
	sleep(2);
	logEvent("test.txt", "a test message 2");
	return 0;
}
*/
