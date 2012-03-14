CXX=g++

all:
	g++ -O3 -funroll-loops -o hypterm hypterm.cpp hypterm.h main.cpp constants.h -lrt -pthread
debug:
	g++ -g -funroll-loops -o hypterm_debug hypterm.cpp hypterm.h main.cpp constants.h -lrt -pthread
ispc:
	mkdir -p objs/
	ispc -O3 --arch=x86-64  --target=sse2,sse4-x2,avx-x2 hypterm.ispc -o objs/hypterm_ispc.o -h objs/hypterm_ispc.h
	g++ hypterm.cpp -Iobjs -O3 -funroll-loops -m64 -c -o objs/hypterm.o
	g++ -Iobjs/ -O3 -funroll-loops -m64 -o hypterm main.cpp objs/hypterm.o objs/hypterm_ispc.o objs/hypterm_ispc_sse2.o objs/hypterm_ispc_sse4.o objs/hypterm_ispc_avx.o -lm -pthread -lstdc++ -lrt
clean:
	rm -rf hypterm hypterm_debug objs
