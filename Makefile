build:
	gcc -pthread -o main main.c list.c controller.c

test:
	gcc -pthread -D test test.c

clean:
	rm main