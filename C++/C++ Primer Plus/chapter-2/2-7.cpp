#include<iostream>
using namespace std;

void show_time(int h,int m);

int main(){
    int h,m;

    cout<<"Enter the number of hours ";
    cin>>h;
    cout<<"Enter the number of minutes ";
    cin>>m;
    show_time(h,m);

    return 0;
}

void show_time(int h,int m){
    cout<<"Time: "<<h<<':'<<m<<endl;
}
