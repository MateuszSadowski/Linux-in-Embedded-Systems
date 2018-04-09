#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

// #define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
//                      perror(source),kill(0,SIGKILL),\
//                      exit(EXIT_FAILURE))

//#define GPIO17 0

void gainFullControl(int);

int main(int argc, char* argv[])
{
    int i;
	int fd;
	int ret;
	struct pollfd pfd;
	char c;

    //gainFullControl(0);

    if(wiringPiSetup() == -1)
    {
        //ERR("wirintPiSetup")
        exit(1);
    }

    //pinMode(0, OUTPUT);


	//Enable gpio25
	fd = open("/sys/class/gpio/export", O_WRONLY);
	write(fd, "22", 2);
	close(fd);

	//Set gpio25 as input
	fd = open("/sys/class/gpio/gpio22/direction", O_WRONLY);
	write(fd, "in", 2);
	close(fd);

	//Set gpio25 interrupt
	fd = open("/sys/class/gpio/gpio22/edge", O_WRONLY);
	write(fd, "falling", 7);
	//write(fd, "both", 4);
	close(fd);

	//Wait for event, repeat 10 times
	fd = open("/sys/class/gpio/gpio22/value", O_RDONLY);
	pfd.fd = fd;
	pfd.events = POLLPRI;
	for(i = 1; i <= 10; i++){
		lseek(fd, 0, SEEK_SET);
		ret = poll(&pfd, 1, 3000);
		read(fd, &c, 1);
		printf("%d: ", i);
		if(ret == 0)
			printf("Timeout\n");
		else
			if(c == '0')
				printf("Push\n");
			else
				printf("Release\n", i, c);
	}
	close(fd);

    // while(1)
    // {
    //     digitalWrite (GPIO17, HIGH); delay (500);
    //     digitalWrite (GPIO17, LOW); delay (500); 
    // }

    //Disable gpio25
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	write(fd, "22", 1);
	close(fd);

    return 0;
}

void gainFullControl(int pin)
{
    // int fd;
    // char buffer [33];
    // //Enable gpio17
    // itoa (pin,buffer,10)
	// fd = open("/sys/class/gpio/export", O_WRONLY);
	// write(fd, buffer, 1);
	// close(fd);

	// //Set gpio17 as output
    // char* path;
    // if(sprintf(path, "/sys/class/gpio/gpio%d/direction", pin) < 1)
    // {
    //     ERR("sprintf");
    // }
	// fd = open(path, O_WRONLY);
	// write(fd, "out", 3);
    // close(fd);

    // //Set gpio25 interrupt
	// fd = open("/sys/class/gpio/gpio25/edge", O_WRONLY);
	// //write(fd, "falling", 7);
	// write(fd, "both", 4);
	// close(fd
    char* command;
    if(sprintf(command, "echo %d > /sys/class/gpio/export; echo out > /sys/class/gpio/gpio%d/direction", pin) < 1)
    {
        //ERR("sprintf");
        exit(1);
    }
    system(command);
}