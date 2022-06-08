#include <iostream>
using namespace std;

double transform(double temp);

int main()
{
    double light_years;

    cout << "Enter the number of light years: ";
    cin >> light_years;
    cout << light_years << " light years = ";
    cout << transform(light_years);
    cout << " astronomical units." << endl;

    return 0;
}

double transform(double temp)
{
    return temp * 63240.0;
}
