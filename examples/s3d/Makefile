CXX=g++
SRC_CXX:=hypterm.cpp constants.h iterator.h iterator.cpp
EXECUTABLE_CXX:=hypterm_cpp

FORTRAN=gfortran
SRC_FORTRAN:=hypterm.f
EXECUTABLE_FORTRAN:=hypterm_fortran

all:
	$(CXX) -o $(EXECUTABLE_CXX) $(SRC_CXX)
	$(FORTRAN) -o $(EXECUTABLE_FORTRAN) $(SRC_FORTRAN)
clean:
	rm -f hypterm_cpp hypterm_fortran  
