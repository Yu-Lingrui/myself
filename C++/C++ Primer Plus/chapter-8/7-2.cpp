#include <iostream>
#include <cstring>
using namespace std;

struct CandyBar
{
    char *brand; //char brand[20];
    double weight;
    int calorie;
};

void set(CandyBar &candy, char *b = "Millennium Mucnh", double w = 2.85, int c = 350);
void show(const CandyBar &candy);

int main()
{
    CandyBar bar;
    set(bar);
    cout << "set is OK" << endl;
    show(bar);

    return 0;
}

void set(CandyBar &candy, char *b, double w, int c)
{
    candy.brand = b; //strcpy(candy.brand, b);
    candy.weight = w;
    candy.calorie = c;
}

void show(const CandyBar &candy)
{
    cout << candy.brand << endl;
    cout << candy.weight << endl;
    cout << candy.calorie << endl;
}
