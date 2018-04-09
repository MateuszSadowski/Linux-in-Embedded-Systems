#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

// #define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
//                      perror(source),kill(0,SIGKILL),\
//                      exit(EXIT_FAILURE))

enum Direction { Left = 0, Right = 1};

#define BLUE_LED 17
#define WHITE_LED 18
#define GREEN_LED 23
#define RED_LED 24
#define BUTTON_1 27
#define BUTTON_2 22
#define BUTTON_3 10

void turnSignal(Direction);

int main(int argc, char* argv[])
{
    if(wiringPiSetupSys() == -1)
    {
		//ERR("wiringPiSetup");
		exit(1);
	}
	
	//TODO: Is it necessary to set up mode for all pins again??




    return 0;
}

void turnSignal(Direction direction)
{
	while(true)		//TODO: Make it be interruptable by button press
	{
		if(direction == Direction.Left)
		{
			digitalWrite(BLUE_LED, 1);
			delay(500);
			digitalWrite(WHITE_LED, 1);
			delay(500);
			digitalWrite(GREEN_LED, 1);
			delay(500);
			digitalWrite(RED_LED, 1);
			delay(500);
		}
		else
		{
			digitalWrite(RED_LED, 1);
			delay(500);
			digitalWrite(GREEN_LED, 1);
			delay(500);
			digitalWrite(WHITE_LED, 1);
			delay(500);
			digitalWrite(BLUE_LED, 1);
			delay(500);
		}

		digitalWrite(BLUE_LED, 0);
		digitalWrite(WHITE_LED, 0);
		digitalWrite(GREEN_LED, 0);
		digitalWrite(RED_LED, 0);
	}
}