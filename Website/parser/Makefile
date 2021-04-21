CC = gcc
CFLAGS = -Wall -std=c11 -pedantic -g -lm
LFLAGS = -c -fpic

all: parser
parser: ../libgpxparser.so

bin/liblist.o: ./include/LinkedListAPI.h ./src/LinkedListAPI.c
	$(CC) $(CFLAGS) $(LFLAGS) -I/usr/include/libxml2 -I./include ./src/LinkedListAPI.c -o ./bin/liblist.o -lxml2

bin/helper.o: ./include/GPXHelper.h ./src/GPXHelper.c
	$(CC) $(CFLAGS) $(LFLAGS) -I/usr/include/libxml2 -Iinclude ./src/GPXHelper.c -o ./bin/helper.o -lxml2

bin/libgpx.o: ./include/GPXParser.h ./src/GPXParser.c ./include/GPXHelper.h
	$(CC) $(CFLAGS) $(LFLAGS) -I/usr/include/libxml2 -I./include ./src/GPXParser.c -o ./bin/libgpx.o -lxml2

test: ./include/GPXParser.h ./src/GPXParser.c ./include/GPXHelper.h ./include/LinkedListAPI.h ./src/LinkedListAPI.c ./src/GPXHelper.c ./src/main.c
	$(CC) $(CFLAGS) -I./include `xml2-config --cflags --libs` -I/usr/include/libxml2 ./src/GPXParser.c ./src/LinkedListAPI.c ./src/GPXHelper.c ./src/main.c -o main -lxml2

../libgpxparser.so: ./bin/liblist.o ./bin/libgpx.o ./bin/helper.o
	$(CC) -shared ./bin/liblist.o ./bin/libgpx.o ./bin/helper.o -o ../libgpxparser.so -lxml2

clean:
	rm -rf ./bin/*.o
	rm -rf ./bin/*.so
	rm -rf ../*.so
