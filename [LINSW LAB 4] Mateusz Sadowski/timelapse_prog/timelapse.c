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

#define CONFIGURATION_FILE "/usr/webcam/config.txt"

static volatile int captureDelay;
static volatile int isCapturing;
static volatile char *saveLocation;

void usage(char *programName);
void processArguments(int argc, char *argv[]);
void setup();
void run();
void capture();
void flashLED();
void importConfigurationFromFile();
void saveConfigurationToFile();
FILE *openConfig();
void closeFile(FILE *config);
void toggleCapture();
void decreaseTimeInterval();
void increaseTimeInterval();
void deBounce(int pin);
int deBounceButton();


int main(int argc, char *argv[])
{
	processArguments(argc, argv);
	setup();

	run();

	return EXIT_SUCCESS;
}

void usage(char *programName)
{
	fprintf(stderr, "USAGE: %s t\n", programName);
	fprintf(stderr, "t - number of seconds to elapse between each capture");
}

void processArguments(int argc, char *argv[])
{
	if (argc > 2)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	if (2 == argc)
		captureDelay = atoi(argv[1]);
}

void setup()
{
	//Setup pins with BCM-GPIO pin numbering
	if (-1 == wiringPiSetupSys())
	{
		perror("wiringPiSetupSys");
		exit(EXIT_FAILURE);
	}

	//Register handlers for interrupts on buttons
	if (-1 == wiringPiISR(BUTTON_3, INT_EDGE_BOTH, &toggleCapture))
	{
		perror("wiringPiISR");
		exit(EXIT_FAILURE);
	}
	if (-1 == wiringPiISR(BUTTON_2, INT_EDGE_BOTH, &decreaseTimeInterval))
	{
		perror("wiringPiISR");
		exit(EXIT_FAILURE);
	}
	if (-1 == wiringPiISR(BUTTON_1, INT_EDGE_BOTH, &increaseTimeInterval))
	{
		perror("wiringPiISR");
		exit(EXIT_FAILURE);
	}

	//Reset RED LED on start
	digitalWrite(RED_LED, 0);

	importConfigurationFromFile();
}

void run()
{
	while (1)
	{
		printf("Waiting to start capturing...\n");
		capture();
		sleep(DELAY_TIMEOUT);
	}
}

void capture()
{
	importConfigurationFromFile();
	while (isCapturing)
	{
		printf("Captured a photo. Time interval: %d\n", captureDelay);
		system("/usr/bin/webcam.sh > /dev/null 2>/dev/null");
		flashLED();
		sleep(captureDelay);
		importConfigurationFromFile();
	}
}

void flashLED()
{
	struct timespec tim;
	tim.tv_sec = 0;
	tim.tv_nsec = FLASHLED_TIME;
	digitalWrite(RED_LED, 1);

	if (nanosleep(&tim, NULL) < 0)
	{
		printf("Nano sleep system call failed \n");
		return;
	}

	digitalWrite(RED_LED, 0);
}

void importConfigurationFromFile()
{
	char buff[255];
	FILE *config = openConfig();

	if (-1 == fseek(config, 0, SEEK_SET))
		perror("fseek");
	if (EOF == fscanf(config, "%s", buff))
		perror("fscanf");
	isCapturing = atoi(buff);

	if (EOF == fscanf(config, "%s", buff))
		perror("fscanf");
	captureDelay = atoi(buff);
	//printf("Status: %d, Time interval: %d\n", isCapturing, captureDelay);

	closeFile(config);
}

void saveConfigurationToFile()
{
	FILE *config = openConfig();
	fprintf(config, "%d %d", isCapturing, captureDelay);
	closeFile(config);
}

FILE *openConfig()
{
	FILE *config;
	if (NULL == (config = fopen(CONFIGURATION_FILE, "r+")))
	{
		perror("Error opening file.\n");
		exit(EXIT_FAILURE);
	}

	return config;
}

void closeFile(FILE *file)
{
	if (EOF == fclose(file))
		fprintf(stderr, "Error closing file.\n");
}

void toggleCapture()
{
	if (0 == deBounceButton(BUTTON_3))
		return;
	//printf("Button %d pressed.\n", BUTTON_3);

	if (1 == isCapturing)
		isCapturing = 0;
	else
		isCapturing = 1;

	saveConfigurationToFile();
}

void decreaseTimeInterval()
{
	if (0 == deBounceButton(BUTTON_2))
		return;
	//printf("Button %d pressed.\n", BUTTON_2);

	if (isCapturing)
	{
		printf("Can not change time interval while capturing.\n");
		return;
	}

	if (captureDelay > 1)
	{
		captureDelay = captureDelay - 1;
		saveConfigurationToFile();
	}
}

void increaseTimeInterval()
{
	if (0 == deBounceButton(BUTTON_1))
		return;
	//printf("Button %d pressed.\n", BUTTON_1);

	if (isCapturing)
	{
		printf("Can not change time interval while capturing.\n");
		return;
	}

	captureDelay = captureDelay + 1;
	saveConfigurationToFile();
}

int deBounceButton(int button)
{
	deBounce(button);
	//Safe to read state
	int state = digitalRead(button);

	if (state == 1)
	{
		return 0;
	}

	return 1;
}

void deBounce(int pin)
{
	int result, i = 0;
	while (0 != (result = waitForInterrupt(pin, DEBOUNCING_TIMEOUT)))
	{ //result == 1 -> successful interrupt event
		//printf("Debouncing for %d time...\n", i++);
		if (-1 == result)
		{
			exit(EXIT_FAILURE);
		}
	}
	//printf("Debouncing timed out. Proceeding with state read...\n");
}
