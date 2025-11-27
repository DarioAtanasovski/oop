#ifndef MYVEC_H
#define MYVEC_H

class MyVec {
private:
    int size;
    int capacity;
    int *elements;

    void _copy(const MyVec& orig);
public:
    MyVec(int size = 0, int value = 0);
    ~MyVec();
    MyVec(const MyVec& orig);
    MyVec& operator=(const MyVec& orig);
    void print();
    void push(int val);
    void pop();

    int operator[](int i);
};
#endif //MYVEC_H