#include "CManager.h"

CManager::CManager(/* args */)
{
}

CManager::CManager(const int id,const string name,const int level)
{
    this->_id = id;
    this->_name = name;
    this->_level = level;
}

void CManager::showInfo()
{
    cout << "id: "       << this->_id
         << "\tname: "   << this->_name
         << "\tlevel:"   << this->getLevel()
         << "\tduty:"    << "to do that boss want to do, and lead employee's work."
         << endl;
}

string CManager::getLevel()
{
    return string("Manager");
}

CManager::~CManager()
{
}
