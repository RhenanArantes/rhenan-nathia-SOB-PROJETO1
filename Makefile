obj-m += projeto1SOB.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) programaTeste.c -o programaTeste
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) programaTeste
