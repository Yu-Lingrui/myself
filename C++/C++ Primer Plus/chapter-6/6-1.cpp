#include <iostream>
#include <cctype>
using namespace std;

int main()
{
    char ch;

    cout << "Type, and I shall repeat(@ to quit)." << endl;
    while (cin.get(ch) && ch != '@')
    {
        if (islower(ch))
        {
            ch = toupper(ch);
        }
        else if (isupper(ch))
        {
            ch = tolower(ch);
        }
        if (!isdigit(ch))
        {
            cout.put(ch);
        }
    }
    cout << "\nPlease excuse the slight confusion." << endl;

    return 0;
}
