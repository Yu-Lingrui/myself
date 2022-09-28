#include <iostream>
using namespace std;

int main(int argc,char **argv)
{
    int N = 100;
    int sum = 0;
    int i = 1;
	
// calculate sum from 1 to 100
    while (i <= N)
    {
        sum = sum + i;
        i = i + 1;
    }

// print the result
    cout << "sum = " << sum << endl;
    cout << "The program is over."   << endl;
    
    return 0;
}
