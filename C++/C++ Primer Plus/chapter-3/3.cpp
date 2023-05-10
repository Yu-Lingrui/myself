#include<iostream>
#include<climits>
 
int main()
{
    using namespace std;
    
    int n_int = INT_MAX;

    cout << "int is " << sizeof(int) << " bytes." << endl;
    cout << "Maximum values:" << endl;
    cout << "int: " << n_int << endl;

    return 0;
}

 