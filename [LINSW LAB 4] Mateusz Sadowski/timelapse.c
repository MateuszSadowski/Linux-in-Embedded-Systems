//#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>

#define BLUE_LED 17
#define WHITE_LED 18
#define GREEN_LED 23
#define RED_LED 24
#define BUTTON_1 10
#define BUTTON_2 22
#define BUTTON_3 27

#define DEBOUNCING_TIMEOUT 100

#define CAPTUREDELAY_MINVALUE 1
#define CAPTUREDELAY_MAXVALUE 86400

#define CONFIGURATION_FILE "config.txt"

static volatile int isCapturing = 1;
static volatile int captureDelay = 1;
static volatile char* saveLocation;

//static sem_t sem;

void usage(char* programName);
void setup();
void toggleCapture();
void capture();
int scanAndUpdateConfiguration(FILE* fp);

int main(int argc, char* argv[])
{
	if(argc > 2)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

    if(2 == argc)
    {
        // char* errStr;
        // if(0 == (captureDelay = (int)strtonum(argv[1], CAPTUREDELAY_MINVALUE, CAPTUREDELAY_MAXVALUE, *errStr)))
        // {
        //     fprintf(stderr, "%s", errStr);
        //     exit(EXIT_FAILURE);
        // }
		captureDelay = atoi(argv[1]);
    }

	// FILE* config;
	// if(NULL == (config = fopen (CONFIGURATION_FILE , "r+")))
	// {
	// 	fprintf(stderr, "Error opening file.\n");
	// 	exit(EXIT_FAILURE);
    // }
	// scanAndUpdateConfiguration(config);

	//setup();

	// printf("Starting to capture every %d seconds...\n", captureDelay);
	// while(1)
	// {

	// }
	
    return EXIT_SUCCESS;
}

void usage(char *programName)
{
    fprintf(stderr, "USAGE: %s t\n", programName);
	fprintf(stderr, "t - number of seconds to elapse between each capture");
}

// void setup()
// {
// 	//Setup pins with BCM-GPIO pin numbering
//     if(-1 == wiringPiSetupSys())
//     {
// 		perror("wiringPiSetupSys");
// 		exit(EXIT_FAILURE);
// 	}

// 	//Register handlers for interrupts on buttons
// 	if(-1 == wiringPiISR(BUTTON_1, INT_EDGE_BOTH, &toggleCapture))
// 	{
// 		perror("wiringPiISR");
// 		exit(EXIT_FAILURE);
// 	}
	
// 	//Reset RED LED on start
// 	digitalWrite(RED_LED, 0);
// }

void toggleCapture()
{
	FILE* config;
	if(NULL == (config = fopen (CONFIGURATION_FILE , "r+")))
	{
		fprintf(stderr, "Error opening file.\n");
		exit(EXIT_FAILURE);
    }

    if(1 == scanAndUpdateConfiguration)
	{
		isCapturing = 0;
		fputs("0", config);
	}
	else
	{
		isCapturing = 1;
		fputs("1", config);
	}

	if(EOF == fclose(config))
		fprintf(stderr, "Error closing file.\n");
}

int scanAndUpdateConfiguration(FILE* fp)
{
	char* errStr;
	char buff[255];
	int i;
	fscanf(fp, "%s", buff);
	// i = (int)strtonum(buff, 0, 1, *errStr);
	// if(0 == i && errStr != NULL)
	// {
	// 	fprintf(stderr, "%s", errStr);
	// 	exit(EXIT_FAILURE);
	// }
	i = atoi(buff);
	printf("str: %s, i: %d\n", buff, i);
	fscanf(fp, "%s", buff);
	// if(0 == (captureDelay = (int)strtonum(buff, CAPTUREDELAY_MINVALUE, CAPTUREDELAY_MAXVALUE, *errStr)))
	// {
	// 	fprintf(stderr, "%s", errStr);
	// 	exit(EXIT_FAILURE);
	// }
	captureDelay = atoi(buff);
	printf("str: %s, i: %d\n", buff, captureDelay);
	fscanf(fp, "%s", buff);
	saveLocation = buff;
	printf("str: %s, i: %s\n", buff, saveLocation);
	return i;
}

// void capture()
// {
// 	while(isCapturing)
// 	{
// 		printf("Captured a photo.\n");
// 		//code here
// 		delay(captureDelay);
// 	}
// }