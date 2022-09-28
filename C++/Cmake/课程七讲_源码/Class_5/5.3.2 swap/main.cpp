#include "swap.h"

int main(int argc, char **argv)
{
    swap myswap(10, 20);
    std::cout << "Before swap:" << std::endl;
    myswap.printInfo();
    myswap.run();
    std::cout << "After  swap:" << std::endl;
    myswap.printInfo();

    return 0;

}