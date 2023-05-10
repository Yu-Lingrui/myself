#ifndef PERSON_H_
#define PERSON_H_
#include <string>
using namespace std;

class Person
{
private:
    static const int LIMIT = 25;
    string lname;
    char fname[LIMIT];

public:
    Person() //默认构造函数声明和定义;
    {
        lname = "";
        fname[0] = '\0';
    }
    Person(const string &ln, const char *fn = "Heyyou"); //用户构造函数;
    void Show() const;//先显示名
    void FormalShow() const;
};

#endif
