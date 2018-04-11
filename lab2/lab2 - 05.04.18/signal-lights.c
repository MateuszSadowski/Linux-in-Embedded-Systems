#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
//#include <poll.h>

#define BLUE_LED 17
#define WHITE_LED 18
#define GREEN_LED 23
#define RED_LED 24
#define BUTTON_1 27
#define BUTTON_2 22
#define BUTTON_3 10

#define MAIN_DELAY 100
#define SIGNAL_DELAY 500
#define HAZARD_DELAY 800

#define DEBOUNCING_TIMEOUT 100

static volatile int isLeftSignalOn = 0;
static volatile int isRightSignalOn = 0;
static volatile int isHazardLightsOn = 0;

void setup();
void signalTurn(int pin);
void signalHazard(int pin);
void deBounce(int pin);

void switchSignalTurn(int pin);
void switchSignalHazard();

void usage(char* programName);

int main(int argc, char* argv[])
{
	if(1 != argc)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	setup();

	while(1)
	{	//Wait for interrupt again and again
		printf("Waiting for interrupt...\n");
		while(1)
		{	//Wait for interrupt
			delay(MAIN_DELAY);
		}
	}
	
    return EXIT_SUCCESS;
}

void setup()
{
	//Setup pins with BCM-GPIO pin numbering
    if(-1 == wiringPiSetupSys())
    {
		//ERR("wiringPiSetup");
		exit(EXIT_FAILURE);
	}
	
	//TODO: Is it necessary to set up mode for all pins again??

	//Register handlers for interrupts on buttons
	if(-1 == wiringPiISR(BUTTON_1, INT_EDGE_BOTH, &signalTurn(BUTTON_1)))
	{
		exit(EXIT_FAILURE);
	}
	if(-1 == wiringPiISR(BUTTON_2, INT_EDGE_BOTH, &signalTurn(BUTTON_2)))
	{
		exit(EXIT_FAILURE);
	}
	if(-1 == wiringPiISR(BUTTON_3, INT_EDGE_BOTH, &signalHazard(BUTTON_3))
	{
		exit(1);
	}
}

void signalTurn(int pin)
{
	deBounce(pin);
	//Safe to read state
	switchSignalTurn(pin);
	
	while(1 == isLeftSignalOn || 1 == isRightSignalOn)
	{
		if(pin == BUTTON_1)
		{
			digitalWrite(BLUE_LED, 1);
			delay(SIGNAL_DELAY);
			digitalWrite(WHITE_LED, 1);
			delay(SIGNAL_DELAY);
			digitalWrite(GREEN_LED, 1);
			delay(SIGNAL_DELAY);
			digitalWrite(RED_LED, 1);
			delay(SIGNAL_DELAY);
		}
		else
		{
			digitalWrite(RED_LED, 1);
			delay(SIGNAL_DELAY);
			digitalWrite(GREEN_LED, 1);
			delay(SIGNAL_DELAY);
			digitalWrite(WHITE_LED, 1);
			delay(SIGNAL_DELAY);
			digitalWrite(BLUE_LED, 1);
			delay(SIGNAL_DELAY);
		}

		digitalWrite(BLUE_LED, 0);
		digitalWrite(WHITE_LED, 0);
		digitalWrite(GREEN_LED, 0);
		digitalWrite(RED_LED, 0);
	}
}

void signalHazard(int pin)
{
	deBounce(pin);
	//Safe to read state
	switchSignalHazard();

	while(isHazardLightsOn)
	{
		digitalWrite(BLUE_LED, 1);
		digitalWrite(WHITE_LED, 1);
		digitalWrite(GREEN_LED, 1);
		digitalWrite(RED_LED, 1);
		delay(HAZARD_DELAY);
		digitalWrite(BLUE_LED, 0);
		digitalWrite(WHITE_LED, 0);
		digitalWrite(GREEN_LED, 0);
		digitalWrite(RED_LED, 0);
	}
}

void deBounce(int pin)
{
	int result, i = 0;
	while(0 != (result = waitForInterrupt(pin, DEBOUNCING_TIMEOUT)))	//TODO: Choose best time period
	{	//result == 1 -> successful interrupt event
		printf("Debouncing for %d time...\n", i++);
		if(-1 == result)
		{
			exit(EXIT_FAILURE);
		}
	}
	printf("Debouncing timed out. Proceeding with state read...\n");
}

void switchSignalTurn(int pin)
{
	isHazardLightsOn = 0;

	if(pin == BUTTON_1)
	{
		isRightSignalOn = 0;
		if(isLeftSignalOn)
		{
			isLeftSignalOn = 0;
			printf("STOPPED SIGNAL TURN LEFT\n");
		}
		else
		{
			isLeftSignalOn = 1;
			printf("STARTED SIGNAL TURN LEFT\n");
		}
	}
	else
	{
		isLeftSignalOn = 0;
		if(isRightSignalOn)
		{
			isRightSignalOn = 0;
			printf("STOPPED SIGNAL TURN RIGHT\n");
		}
		else
		{
			isRightSignalOn = 1;
			printf("STARTED SIGNAL TURN RIGHT\n");
		}
	}
}

void switchSignalHazard(int pin)
{
	isLeftSignalOn = 0;
	isRightSignalOn = 0;
	if(isHazardLightsOn)
	{
		isHazardLightsOn = 0;
		printf("STOPPED HAZARD LIGHTS\n");
	}
	else
	{
		isHazardLightsOn = 1;
		printf("STARTED HAZARD LIGHTS\n");
	}
}

void usage(char *programName)
{
    fprintf(stderr, "USAGE: %s\n", programName);
	fprintf(stderr, "Function accepts NO parameters.");
}