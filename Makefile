
fft: fft.c
	gcc -pthread -std=gnu11 fft.c  -lm -o fft -g

poly: poly.c
	gcc -std=gnu11 poly.c  -lm -o poly -g
