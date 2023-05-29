#include "c_main.h"
#include "c_communication.h"



using namespace std;


void setColor(unsigned short color)
{
	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hcon, color);
}


int main()
{
	SetConsoleTitle(L"Merino Client v.1.2");
	setColor(6);


	if (!Setup("NtCompareSigningLevels"))
	{
		cout << "[MRN] failed to initialize communication!\n";
		return 0;
	}

	auto procID = GetProcessID(L"explorer.exe");
	if (!procID) {
		cout << "[MRN] process not found!\n";
		return 0;
	}

	PVOID base = GetBaseAddress(procID);
	if (!base) {
		cout << "[MRN] failed to get base address\n";
		cout << "[MRN] failed base: " << base << endl;
	}

	else
		cout << "[MRN] Image Base: 0x" << base << endl;


	system("PAUSE");

	cout << endl << "[MRN] Unload function called\n";
	cout << "[MRN] Unload response: " << driver::UNLOAD() << endl;

	
	system("PAUSE");
	return 0;
}