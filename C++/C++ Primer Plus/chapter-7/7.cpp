#include <iostream>
using namespace std;

int main()
{

	/*
		数组指针&指针数组
		*/
	int(*arrPtr1)[10] = NULL;
	int *arrPtr2[3];
	int matrix[3][10];
	// arrPtr是一个指向(有十个元素)数组的指针
	// matrix是指向首元素(有十个元素数组)的指针(一共有三个元素)
	arrPtr1 = matrix;
	//[]是寻址符：（*arrPtr）[i] 等同于 *（（*arrPtr）+i）
	(*arrPtr1)[0] = 5;
	//arrPtr1++; // 加了4*10:40,16进制：28
	(*(arrPtr1+1))[0] = 7; // matrix[1][0] = 7
	arrPtr2[0] = matrix[0];
	cout << matrix[0][0] << endl;
	cout << matrix[1][0] << endl;
	cout << arrPtr1[0][0] << endl;
	cout << arrPtr1[0] << endl;
	cout << arrPtr2[0] << endl;
	cout << *arrPtr2[0] << endl;
}