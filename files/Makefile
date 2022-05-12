all: main manufacturer consumer

main: main.c ring.c ring.h
	gcc main.c ring.c -o main

manufacturer: manufacturer.c
	gcc manufacturer.c ring.c -o manufacturer

consumer: consumer.c
	gcc consumer.c ring.c -o consumer

clear:
	rm main manufacturer consumer
