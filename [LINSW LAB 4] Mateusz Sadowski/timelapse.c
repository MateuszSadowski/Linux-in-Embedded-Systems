#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define BLUE_LED 17
#define WHITE_LED 18
#define GREEN_LED 23
#define RED_LED 24
#define BUTTON_1 10
#define BUTTON_2 22
#define BUTTON_3 27

#define DEBOUNCING_TIMEOUT 100
#define DELAY_TIMEOUT 1
#define FLASHLED_TIME 500000000L

#define CAPTUREDELAY_MINVALUE 1
#define CAPTUREDELAY_MAXVALUE 86400

#define CONFIGURATION_FILE "config.txt"

static volatile int captureDelay = 1;
static volatile char* saveLocation;

void usage(char* programName);
void setup();
void toggleCapture();
void capture();
void scanAndUpdateCaptureDelay(FILE* fp);
FILE* openConfig();
void closeConfig(FILE* config);
int checkIsCapturing(FILE* fp);
void setIsCapturing(FILE* fp, int isCapturing);
void deBounce(int pin);
void deBounceButton();

int main(int argc, char* argv[])
{
	if(argc > 2)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

    if(2 == argc)
    {
		captureDelay = atoi(argv[1]);
    }

	setup();

	while(1)
	{
		printf("Starting to capture every %d seconds...\n", captureDelay);
		capture();
		sleep(DELAY_TIMEOUT);
	}
	
    return EXIT_SUCCESS;
}

void usage(char *programName)
{
    fprintf(stderr, "USAGE: %s t\n", programName);
	fprintf(stderr, "t - number of seconds to elapse between each capture");
}

void setup()
{
	//Setup pins with BCM-GPIO pin numbering
    if(-1 == wiringPiSetupSys())
    {
		perror("wiringPiSetupSys");
		exit(EXIT_FAILURE);
	}

	//Register handlers for interrupts on buttons
	if(-1 == wiringPiISR(BUTTON_1, INT_EDGE_BOTH, &toggleCapture))
	{
		perror("wiringPiISR");
		exit(EXIT_FAILURE);
	}
	
	//Reset RED LED on start
	digitalWrite(RED_LED, 0);
}

void toggleCapture()
{
	if(0 == deBounceButton())
		return;

	FILE* config = openConfig();

    if(1 == checkIsCapturing(config))
	{
		setIsCapturing(config, 0);
	}
	else
	{
		setIsCapturing(config, 1);
	}

	closeConfig(config);
}

int deBounceButton()
{
	deBounce(BUTTON_3);
	//Safe to read state
	int state = digitalRead(BUTTON_3);
	
	if(state == 1)
	{
		return 0;
	}

	return 1;
}

void scanAndUpdateCaptureDelay(FILE* fp)
{
	char buff[255];
	fscanf(fp, "%s", buff);
	fscanf(fp, "%s", buff);
	captureDelay = atoi(buff);
}

FILE* openConfig()
{
	FILE* config;
	if(NULL == (config = fopen (CONFIGURATION_FILE , "r+")))
	{
		fprintf(stderr, "Error opening file.\n");
		exit(EXIT_FAILURE);
    }

	return config;
}

void closeConfig(FILE* config)
{
	if(EOF == fclose(config))
		fprintf(stderr, "Error closing file.\n");
}

int checkIsCapturing(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	char buff[255];
	fscanf(fp, "%s", buff);
	return atoi(buff);
}

void setIsCapturing(FILE* fp, int isCapturing)
{
	char buf[10];
	sprintf(buf, "%d", isCapturing);
	fseek(fp, 0, SEEK_SET);
	fwrite(buf, strlen(buf), 1, fp);
}

void capture()
{
	FILE* config = openConfig();
	scanAndUpdateCaptureDelay(config);
	while(1 == checkIsCapturing(config))
	{
		//printf("Captured a photo. Timeout: %d\n", captureDelay);
		system("./webcam.sh");
		flashLED();
		sleep(captureDelay);
	}
	closeConfig(config);
}

void deBounce(int pin)
{
	int result, i = 0;
	while(0 != (result = waitForInterrupt(pin, DEBOUNCING_TIMEOUT)))
	{	//result == 1 -> successful interrupt event
		//printf("Debouncing for %d time...\n", i++);
		if(-1 == result)
		{
			exit(EXIT_FAILURE);
		}
	}
	//printf("Debouncing timed out. Proceeding with state read...\n");
}

void flashLED()
{
	struct timespec tim;
   	tim.tv_sec = 0;
   	tim.tv_nsec = FLASHLED_TIME;
	digitalWrite(RED_LED, 1);

	if(nanosleep(&tim , NULL) < 0 )   
	{
		printf("Nano sleep system call failed \n");
		return;
	}

	digitalWrite(RED_LED, 0);
}