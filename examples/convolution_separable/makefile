CFLAGS =  -Wall

convolution_separable: convolution_separable.o
	g++ -o convolution_separable convolution_separable.o ../util.o

convolution_separable.o: convolution_separable.cpp
	g++ -c -o convolution_separable.o convolution_separable.cpp $(CFLAGS)

