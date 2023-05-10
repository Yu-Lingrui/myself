#include <iostream>
#include <cctype>
#include "stack.h"

int main()
{
    using namespace std;
    char ch;
    Stack st;
    Item temp;
    double total = 0.0;

    cout << "a to add a customer." << endl;
    cout << "d to delete a customer." << endl;
    cout << "q to exit the menu." << endl;
    cout << "Please enter your choice: ";
    while (cin >> ch && tolower(ch) != 'q')
    {
        while (cin.get() != '\n')
            continue;
        if (tolower(ch) != 'a' && tolower(ch) != 'd') //处理错误选择;
        {
            cout << "Please enter a, d or q: ";
            continue;
        }
        switch (tolower(ch))
        {
	        case 'a':
	        {
	            cout << "Enter the customer's fullname: ";
	            cin.getline(temp.fullname, 35);
	            cout << "Enter the customer's payment: ";
	            while (!(cin >> temp.payment)) //处理错误非数值输入;
	            {
	                cin.clear();
	                while (cin.get() != '\n')
	                    continue;
	                cout << "Please enter an number: ";
	            }
	            if (st.isfull())
	            {
	                cout << "Can't add new customer." << endl;
	            }
	            else
	            {
	                st.push(temp);
	            }
	            break;
	        }
	        case 'd':
	        {
	            if (st.isempty())
	            {
	                cout << "No any customer.\n";
	            }
	            else
	            {
	                st.pop(temp);
	                total += temp.payment; //累计payment的数值;
	                cout << "Customer " << temp.fullname << " will quit." << endl;
	                cout << "Now the total payments are: " << total << endl; //报告当前total的总数;
	            }
	            break;
	        }
        }
        cout << "\n\n\n";
        cout << "a to add a customer." << endl;
        cout << "d to delete a customer." << endl;
        cout << "q to exit the menu." << endl;
        cout << "Please enter your choice: ";
    }
    cout << "Bye." << endl;

    return 0;
}
