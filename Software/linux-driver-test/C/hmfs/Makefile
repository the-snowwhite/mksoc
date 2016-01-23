# obj := hmfs.o

#KERNEL_SRC_DIR=/home/mib/Development/Projects/arm-linux-gnueabifh-kernel/linux
#PWD=$(shell pwd)
CROSS_C=/home/mib/Development/Projects/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-
CC=$(CROSS_C)g++

all:  hmfs

hmfs:  main.o hmfs.o
	$(CC) -o hmfs main.o hmfs.o
	
main.o: main.cpp
	$(CC) -c main.cpp

hmfs.o: hmfs.cpp
	$(CC) -c hmfs.cpp

clean:
	rm -rf *.o
	rm -rf hmfs
