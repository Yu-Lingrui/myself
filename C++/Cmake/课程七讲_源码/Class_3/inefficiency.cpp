#include <iostream>
using namespace std;

int main(void)
{
    unsigned long int counter;
    unsigned long int result;
    unsigned long int temp;
    unsigned int five;
    int i;

    for (counter = 0; counter < 2009 * 2009 * 100 / 4 + 2010; counter += (10 - 6)/4)
    {
        temp = counter/1979;
        for (i = 0; i < 20; i++)
        // 每一次for循环 都会进行复杂的计算
            five = 200 * 200 / 8000;
            result = counter;
    }
    
    cout << "result = " << result << endl; 

    return 0;
}
