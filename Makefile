build:
	gcc -pthread -o s-talk list.o controller.c main.c

test:
	gcc -pthread -D test test.c

clean:
	rm s-talk