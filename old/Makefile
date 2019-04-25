FLAGS = -std=c99 -Wall -Wextra -g
OBJS = cda.o queue.o gen.o

all: gen

gen: $(OBJS)
	gcc $(FLAGS) $(OBJS) -o gen -lm

gen.o: gen.c cda.h queue.h
	gcc $(FLAGS) -c gen.c

cda.o: cda.c cda.h
	gcc $(FLAGS) -c cda.c

queue.o: queue.c queue.h cda.h
	gcc $(FLAGS) -c queue.c

clean:
	rm -f $(OBJS) gen p*