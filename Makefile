build:
	gcc -pthread -o s-talk list.c main.c controller.c

test:
	gcc -pthread -D test test.c

clean:
	rm s-talk