FLAGS = -std=c99 -Wall -Wextra -g
OBJS = cda.o queue.o process.o main.o memoryAllocator.o lru.o

all: main

main: $(OBJS)
	gcc $(FLAGS) $(OBJS) -o main -lm

main.o: main.c cda.h queue.h
	gcc $(FLAGS) -c main.c

cda.o: cda.c cda.h
	gcc $(FLAGS) -c cda.c

queue.o: queue.c queue.h cda.h
	gcc $(FLAGS) -c queue.c

process.o: process.c process.h
	gcc $(FLAGS) -c process.c

memoryAllocator.o: memoryAllocator.c memoryAllocator.h
	gcc $(FLAGS) -c memoryAllocator.c

lru.o: lru.c lru.h
	gcc $(FLAGS) -c lru.c

clean:
	rm -f $(OBJS) main requests results