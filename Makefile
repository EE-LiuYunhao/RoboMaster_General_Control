testing.exe:testing.o Control.o tesing.o
	gcc -std=c99 tesing.o Control.o testing.o -o testing.exe
tesing.o:tesing.c testing.h bool.h Control.h
	gcc tesing.c -c -std=c99
Control.o:Control.c Control.h bool.h
	gcc Control.c -c -std=c99
testing.o:testing.c testing.h bool.h Control.h
	gcc testing.c -c -std=c99
test:testing.exe
	./testing.exe < input.txt >output.txt
