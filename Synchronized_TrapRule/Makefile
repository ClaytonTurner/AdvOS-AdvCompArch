CC=gcc
FLAGS=-lpthread -lm -Wall

Makefile: synchronized_traprule.c 
	$(CC) -o synchronized_traprule synchronized_traprule.c $(FLAGS)

clean:
	find . -type f | xargs -n 5 touch
	rm -rf $(OBJS)
