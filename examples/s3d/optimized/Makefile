CXX=g++

all:
	g++ hypterm.cpp -Iobjs -O3 -funroll-loops -m64 -c -o objs/hypterm.o
	g++ hypterm_threaded.cpp -Iobjs -O3 -funroll-loops -m64 -c -o objs/hypterm_threaded.o
	g++ -Iobjs/ -O3 -funroll-loops -m64 -o hypterm main.cpp objs/hypterm.o objs/hypterm_threaded.o objs/hypterm_ispc.o objs/hypterm_ispc_sse2.o objs/hypterm_ispc_sse4.o objs/hypterm_ispc_avx.o -lm -pthread -lstdc++ -lrt
ispc:
	mkdir -p objs/
	ispc -O3 --arch=x86-64  --target=sse2,sse4-x2,avx-x2 hypterm.ispc -o objs/hypterm_ispc.o -h objs/hypterm_ispc.h
clean:
	rm -rf hypterm objs
