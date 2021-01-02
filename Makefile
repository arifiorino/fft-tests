
fft: fft.c
	gcc -pthread -std=gnu11 fft.c  -lm -o fft -g

fft2: fft2.c
	gcc -pthread -std=gnu11 fft2.c  -lm -o fft2 -g

synth: synth.c
	gcc -pthread -std=gnu11 synth.c  -lm -o synth -g

spectrogram: spectrogram.c
	gcc -pthread -std=gnu11 spectrogram.c  -lm -o spectrogram -g

poly: poly.c
	gcc -std=gnu11 poly.c  -lm -o poly -g
