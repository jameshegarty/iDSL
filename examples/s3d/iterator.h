#ifndef __ITERATOR_H__
#define __ITERATOR_H__

class Iterator{
    public:
        Iterator(int n0i,int n1i,int n2i,int ng0i,int ng1i,int ng2i);
        ~Iterator();

        bool Valid();
        void Next();
        int I0();
        int I1();
        int I2();

    private:
        int n0;
        int n1;
        int n2;
        int ng0;
        int ng1;
        int ng2;
        int index0;
        int index1;
        int index2;
        int position;
        int product;
};
#endif
