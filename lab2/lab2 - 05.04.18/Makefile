CC=$(CROSS_COMPILE)gcc
OBJS := signal-lights.o
signal-lights: $(OBJS)
	$(CC) -o signal-lights $(OBJS) -l wiringPi
$(OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@