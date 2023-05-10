#include <iostream>
#include "move.h"

int main()
{
    Move a;
    Move b(1, 2);
    a.showmove();
    b.showmove();
    a = a.add(b);
    b = b.add(a);
    a.showmove();
    b.showmove();
    a.reset(1, 2);
    b.reset(1, 2);
    a.showmove();
    b.showmove();
    return 0;
}
