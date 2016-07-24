all: powerdetect wrapper

powerdetect: powerdetect.c logger.o
	gcc -o powerdetect powerdetect.c logger.o -I .

wrapper: wrapper.c logger.o
	gcc -o wrapper wrapper.c logger.o -I .

logger.o: logger.h logger.c
	gcc -c logger.c -I .

clean:
	rm -f *.o core
