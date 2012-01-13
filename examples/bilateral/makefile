CFLAGS =  -Wall

bilateral: bilateral.o
	g++ -g -o bilateral bilateral.o ../util.o

bilateral.o: bilateral.cpp
	g++ -g -c -o bilateral.o bilateral.cpp $(CFLAGS)

