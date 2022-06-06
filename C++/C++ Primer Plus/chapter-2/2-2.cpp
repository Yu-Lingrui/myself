#include <iostream>
using namespace std;

int main()
{
    double long_distance;

    cout << "Enter a distance for long unit: ";
    cin >> long_distance;

    cout << long_distance << " long distance ";
    cout << "is equal to ";
    long_distance *= 220.0;
    cout << long_distance << " yard distance." << endl;

    return 0;
}

