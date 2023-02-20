build:
	gcc -pthread -o s-talk main.c list.c controller.c

test:
	gcc -pthread -D test test.c

clean:
	rm s-talk