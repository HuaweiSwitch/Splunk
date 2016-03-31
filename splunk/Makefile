OBJS=netconf

CFLAGS += -g

all: netconf.c
	$(CROSS_COMPILE)gcc $(CFLAGS) $^ -o $(OBJS) -L ./lib -lnetconf


clean c :
	@rm -rf $(OBJS)

e ee:
	./$(OBJS)
	#echo -e "\naaaaaaaaaaa\n"