SRC := text.cpp ../util.cpp
EXECUTABLE := text
OBJECTS := ispc.o


include ../main.mk

ispc.o: text.ispc
	ispc text.ispc -o ispc.o -h ispc.h
