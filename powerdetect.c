/*******************************************************************************
* Author:      Andrew Seligman
* Date:        March 9, 2016
* File:        powerdetect.c
* Purpose:     This program detects whether a button is pressed on a gamepad at
*               specific times of the day. If so, an SMS message is sent to a
*               cell phone number.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <logger.h>

#define GAMEPAD "/dev/input/js0"
#define LOGFILE "[path]/powerdetect/log/general.txt"
#define PHONE "5555555555"
#define TEXTMESSAGE "Message to be sent"
#define REFERENCEFILE "[path]/powerdetect/reference.dat"
#define RESPONSEFILE "[path]/powerdetect/response.dat"

struct js_event {
	unsigned int time;
	short value;
	unsigned char type;
	unsigned char number;
};

int openGamepad();
int handleEvents(int, int);
void handleIntermittentEvents(int, time_t*);
int buttonRecentlyPressed(time_t);
int isTime();
void getTime(char*);
void sendText(char*);

int main(void)
{
	int fd;
	//int buttonStatus = 0;
	time_t lastPress;
	struct tm* baseTime;
	char timeBuffer[6];
	char textMessageBuffer[100];
	char buffer[200];
	
	printf("Program started\n");
	//set lastPress to time in past
	time(&lastPress);
	baseTime = localtime(&lastPress);
	baseTime->tm_year = baseTime->tm_year - 1;
	lastPress = mktime(baseTime);
	
	fd = openGamepad();
	sleep(5); //wait for gamepad events to fill queue
	
	while(1) {
		//buttonStatus = handleEvents(fd, buttonStatus);
		//if(buttonStatus == 1 && isTime())
		//	sendText(message);
		//printf("Status result = %d \nSleeping.\n", buttonStatus);
		
		handleIntermittentEvents(fd, &lastPress);
		if(buttonRecentlyPressed(lastPress) && isTime()) {
			getTime(timeBuffer);
			sprintf(textMessageBuffer, "%s PM %s", timeBuffer, TEXTMESSAGE);
			sprintf(buffer, "curl -X POST http://textbelt.com/text -d number=%s -d \"message=%s\" > %s",
							PHONE, textMessageBuffer, RESPONSEFILE);
			sendText(buffer);
			printf("Message sent\n");
		}
		sleep(110);
	}
}

/*******************************************************************************
* Function name:  openGamepad
*                                                                             
* Description:    Opens the device located at GAMEPAD path
*                                                                             
* Parameters:     none
*                                                                             
* Return Value:   file descriptor to open device
*******************************************************************************/
int openGamepad() {
	int fd = open(GAMEPAD, O_RDONLY | O_NONBLOCK);
	if(fd < 0){
		perror("open");
		exit(1);
	}
	return fd;
}

/*******************************************************************************
* Function name:  handleEvents
*                                                                             
* Description:    Reads the events generated by the gamepad and checks a 
*                  particular gamepad button to determine whether the generated
*                  event pertains to this button. If so, the variable that 
*                  reflects the button's pressed status is updated
*                                                                             
* Parameters:     int fd        - IMPORT - file descriptor from which to read
*                 int oldStatus - IMPORT - previous status of the button
*                                                                             
* Return Value:   1 if button is pressed, 0 if not; or oldStatus if the
*                  generated event belongs to a different button
*******************************************************************************/
int handleEvents(int fd, int oldStatus) {
	struct js_event e;
	int newStatus = oldStatus;
	
	//process all gamepad events in the queue
	while(read(fd, &e, sizeof(e)) > 0) {
		if(e.number == 4)
			newStatus = e.value;
		//printf("Event %d processed: %d. Status = %d \n", e.number, e.value, newStatus);
	}
	//EAGAIN is returned when the queue is empty
	if(errno != EAGAIN) {
		perror("read event");
		exit(1);
	}
	
	return newStatus;
}

/*******************************************************************************
* Function name:  handleIntermittentEvents
*                                                                             
* Description:    An alternative to handleEvents. Reads the events generated by 
*                  the gamepad and checks a particular gamepad button to 
*                  determine whether the generated event pertains to this 
*                  button. If so, and the button's pressed status is 'on', the
*                  current time is saved to lastPress
*                                                                             
* Parameters:     int fd           - IMPORT - file descriptor from which to read
*                 time_t* lastPress - EXPORT - time when button was last 
*                                              detected as pressed
*                                                                             
* Return Value:   none
*******************************************************************************/
void handleIntermittentEvents(int fd, time_t* lastPress) {
	struct js_event e;
	int detected = 0;
	
	//process all gamepad events in the queue
	while(read(fd, &e, sizeof(e)) > 0) {
		if(e.number == 7 && e.value == 1)
			detected = 1;
		//printf("Event %d processed: %d.\n", e.number, e.value);
	}
	//EAGAIN is returned when the queue is empty
	if(errno != EAGAIN) {
		perror("read event");
		exit(1);
	}
	if(detected) {
		time(lastPress);
		logEvent("[path]/powerdetect/log/detected.txt", "Power detected");
	}
}

/*******************************************************************************
* Function name:  buttonRecentlyPressed
*                                                                             
* Description:    Compares the given time to the current time and returns 
*                  whether secondLimit seconds have passed at maximum
*                  
* Parameters:     time_t lastPress - IMPORT - time when button was last 
*                                              detected as pressed
*                                                                             
* Return Value:   1 if the current time is at most secondLimit seconds after 
*                 lastPress; 0 otherwise
*******************************************************************************/
int buttonRecentlyPressed(time_t lastPress) {
	double secondLimit = 2;
	time_t current = time(NULL);
	if(difftime(current, lastPress) <= secondLimit)
		return 1;
	return 0;
}

/*******************************************************************************
* Function name:  isTime
*                                                                             
* Description:    Returns whether the current time falls within a specified
*                  timeframe.
*                                                                          
* Parameters:     none
*                                                                             
* Return Value:   1 if current time is 8pm-11pm and :00 or :01 past the hour;
*                 0 otherwise
*******************************************************************************/
int isTime() {
	time_t timer;
	struct tm* currentTime;
	char buffer[6];
	int hour, min;
	char* endPtr;
	
	time(&timer);
	currentTime = localtime(&timer);
	strftime(buffer, 6, "%H %M", currentTime);
	
	hour = (int)strtol(buffer, &endPtr, 10);
	min = (int)strtol(endPtr, NULL, 10);

	//time values here must give consideration to duration of sleep in main()
	//to avoid repeat signals
	if(hour > 19 && min < 2) {
		printf("Time: %s\n", buffer);
		sleep(12);
		return 1;
	}
	return 0;
}

/*******************************************************************************
* Function name:  getTime
*                                                                             
* Description:    Copies the current hour and minute (12h format) into buffer
*                                                                          
* Parameters:     char* buffer - EXPORT - current hour and minute in 12h format
*                                                                             
* Return Value:   none
*******************************************************************************/
void getTime(char* buffer) {
	time_t timer;
	struct tm* currentTime;

	time(&timer);
	currentTime = localtime(&timer);
	strftime(buffer, 6, "%I:%M", currentTime);
}

/*******************************************************************************
* Function name:  sendText
*                                                                             
* Description:    Executes a shell command which is formatted to transmit an SMS
*                  message and store the command's response in a file. This 
*                  response is compared to the known success message which is 
*                  stored in a separate file. If the SMS transmission is not
*                  successful, the command will be executed again up to a total
*                  of five times
*                                                                             
* Parameters:     char* message - IMPORT - shell command to execute
*                                                                             
* Return Value:   none
*******************************************************************************/
void sendText(char* message) {
	FILE* pFile1;
	FILE* pFile2;
	int ch1, ch2;
	int attempts = 5;
	char buffer[80];
	
	while(attempts > 0) {
		remove(RESPONSEFILE);
		if(system(message) == -1) {
			perror("SMS init");
		}
		else {
			//check response from SMS service
			if(pFile1 = fopen(RESPONSEFILE, "r")) {
				if(pFile2 = fopen(REFERENCEFILE, "r")) {
					do {
						ch1 = fgetc(pFile1);
						ch2 = fgetc(pFile2);
					}
					while(ch1 != EOF && ch2 != EOF && ch1 == ch2);
					
					fclose(pFile1);
					fclose(pFile2);
					
					if(ch1 == ch2) {
						logEvent(LOGFILE, "SMS transmitted successfully");
						return;
					}
				}
				else
					perror("open reference file");
			}
			else
				perror("open response file");
		}
		attempts--;
		sprintf(buffer, "SMS transmission unsuccessful. %d attempts remaining", attempts);
		logEvent(LOGFILE, buffer);
		sleep(60);
	}
	logEvent(LOGFILE, "SMS transmission failed");
}
