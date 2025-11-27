#include "MyVec.h"
#include <iostream>
#include <stdexcept>

MyVec::MyVec(int size, int value) {
    int capacity = 4;
    while (size >= capacity) capacity *= 2;
    elements = new int[capacity];
    this->capacity = capacity;
    this->size = size;
    for (int i = 0; i < size; i++) elements[i] = value;
}

MyVec::~MyVec() {
    delete[] elements;
}

void MyVec::_copy(const MyVec& orig) {
     this->capacity = orig.capacity;
     this->size = orig.size;
     this->elements = new int[capacity];
    for (int i = 0; i < size; i++) elements[i] = orig.elements[i];
}

MyVec::MyVec(const MyVec& orig) {
    _copy(orig);
}

MyVec& MyVec::operator=(const MyVec& orig) {
    if (this != &orig) {
        delete[] elements;
        _copy(orig);
    }
    return *this;
}

void MyVec::print() {
    std::cout << "C: " << capacity << " S: " << size << ":\n";
    for (int i = 0; i < size; i++) std::cout << elements[i] << " ";
    std::cout << std::endl;
}

void MyVec::push(int val) {
    if (size == capacity) {
        int newcap = capacity * 2;
        int* tmp = new int[newcap];
        for (int i = 0; i < size; i++) tmp[i] = elements[i];
        delete[] elements;
        elements = tmp;
         this->capacity = newcap;
    }
    this-> elements[size++] = val;
}

void MyVec::pop() {
    if (size == 0) return;
    size--;
    if (size > 0 && size <= capacity / 4) {
        int newcap = capacity / 2;
        if (newcap < 4) newcap = 4;
        int* tmp = new int[newcap];
        for (int i = 0; i < size; i++) tmp[i] = elements[i];
        delete[] elements;
        elements = tmp;
        capacity = newcap;
    }
}

int MyVec::operator+(const MyVec& other) {
    return this->size + other.size;
}

void MyVec::sort() {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size - i - 1; j++)
            if (elements[j] > elements[j + 1])
                std::swap(elements[j], elements[j + 1]);
}

int MyVec::at(int index) {
    if (index < 0 || index >= size) throw std::out_of_range("index");
    return elements[index];
}

int MyVec::front() {
    if (size == 0) throw std::out_of_range("empty");
    return elements[0];
}

int MyVec::back() {
    if (size == 0) throw std::out_of_range("empty");
    return elements[size - 1];
}

int* MyVec::data() {
    return elements;
}

int MyVec::operator[](int i) {
    if (i < 0 || i >= size) return 42;
    return elements[i];
}
