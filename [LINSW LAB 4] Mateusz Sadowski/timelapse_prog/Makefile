CC=$(CROSS_COMPILE)gcc
OBJS := timelapse.o
blink: $(OBJS)
	$(CC) -o timelapse $(OBJS) -l wiringPi
$(OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ -g
