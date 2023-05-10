#include <iostream>
#include <cstring>
using namespace std;

int main()
{
    const int ArSize = 20;
    char fname[ArSize];
    char lname[ArSize];

    cout << "Enter your first name: ";
    cin.getline(fname, ArSize);
    cout << "Enter your last name: ";
    cin.getline(lname, ArSize);

    int len = strlen(fname) + strlen(lname) + 10;
    char name[len];
    strcpy(name, lname);
    strcat(name, ", ");
    strcat(name, fname);

    cout << "Here's the information in a single string: ";
    cout << name << endl;

    return 0;
}
