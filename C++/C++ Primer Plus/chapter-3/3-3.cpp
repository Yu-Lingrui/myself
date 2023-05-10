#include <iostream>
using namespace std;
const int MINUTE_PER_DEGREE = 60;
const int SECOND_PER_MINUTE = 60;

int main()
{

    int degree, minute, second;
    float total;

    cout << "Enter a latitude in degrees, minutes, and seconds:" << endl;
    cout << "First, enter the degrees: ";
    cin >> degree;
    cout << "Next, enter the minutes of arc: ";
    cin >> minute;
    cout << "Finally, enter the seconds of arc: ";
    cin >> second;

    total = degree + float(minute) / MINUTE_PER_DEGREE + float(second) / (MINUTE_PER_DEGREE * SECOND_PER_MINUTE);
    cout << degree << " degrees, "
         << minute << " minutes, "
         << second << " seconds  = "
         << total << " degrees" << endl;

    return 0;
}
