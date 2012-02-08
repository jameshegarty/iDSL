#include "iterator.h"

Iterator::Iterator(int n0i, int n1i, int n2i, int ng0i, int ng1i, int ng2i):
    n0(n0i), n1(n1i), n2(n2i), ng0(ng0i), ng1(ng1i), ng2(ng2i), index0(0), index1(0), index2(0), position(0), product((n0+2*ng0)*(n1+2*ng1)*(n2+2*ng2)){
}

Iterator::~Iterator(){
}

bool Iterator::Valid(){
    return (position < product);
}

void Iterator::Next(){
    position++;
}

int Iterator::I0(){
    return (position/((n1+2*ng1)*(n2+2*ng2)))%(n0+2*ng0);
}

int Iterator::I1(){
    return (position/(n2+2*ng2))%(n1+2*ng1);
}

int Iterator::I2(){
    return position%(n2+2*ng2);
}
