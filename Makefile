build:
	gcc -pthread -o main main.c list.c monitor.c

clean:
	rm main