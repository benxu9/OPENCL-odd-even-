CC=gcc
FLAGS=-O3 -Wall -Wextra -Wpedantic

all:main

main: parallel.c
	$(CC) $(FLAGS) parallel.c -o main -l OpenCL

serial: serial.c
	$(CC) $(FLAGS) serial.c -o serial

clean:
	rm -f main serial
