#include <iostream>
#include "move.h"

Move::Move(double a, double b) // 构造函数;
{
    x = a;
    y = b;
}

void Move::showmove() const
{
    std::cout << "x:"  << x << " y:" << y << std::endl;
}

Move Move::add(const Move &m)
{   
    Move temp;
    temp.x = x + m.x;
    temp.y = y + m.y;
    return temp;
}

void Move::reset(double a, double b)
{
    x = a;
    y = b;
}

