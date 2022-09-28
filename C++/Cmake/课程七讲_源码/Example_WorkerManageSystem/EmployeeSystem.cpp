#include <iostream>
#include <string>
#include "CCtrlSystem.h"
#include "CWorker.h"
#include "CEmployee.h"
#include "CManager.h"
#include "CBoss.h"
#include <unistd.h>
// #include <conio.h>

void test();                           // global declaration
void runSystem(CCtrlSystem &workCtrl); // global declaration
void waitForAnyKey();

int main(int argc, char **argv)
{
    CCtrlSystem workCtrl;
    runSystem(workCtrl);
    return 0;
}

void runSystem(CCtrlSystem &workCtrl)
{
    while (true)
    {
        workCtrl.showMenu();
        int inputChoose = 0;
        cout << "Please input your choose number: " << endl;
        cin >> inputChoose;

        switch (inputChoose)
        {
        case 0: // exitSystem
            workCtrl.exitSystem();
            break;
        case 1: // addWorkers
            workCtrl.addWorkers();
            break;
        case 2: // showWorkers
            workCtrl.showWorkers();
            break;
        case 3: // delWorker
            workCtrl.delWorker();
            break;
        case 4: // modifyWorker
            workCtrl.modifyWorker();
            break;
        case 5: // searchWorker
            workCtrl.searchWorker();
            break;
        case 6: // sortWorkers
            workCtrl.sortWorkers();
            break;
        case 7: // clearAllWorkers
            workCtrl.clearAllWorkers();
            break;
        default:
            cout << "Input error, please try again! " << endl;
            break;
        }
        waitForAnyKey();
        int ret = system("clear");
    }
}

void test()
{
    CWorker *pWorker = nullptr;
    pWorker = new CEmployee(1, "John", 1);
    pWorker->showInfo();

    delete pWorker;
    pWorker = new CManager(2, "Summer", 2);
    pWorker->showInfo();

    delete pWorker;
    pWorker = new CBoss(3, "Jonney", 3);
    pWorker->showInfo();

    cout << endl
         << "-------------------------------------------" << endl;
}

void waitForAnyKey()
{
    cout << "Press any key to continue..." << endl;
    // system("read"); // Continues when pressed a key like windows
    // sleep(100);
    char ch;
    cin >> ch;
    // ch = getchar();
    // getch();
}