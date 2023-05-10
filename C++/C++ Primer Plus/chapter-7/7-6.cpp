#include <iostream>
using namespace std;

int Fill_array(double arr[], int n);
void Show_array(const double arr[], int n);
void Reverse_average(double arr[], int n);

const int ArSize = 10;

int main()
{
    int n;
    double arr[ArSize];

    n = Fill_array(arr, ArSize);
    Show_array(arr, n);
    Reverse_average(arr, n);
    Show_array(arr, n);
    Reverse_average(arr + 1, n - 2); // &arr[1]
    Show_array(arr, n);
    cout << "Bye." << endl;

    return 0;
}

int Fill_array(double arr[], int n)
{
    int i = 0;

    cout << "You can enter up to " << ArSize;
    cout << " numbers (q to terminate)." << endl;

    cout << "Number #1: ";
    while (i < n && cin >> arr[i])
    {
        if (++i < ArSize)
        {
            cout << "Number #" << i + 1 << ": ";
        }
    }
    return i;
}

void Show_array(const double arr[], int n)
{
    cout << "All numbers:" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << arr[i] << ' ';
    }
    cout.put('\n');
    return;
}

void Reverse_average(double arr[], int n)
{
    double temp;

    for (int i = 0; i < n / 2; i++)
    {
        temp = arr[n - 1 - i];
        arr[n - 1 - i] = arr[i];
        arr[i] = temp;
    }
    return;
}
