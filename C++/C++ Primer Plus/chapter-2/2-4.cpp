#include <iostream>
using namespace std;

int main()
{
    int age_total_months;

    cout << "Enter your age: ";
    cin >> age_total_months;
    cout << "Your age includes " << age_total_months << " years. or " ;
    cout << age_total_months * 12 << " months." << endl;

    return 0;
}