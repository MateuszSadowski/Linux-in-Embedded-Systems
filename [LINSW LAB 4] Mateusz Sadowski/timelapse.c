#include <wiringPi.h>
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

static volatile int isCapturing = 0;
static volatile int captureDelay = 1;

static sem_t sem;

void usage(char* programName);

int main(int argc, char* argv[])
{
	if(argc > 2)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

    if(1 == argc)
    {
        char* errStr;
        if(0 == (captureDelay = (int)strtonum(argv[1], CAPTUREDELAY_MINVALUE, CAPTUREDELAY_MAXVALUE, *errStr)))
        {
            fprintf(stderr, errStr);
            exit(EXIT_FAILURE);
        }
    }

	setup();

	printf("Start program...\n");
	while(1)
	{	//Wait for interrupt
	signalTurnLeft();
	signalTurnRight();
	signalHazard();
	printf("Waiting for interrupt again...\n");
	delay(MAIN_DELAY);
	}
	
    return EXIT_SUCCESS;
}

void usage(char *programName)
{
    fprintf(stderr, "USAGE: %s t\n", programName);
	fprintf(stderr, "t - number of seconds to elapse between each capture");
}