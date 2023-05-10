#include <iostream>
#include <array>
using namespace std;

int main()
{
    int i = 0, j = 0;
    unsigned int count = 0;
    const int ArSize = 10;
    array<double, ArSize> donations;
    double total = 0.0, average = 0.0;

    cout << "You may enter up to " << ArSize;
    cout << " donation (q to terminate)." << endl;
    cout << "donation #1: ";
    while (i < ArSize && cin >> donations[i])
    {
        if (++i < ArSize)
        {
            cout << "donation #" << i + 1 << ": ";
        }
    }

    for (j = 0; j < i; j++)
    {
        total += donations[j];
    }
    average = total / i;
    for (j = 0; j < i; j++)
    {
        if (average < donations[j])
        {
            ++count;
        }
    }

    if (0 == i)
    {
        cout << "No donation!" << endl;
    }
    else
    {
        cout << average << " = average of ";
        cout << i << " donations.\n";
        cout << count << " numbers are greater than the average." << endl;
    }

    return 0;
}
