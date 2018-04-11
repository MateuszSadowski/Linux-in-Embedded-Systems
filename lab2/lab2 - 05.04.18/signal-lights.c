#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

// #define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
//                      perror(source),kill(0,SIGKILL),\
//                      exit(EXIT_FAILURE))

//enum Direction { Left = 0, Right = 1};

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

//void signalTurn(Direction);
void setup();
void signalTurn(int pin);
void signalHazard(int pin);
void deBounce(int pin);

int main(int argc, char* argv[])
{
	setup();
	while(true)
	{
		delay(MAIN_DELAY);
	}
    return 0;
}

void setup()
{
	//Setup pins with BCM-GPIO pin numbering
    if(-1 == wiringPiSetupSys())
    {
		//ERR("wiringPiSetup");
		exit(1);
	}
	
	//TODO: Is it necessary to set up mode for all pins again??

	//Register handlers for interrupts on buttons
	// wiringPiISR(BUTTON_1, INT_EDGE_BOTH, &signalTurn(Direction.Left));
	// wiringPiISR(BUTTON_2, INT_EDGE_BOTH, &signalTurn(Direction.Right));
	// wiringPiISR(BUTTON_3, INT_EDGE_BOTH, &signalTurn(Direction.signalHazard));
	if(-1 == wiringPiISR(BUTTON_1, INT_EDGE_BOTH, &signalTurn(BUTTON_1)))
	{
		exit(1);
	}
	if(-1 == wiringPiISR(BUTTON_2, INT_EDGE_BOTH, &signalTurn(BUTTON_2)))
	{
		exit(1);
	}
	if(-1 == wiringPiISR(BUTTON_3, INT_EDGE_BOTH, &signalHazard(BUTTON_3))
	{
		exit(1);
	}
}

void signalTurn(int pin)
{
	deBounce(pin);

	while(true)
	{
		if(pin == BUTTON_1)
		{
			printf("SIGNAL LEFT\n");
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
			printf("SIGNAL RIGHT\n");
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
	
	printf("STARTED HAZARD LIGHTS\n");

	while(true)
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
	while(0 != (result = waitForInterrupt(pin, DEBOUNCING_TIMEOUT)))		//TODO: Choose best time period
	{
		printf("Debouncing for %d time...\n", i++);
		if(-1 == result)
		{
			exit(1);
		}
	}
	printf("Debouncing timed out. Proceeding with state read...\n");
}