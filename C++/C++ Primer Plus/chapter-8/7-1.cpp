#include <iostream>
using namespace std;

void print_string(const char *str, int n = 0);

int main()
{
    const char *str = "This is a test.";
    print_string(str);
    print_string(str);
    print_string(str, 3);

    return 0;
}

void print_string(const char *str, int n)
{
    static int count = 0;
    count++;
    if (n == 0)
    {
        cout << "=0 " << endl;
        cout << str << endl;
    }
    else{
        cout << "!=0 " << endl;
        for (size_t i = 0; i < count; i++)
        {
            cout << str << endl;
        }
    }  
}
