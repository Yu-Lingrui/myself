#pragma once
#include <string>

class Gun
{
public:
    Gun(std::string type){
        this->_bullet_count = 0;
        this->_type = type;
    }

    void addBullet(int bullet_num);
    bool shoot();

private:
    int _bullet_count;
    std::string _type;

};