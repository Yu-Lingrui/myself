#ifndef GOLF_H_
#define GOLF_H_

class Golf
{
private:
    static const int Len = 40;
    char fullname[Len];
    int handicap;

public:
    Golf(const char *name, int hc); //默认构造函数;
    Golf();                         //用户定义构造函数;
    void set_handicap(int hc);
    void showgolf() const;
};

#endif
