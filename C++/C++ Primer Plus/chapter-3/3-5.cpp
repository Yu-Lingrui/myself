#include <iostream>
using namespace std;

int main()
{
    long long world_population;
    long long china_population;

    cout << "Enter the world's population: ";
    cin >> world_population;
    cout << "Enter the population of the China: ";
    cin >> china_population;

    cout << "The population of the China is ";
    cout << double(china_population) / world_population * 100LL;
    cout << "% of the world population." << endl;

    return 0;
}
