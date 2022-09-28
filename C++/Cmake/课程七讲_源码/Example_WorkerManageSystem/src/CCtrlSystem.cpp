#include "CCtrlSystem.h"

CCtrlSystem::CCtrlSystem(/* args */)
{
	ifstream ifs;
	ifs.open(FILENAME, ios::in);
	if (!ifs.is_open())
	{
		cout << "File is not exist! " << endl;
		this->_isFileEmpty = true;
		this->_size = 0;
		this->_p_workerArray = nullptr;
		ifs.close();
		return;
	}
	char ch;
	ifs >> ch;
	if (ifs.eof())
	{
		cout << "File exist but is empty! " << endl;
		this->_isFileEmpty = true;
		this->_size = 0;
		this->_p_workerArray = nullptr;
		ifs.close();
		return;
	}
	else
	{
		int size = this->getSize(ifs);
		// cout << "Workers' number is: " << size << endl;
		this->_isFileEmpty = false;
		this->_size = size;
		this->_p_workerArray = new CWorker *[this->_size];
		this->initArray();
	}
}

void CCtrlSystem::showMenu() const// 增删改查 四大功能  + 显 排
{
	cout << "********************************************" << endl;
	cout << "***********  欢迎使用职工管理系统 **********" << endl;
	cout << "*************  0.退出管理程序  *************" << endl;
	cout << "*************  1.增加职工信息  *************" << endl;
	cout << "*************  2.显示职工信息  *************" << endl;
	cout << "*************  3.删除离职职工  *************" << endl;
	cout << "*************  4.修改职工信息  *************" << endl;
	cout << "*************  5.查找职工信息  *************" << endl;
	cout << "*************  6.按照编号排序  *************" << endl;
	cout << "*************  7.清空所有文档  *************" << endl;
	cout << "********************************************" << endl;
	cout << endl;
}

void CCtrlSystem::exitSystem() const
{
	cout << "Goodbye! welcome next time!" << endl;
	exit(0);
}

void CCtrlSystem::addWorkers()
{
	cout << "Please input the numbers to be added: " << endl;
	int addNum;
	cin >> addNum;
	if (addNum > 0)
	{
		// calculate new size
		int newSize = this->_size + addNum;
		// malloc new space
		CWorker **newSpace = new CWorker *[newSize];
		// add old space data to new space
		if (this->_p_workerArray != nullptr)
		{
			for (size_t i = 0; i < this->_size; i++)
			{
				newSpace[i] = this->_p_workerArray[i];
			}
		}

		for (size_t i = this->_size; i < newSize; i++)
		{
			int id;
			string name;
			int level;

			cout << "Please input " << i - this->_size + 1 << " id: " << endl;
			cin >> id;
			cout << "Please input " << i - this->_size + 1 << " name: " << endl;
			cin >> name;
			cout << "Please input " << i - this->_size + 1 << " level: " << endl;
			cout << "1、Employee" << endl;
			cout << "2、Manager" << endl;
			cout << "3、Boss" << endl;
			while (true)
			{
				cin >> level;
				if (level == 1 || level == 2 || level == 3)
				{
					CWorker *worker = nullptr;
					switch (level)
					{
					case 1:
						worker = new CEmployee(id, name, level);
						break;
					case 2:
						worker = new CManager(id, name, level);
						break;
					case 3:
						worker = new CBoss(id, name, level);
						break;
					default:
						break;
					}

					newSpace[i] = worker;

					break;
				}
				else
				{
					cout << "Error input! 1、Employee 2、Manager 3、Boss" << endl;
				}
			}
		}
		// free old ptr
		delete[] this->_p_workerArray;
		// update new space ptr
		this->_p_workerArray = newSpace;
		// update size
		this->_size = newSize;
		// update isFileEmpty flag
		this->_isFileEmpty = false;

		cout << "Added successfully! " << endl;
		this->saveFile();
	}
	else
	{
		cout << "Error input! " << endl;
	}
}

void CCtrlSystem::saveFile() const
{
	ofstream ofs;
	ofs.open(FILENAME, ios::out);

	for (size_t i = 0; i < this->_size; i++)
	{
		ofs << this->_p_workerArray[i]->_id << " "
			<< this->_p_workerArray[i]->_name << " "
			<< this->_p_workerArray[i]->_level << " "
			<< endl;
	}
	ofs.close();
}

int CCtrlSystem::getSize(ifstream &ifs) const
{
	string strLine;
	int lineCount = 0;
	while (std::getline(ifs, strLine))
	{
		lineCount++;
	}
	return lineCount;
}

void CCtrlSystem::initArray()
{
	ifstream ifs;
	ifs.open(FILENAME, ios::in);

	int id, level;
	string name;

	int index = 0;
	while (ifs >> id >> name >> level)
	{
		switch (level)
		{
		case 1:
			this->_p_workerArray[index] = new CEmployee(id, name, level);
			break;
		case 2:
			this->_p_workerArray[index] = new CManager(id, name, level);
			break;
		case 3:
			this->_p_workerArray[index] = new CBoss(id, name, level);
			break;
		default:
			break;
		}
		index++;
	}
	ifs.close();
}

void CCtrlSystem::showWorkers()
{
	if (this->_isFileEmpty)
	{
		cout << "File is not exist or empty! " << endl;
	}
	else
	{
		cout << "Workers' number is: " << this->_size << endl;
		for (size_t i = 0; i < this->_size; i++)
		{
			this->_p_workerArray[i]->showInfo();
		}
		// cout << "" << endl;
	}
}

int CCtrlSystem::isExist(const int& id) const
{
	int index = -1; // ***************important*****************
	for (size_t i = 0; i < this->_size; i++)
	{
		if (id == this->_p_workerArray[i]->_id)
		{
			index = i;
			break;
		}
	}
	return index;
}

void CCtrlSystem::delWorker()
{
	cout << "Please the id you want to delete: " << endl;
	int id;
	cin >> id;

	int index = this->isExist(id);

	if (index != -1)
	{
		for (size_t i = index; i < (this->_size - 1); i++)
		{
			this->_p_workerArray[i] = this->_p_workerArray[i + 1];
		}
		// delete this->_p_workerArray[this->_size - 1]; // added is segment fault
		this->_size--;

		this->saveFile();
		cout << "Delete successfully! " << endl;
	}
	else
	{
		cout << "The id is not exist! " << endl;
	}
}

void CCtrlSystem::modifyWorker()
{
	int inputID;
	cout << "Please input the id you want to modify: " << endl;
	cin >> inputID;

	int index = isExist(inputID);
	if (index != -1)
	{
		int newId, newLevel;
		string newName;
		cout << "Input new ID: " << endl;
		cin >> newId;
		cout << "Input new name: " << endl;
		cin >> newName;
		cout << "Input new level: " << endl;
		cout << "1、Employee" << endl;
		cout << "2、Manager" << endl;
		cout << "3、Boss" << endl;
		cin >> newLevel;

		CWorker *worker = nullptr;
		if (newLevel == 1 || newLevel == 2 || newLevel == 3)
		{
			switch (newLevel)
			{
			case 1:
				worker = new CEmployee(newId, newName, newLevel);
				break;
			case 2:
				worker = new CManager(newId, newName, newLevel);
				break;
			case 3:
				worker = new CBoss(newId, newName, newLevel);
				break;
			default:
				break;
			}
			this->_p_workerArray[index] = worker;
			this->saveFile();
			cout << "Modified successfully! " << endl;
		}
		else
		{
			cout << "Error input! Modified failed!" << endl;
		}
	}
	else
	{
		cout << "No worker founded! " << endl;
	}
}

void CCtrlSystem::searchWorker() const
{
	if (this->_isFileEmpty)
	{
		cout << "File is not exist or empty! " << endl;
		return;
	}

	cout << "Please input search method：" << endl;
	cout << "1、by ID" << endl;
	cout << "2、by name" << endl;
	int inMethod;
	cin >> inMethod;
	if (inMethod == 1)
	{
		int inId = 0;
		cout << "Please input the ID you want to search: " << endl;
		cin >> inId;

		int index = isExist(inId);
		if (index != -1)
		{
			this->_p_workerArray[index]->showInfo();
		}
		else
		{
			cout << "No worker founded! " << endl;
		}
	}
	else if (inMethod == 2)
	{
		string inName;
		cout << "Please input the name you want to search: " << endl;
		cin >> inName;
		bool nameFoundFlag = false; // ****** important *************
		for (size_t i = 0; i < this->_size; i++)
		{
			if (inName == this->_p_workerArray[i]->_name)
			{
				this->_p_workerArray[i]->showInfo();
				nameFoundFlag = true;
			}
		}
		if (!nameFoundFlag)
		{
			cout << "No worker founded! " << endl;
		}
	}
	else
	{
		cout << "Error input! " << endl;
	}

	this->saveFile();
}

void CCtrlSystem::sortWorkers()
{
	if (this->_isFileEmpty)
	{
		cout << "File is not exist or empty! " << endl;
		return;
	}
	else
	{
		cout << "1、Ascending order" << endl;
		cout << "2、Descending order" << endl;
		int inNum;
		cin >> inNum;
		if (inNum == 1)
		{
			for (size_t i = 0; i < this->_size - 1; i++) // total loops
			{
				for (size_t j = 0; j < (this->_size - i - 1); j++) // compare times per loop
				{
					CWorker *temp = nullptr;
					if ((this->_p_workerArray[j]->_id) > (this->_p_workerArray[j + 1]->_id))
					{
						temp = this->_p_workerArray[j];
						this->_p_workerArray[j] = this->_p_workerArray[j + 1];
						this->_p_workerArray[j + 1] = temp;
					}
				}
			}
			cout << "Ascending order successfully " << endl;
		}
		else if (inNum == 2)
		{
			for (size_t i = 0; i < this->_size - 1; i++) // total loops
			{
				for (size_t j = 0; j < (this->_size - i - 1); j++) // compare times per loop
				{
					CWorker *temp = nullptr;
					if (this->_p_workerArray[j]->_id < this->_p_workerArray[j + 1]->_id)
					{
						temp = this->_p_workerArray[j];
						this->_p_workerArray[j] = this->_p_workerArray[j + 1];
						this->_p_workerArray[j + 1] = temp;
					}
				}
			}
			cout << "Descending order successfully " << endl;
		}
		else
		{
			cout << "Error input! " << endl;
		}
	}

	this->saveFile();
}

void CCtrlSystem::clearAllWorkers()
{
	cout << "Are you sure to clear? " << endl;
	cout << "y--clear, n--exit" << endl;
	char inch;
	cin >> inch;
	if (inch == 'y')
	{
		if (this->_isFileEmpty)
		{
			cout << "File is not exist or empty! " << endl;
		}
		else
		{
			ofstream ofs;
			ofs.open(FILENAME, ios::trunc);		 // *********important*********
			if (this->_p_workerArray != nullptr) // *********important*********
			{
				for (size_t i = 0; i < this->_size; i++)
				{
					if (this->_p_workerArray[i] != nullptr)
					{
						delete this->_p_workerArray[i];
						this->_p_workerArray[i] = nullptr;
					}
				}
				delete[] this->_p_workerArray;
				this->_size = 0;
				this->_isFileEmpty = 0;
			}
			cout << "Clear successfully! " << endl;
		}
	}
	else if (inch == 'n')
	{
		cout << "Abort to clear" << endl;
	}
	else
	{
		cout << "Error input! " << endl;
	}
}

CCtrlSystem::~CCtrlSystem()
{
	if (this->_p_workerArray != nullptr)
	{
		delete[] this->_p_workerArray;
	}
}
