CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g3 -lm

Heatmap: heatmap.o location.o trackpoint.o track.o
	${CC} ${CFLAGS} -o $@ $^

Unit: track_unit.c location.o trackpoint.o track.o
	${CC} ${CFLAGS} -o $@ $^

heatmap.o: heatmap.c
	${CC} ${CFLAGS} -c heatmap.c

track.o: track.h track.c 
	${CC} ${CFLAGS} -c track.c

trackpoint.o: trackpoint.h trackpoint.c
	${CC} ${CFLAGS} -c trackpoint.c

location.o: location.h location.c
	${CC} ${CFLAGS} -c location.c

