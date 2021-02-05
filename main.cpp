#include "BackstageData.h"

#include <iostream>

using namespace std;

int main()
{
	GlobalData::read();

	cout << GlobalData::g_nodePool._nameCount() << endl;
	cout << GlobalData::g_nodePool._suffCount() << endl;

	GlobalData::updata();

	GlobalData::save();

	system("pause");

	return 0;
}