CC=$(CROSS_COMPILE)gcc
OBJS := blink.o
blink: $(OBJS)
	$(CC) -o blink $(OBJS) -l wiringPi
$(OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@