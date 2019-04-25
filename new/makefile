FLAGS = -std=c99 -Wall -Wextra -g
OBJS = cda.o queue.o main.o

all: main

main: $(OBJS)
	gcc $(FLAGS) $(OBJS) -o main -lm

main.o: main.c cda.h queue.h
	gcc $(FLAGS) -c main.c

cda.o: cda.c cda.h
	gcc $(FLAGS) -c cda.c

queue.o: queue.c queue.h cda.h
	gcc $(FLAGS) -c queue.c

clean:
	rm -f $(OBJS) main p*