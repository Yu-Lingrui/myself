#include "CEmployee.h"

CEmployee::CEmployee()
{
}

CEmployee::CEmployee(const int id, const string name, const int level)
{
    this->_id = id;
    this->_name = name;
    this->_level = level;
}

void CEmployee::showInfo()
{
    cout << "id: "       << this->_id
         << "\tname: "   << this->_name
         << "\tlevel:"   << this->getLevel()
         << "\tduty:"    << "to do that manager want to do."
         <<endl;
}

string CEmployee::getLevel()
{
    return string("Employee");
}

CEmployee::~CEmployee()
{
}
