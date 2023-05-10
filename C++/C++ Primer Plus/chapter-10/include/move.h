#ifndef MOVE_H_
#define MOVE_H_

class Move
{
private:
    double x;
    double y;

public:
    Move(double a = 0, double b = 0);       // 重构构造函数;
    void showmove() const;                  // show;
    Move add(const Move &m);                // x = m + x; y = m + y; return x,y;
    void reset(double a = 0, double b = 0); // reset x,y = a,b;
};

#endif
