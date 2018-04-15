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

#define MAIN_DELAY 1000
#define SIGNAL_DELAY 200
#define HAZARD_DELAY 800

#define DEBOUNCING_TIMEOUT 100

static volatile int isLeftSignalOn = 0;
static volatile int isRightSignalOn = 0;
static volatile int isHazardLightsOn = 0;

static sem_t sem;

void setup();
void signalTurnLeft();
void signalTurnRight();
void signalHazard();
void signalTurnLeftHandler();
void signalTurnRightHandler();
void signalHazardHandler();
void deBounce(int pin);

void switchSignalTurn(int pin);
void switchSignalHazard();

void usage(char* programName);
void printButton(int pin);

int main(int argc, char* argv[])
{
	if(1 != argc)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
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

void setup()
{
	//Setup pins with BCM-GPIO pin numbering
    if(-1 == wiringPiSetupSys())
    {
		perror("wiringPiSetupSys");
		exit(EXIT_FAILURE);
	}

	//Register handlers for interrupts on buttons
	if(-1 == wiringPiISR(BUTTON_1, INT_EDGE_BOTH, &signalTurnLeftHandler))
	{
		perror("wiringPiISR");
		exit(EXIT_FAILURE);
	}
	if(-1 == wiringPiISR(BUTTON_2, INT_EDGE_BOTH, &signalTurnRightHandler))
	{
		perror("wiringPiISR");
		exit(EXIT_FAILURE);
	}
	if(-1 == wiringPiISR(BUTTON_3, INT_EDGE_BOTH, &signalHazardHandler))
	{
		perror("wiringPiISR");
		exit(1);
	}
	
	//Reset LEDS on start
	digitalWrite(BLUE_LED, 0);
	digitalWrite(WHITE_LED, 0);
	digitalWrite(GREEN_LED, 0);
	digitalWrite(RED_LED, 0);
}

void signalTurnLeftHandler()
{
	deBounce(BUTTON_1);
	//Safe to read state
	int state = digitalRead(BUTTON_1);
	
	if(state == 1)
	{
		return;
	}
	printButton(BUTTON_1);
	switchSignalTurn(BUTTON_1);
}

void signalTurnLeft()
{
	if(0 == isLeftSignalOn)
	{
		digitalWrite(BLUE_LED, 0);
		digitalWrite(WHITE_LED, 0);
		digitalWrite(GREEN_LED, 0);
		digitalWrite(RED_LED, 0);
	}

	while(1 == isLeftSignalOn)
	{
		digitalWrite(BLUE_LED, 1);
		delay(SIGNAL_DELAY);
		digitalWrite(WHITE_LED, 1);
		delay(SIGNAL_DELAY);
		digitalWrite(GREEN_LED, 1);
		delay(SIGNAL_DELAY);
		digitalWrite(RED_LED, 1);
		delay(SIGNAL_DELAY);

		digitalWrite(BLUE_LED, 0);
		digitalWrite(WHITE_LED, 0);
		digitalWrite(GREEN_LED, 0);
		digitalWrite(RED_LED, 0);
		delay(SIGNAL_DELAY);
	}
}

void signalTurnRightHandler()
{
	deBounce(BUTTON_2);
	//Safe to read state
	int state = digitalRead(BUTTON_2);
	
	if(state == 1)
	{
		return;
	}
	printButton(BUTTON_2);
	switchSignalTurn(BUTTON_2);
}

void signalTurnRight()
{
	if(0 == isRightSignalOn)
	{
		digitalWrite(BLUE_LED, 0);
		digitalWrite(WHITE_LED, 0);
		digitalWrite(GREEN_LED, 0);
		digitalWrite(RED_LED, 0);
	}

	while(1 == isRightSignalOn)
	{
		digitalWrite(RED_LED, 1);
		delay(SIGNAL_DELAY);
		digitalWrite(GREEN_LED, 1);
		delay(SIGNAL_DELAY);
		digitalWrite(WHITE_LED, 1);
		delay(SIGNAL_DELAY);
		digitalWrite(BLUE_LED, 1);
		delay(SIGNAL_DELAY);

		digitalWrite(BLUE_LED, 0);
		digitalWrite(WHITE_LED, 0);
		digitalWrite(GREEN_LED, 0);
		digitalWrite(RED_LED, 0);
		delay(SIGNAL_DELAY);
	}
}

void signalHazardHandler()
{
	deBounce(BUTTON_3);
	//Safe to read state
	int state = digitalRead(BUTTON_3);
	
	if(state == 1)
	{
		return;
	}
	printButton(BUTTON_3);
	switchSignalHazard();
}

void signalHazard()
{
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
		delay(HAZARD_DELAY);
	}
}

void deBounce(int pin)
{
	int result, i = 0;
	while(0 != (result = waitForInterrupt(pin, DEBOUNCING_TIMEOUT)))
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

void printButton(int pin)
{
	printf("[Pressed button on pin %d]\n", pin);
}
