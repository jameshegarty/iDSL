CFLAGS =  -Wall

convolution: convolution.o
	g++ -o convolution convolution.o ../util.o

convolution.o: convolution.cpp
	g++ -c -o convolution.o convolution.cpp $(CFLAGS)

