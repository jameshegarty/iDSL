CXX=g++
CXX_FLAGS:=-O3 -funroll-loops

all:
	$(CXX) $(CXX_FLAGS) -o hypterm main.cpp hypterm.cpp hypterm.h constants.h -pthread -lrt
clean:
	rm -f hypterm
