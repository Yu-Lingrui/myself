#include <iostream>
using namespace std;

int main()
{
    int i, j, row;

    cout << "Enter number of rows: ";
    cin >> row;

    for (i = 1; i <= row; i++)
    {
        for (j = i; j <= row - 1; j++)
        {
            cout.put('.');
        }
        for (j = 1; j <= i; j++)
        {
            cout.put('*');
        }
        cout.put('\n');
    }

    return 0;
}
