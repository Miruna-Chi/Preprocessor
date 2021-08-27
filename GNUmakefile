CC = gcc
CFLAGS = -Wall -g

build: so-cpp

so-cpp: hashmap.o main.o stringmanip.o inoutmanip.o utils.o preprocessing.o
	$(CC) $^ -o $@

preprocessing.o: preprocessing.c

hashmap.o: hashmap.c

stringmanip.o: stringmanip.c

inoutmanip.o: inoutmanip.c

utils.o: utils.c

main.o: main.c

.PHONY: clean

clean:
	rm -f so-cpp hashmap.o main.o stringmanip.o inoutmanip.o utils.o preprocessing.o