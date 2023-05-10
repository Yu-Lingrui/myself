#include <iostream>
using namespace std;

int main()
{
    int n = 0;
    double daphne_money = 100;
    double cleo_money = 100;

    while (cleo_money <= daphne_money)
    {
        cout << "Year " << ++n << ':' << endl;
        daphne_money += 10;
        cleo_money += cleo_money * 0.05;
        cout << "Cleo's money = " << cleo_money;
        cout << ", Daphne's money = " << daphne_money << endl;
    }
    cout << "After " << n << " years, ";
    cout << "Cleo's money";
    cout << " > Daphne's money." << endl;

    return 0;
}
