#pragma once
#include <iostream>

class swap
{
public:
    swap(int a, int b){
        this->_a = a;
        this->_b = b;
    }
    void run();
    void printInfo();
private:
    int _a;
    int _b;
};