#include <iostream>
#include <string>
using namespace std;

const int HIGH_MONEY = 10000;

struct corporation
{
    string name;
    double money;
};

int main()
{
    int i, num;
    unsigned int patrons = 0;
    unsigned int grand_patrons = 0;

    cout << "Please enter the number of donators: ";
    (cin >> num).get(); //吸收换行符;
    corporation *people = new corporation[num]; //动态数组

    for (i = 0; i < num; i++)
    {
        cout << "Please enter name #" << i + 1 << ": ";
        getline(cin, people[i].name);  //自动吸收输入换行符;
        cout << "Please enter the amount of donation #" << i + 1 << ": ";
        while (!(cin >> people[i].money)) //处理错误输入;
        {
            cin.clear();
            while (cin.get() != '\n')
                continue;
            cout << "Please enter a number: ";
        }
        cin.get(); //吸收正确输入时的换行符;
    }
    for (i = 0; i < num; i++)
    {
        HIGH_MONEY < people[i].money ? ++grand_patrons : ++patrons; //条件运算符代替条件语句;
    }

    cout << "\nGrand Patrons:" << endl;
    if (grand_patrons != 0)
    {
        for (i = 0; i < num; i++)
        {
            if (people[i].money > HIGH_MONEY)
            {
                cout << "Name: " << people[i].name;
                cout << "\nMoney: " << people[i].money << endl;
            }
        }
    }
    else
    {
        cout << "none" << endl;
    }

    cout << "\nPatrons:" << endl;
    if (patrons != 0)
    {
        for (i = 0; i < num; i++)
        {
            if (people[i].money < HIGH_MONEY)
            {
                cout << "Name: " << people[i].name;
                cout << "\nMoney: " << people[i].money << endl;
            }
        }
    }
    else
    {
        cout << "none" << endl;
    }
    delete[] people;

    return 0;
}
