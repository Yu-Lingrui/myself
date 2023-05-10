#include <iostream>
using namespace std;
const int HOUR_PER_DAY = 24;      // 1天有24小时;
const int MINUTE_PER_HOUR = 60;   // 1小时有60分钟;
const int SECOND_PER_MINUTE = 60; // 1分钟有60秒;

int main()
{
    long long second;

    cout << "Enter the number of seconds: ";
    cin >> second;

    cout << second << " seconds = ";
    cout << second / (HOUR_PER_DAY * MINUTE_PER_HOUR * SECOND_PER_MINUTE);
    cout << " days, ";
    cout << second / (SECOND_PER_MINUTE * MINUTE_PER_HOUR) % HOUR_PER_DAY;
    cout << " hours, ";
    cout << second % (SECOND_PER_MINUTE * MINUTE_PER_HOUR) / SECOND_PER_MINUTE;
    cout << " minutes, ";
    cout << second % SECOND_PER_MINUTE;
    cout << " seconds" << endl;

    return 0;
}
