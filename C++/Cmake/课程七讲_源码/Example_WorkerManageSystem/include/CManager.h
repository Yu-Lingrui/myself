#pragma once 
#include <iostream>
#include <string>
#include "CWorker.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

class CManager:public CWorker
{
private:
    /* data */
public:
    CManager();
    CManager(const int id,const string name,const int level);
    string getLevel() override;
    void showInfo() override;
    // void addWorker() override;
    // void modifyWorker() override;
    ~CManager();
};
