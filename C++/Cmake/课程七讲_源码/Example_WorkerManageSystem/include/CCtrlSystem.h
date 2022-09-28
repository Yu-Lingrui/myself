#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "CWorker.h"
#include "CEmployee.h"
#include "CManager.h"
#include "CBoss.h"

#define FILENAME "database.txt"

using std::cin;
using std::cout;
using std::endl;

using std::ifstream;
using std::ios;
using std::ofstream;

class CCtrlSystem
{
public:
    CCtrlSystem();

    void showMenu() const;
    void exitSystem() const;
    void addWorkers();
    void saveFile() const;
    int getSize(ifstream &ifs) const;
    void initArray();
    void showWorkers();
    int isExist(const int& id) const;
    void delWorker();
    void modifyWorker();
    void searchWorker() const;
    void sortWorkers();
    void clearAllWorkers();

    ~CCtrlSystem();

private:
    int _isFileEmpty;
    int _size;
    CWorker **_p_workerArray;
};
