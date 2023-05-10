#include <iostream>
using namespace std;

int main()
{
    const int INCH_PER_FEET = 12;
    int height;

    cout << "Please enter your height in inch: ___\b\b\b";
    cin >> height;
    cout << "Your height is " << height / INCH_PER_FEET << " feet and ";
    cout << height % INCH_PER_FEET << " inches." << endl;

    return 0;
}
