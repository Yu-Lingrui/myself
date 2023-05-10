#include <iostream>
using namespace std;

int main()
{
    long long num;
    long long sum = 0LL;

    while (cout << "Please enter an integer(0 to quit): ", cin >> num && num != 0)
    // for(; cin >> num && num != 0; )
    {
        //↑逗号运算符只取最后的结果作为判断条件;
        sum += num;
        cout << "Sum of all integers are " << sum << endl;
    }

    return 0;
}
