#pragma once 
#include <iostream>
#include <fstream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

using std::ofstream;
using std::ifstream;
using std::ios;

class CWorker
{

    // friend ofstream& operator<<(ofstream& ofs, const CWorker* worker);
public:
    CWorker();
    virtual void showInfo() = 0;
    virtual string getLevel() = 0;
    // virtual void addWorker() = 0;
    // virtual void modifyWorker() = 0;
    ~CWorker();

public:
    int _id;
    string _name;
    int _level;
};

