CXX=g++

all:
	mkdir -p objs/
	g++ hypterm.cpp -Iobjs -O2 -m64 -c -o objs/hypterm.o
	ispc -O2 --arch=x86-64  --target=sse2,sse4-x2,avx-x2 hypterm.ispc -o hypterm_ispc.o -h hypterm_ispc.h
	g++ -Iobjs/ -O2 -m64 -o hypterm main.cpp objs/hypterm.o objs/hypterm_ispc.o objs/hypterm_ispc_sse2.o objs/hypterm_ispc_sse4.o objs/hypterm_ispc_avx.o -lm -pthread -lstdc++ -lrt
clean:
	rm -rf hypterm objs
