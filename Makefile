
fft: fft.c
	gcc -pthread -std=gnu11 fft.c  -lm -o fft -g

fft2: fft2.c
	gcc -pthread -std=gnu11 fft2.c  -lm -o fft2 -g

poly: poly.c
	gcc -std=gnu11 poly.c  -lm -o poly -g
