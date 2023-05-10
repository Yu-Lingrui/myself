#include <iostream>
using namespace std;

long double factorial(long double n);

int main()
{
    long double temp;

    cout << "Please enter a number(<0 or q to quit): ";
    while (cin >> temp && temp >= 0)
    {
        cout << temp << "! = " << factorial(temp) << endl;
        cout << "Next number(<0 or q to quit): ";
    }
    cout << "Bye." << endl;

    return 0;
}

long double factorial(long double n)
{
    if (0 == n)
    {
        return 1;
    }
    else
    {
        return n * factorial(n - 1);
    }
}
