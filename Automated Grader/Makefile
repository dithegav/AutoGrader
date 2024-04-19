project4: project4.o functions.o
	gcc -Wall -g project4.o functions.o -o project4 -fsanitize=address
project4.o: project4.c functions.h
	gcc -Wall -g -c project4.c
functions.o: functions.c functions.h
	gcc -Wall -g -c functions.c