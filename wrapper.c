/*******************************************************************************
* Author:      Andrew Seligman
* Date:        March 12, 2016
* File:        wrapper.c
* Purpose:     This program accepts a program name as a command line argument
*               and creates a new process to run that program. Once the given
*               program exits, this program will catch errors and restart
*               the given program after a short period of time.
*******************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <logger.h>

//number of seconds to wait before restarting given program
#define RESTARTAFTER 30
#define LOGFILE "/home/windsor/code/powerdetect/log/general.txt"

void createChild(char*);

int main(int argc, char** argv)
{
	char* usage = "Usage: %s program_name\n";

	if(argc != 2) {
		printf(usage, argv[0]);
		return 1;
	}

	while(1) {
		createChild(argv[1]);
		sleep(RESTARTAFTER);
	}

	return 0;
}

/*******************************************************************************
* Function name:  createChild
*                                                                             
* Description:    Creates a child process to execute the program name passed in
*                                                                             
* Parameters:     char* program - IMPORT - name of the program to execute
*                                                                             
* Return Value:   none
*******************************************************************************/
void createChild(char* program)
{
	char message[80];
	int status;
	pid_t pid;

	sprintf(message, "Parent PID = %d creating new process", getpid());
	logEvent(LOGFILE, message);
	if((pid = fork()) < 0) {
		perror("fork");
		return;
	}

	if (pid == 0) {
		//child process code
		int childPID = getpid();
		sprintf(message, "Child process created: PID = %d", childPID);
		logEvent(LOGFILE, message);
		if(system(program) == -1) {
			perror("program init");
			exit(-1);
		}
		exit(0);
	}

	//parent process code
	pid = wait(&status);
	if(WIFEXITED(status)) {
		sprintf(message, "PID %d exits: %d", pid, WEXITSTATUS(status));
		logEvent(LOGFILE, message);
	}
	else if(WIFSTOPPED(status)) {
		sprintf(message, "PID %d stopped by: %d", pid, WSTOPSIG(status));
		logEvent(LOGFILE, message);
	}
	else if(WIFSIGNALED(status)) {
		sprintf(message, "PID %d killed by: %d", pid, WTERMSIG(status));
		logEvent(LOGFILE, message);
	}
	else
		perror("waitpid");
}
