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

	PVOID baseAddress;
	driver::GetBaseAddress(procID, &baseAddress);

	if (!baseAddress) {
		cout << "[MRN] Failed to retrieve the base address." << endl;
		cout << "[MRN] failed base: " << baseAddress << endl;
	}


	else {
		setColor(2);
		cout << "[MRN] Image Base: 0x" << baseAddress << endl;
		setColor(6);
	}


	system("PAUSE");
	PVOID result = NULL;
	cout << endl << "[MRN] Read function called.\n";
	driver::RVM(procID, baseAddress, (PVOID)0x00EC, &result);
	cout << "[MRN] Readen: 0x" << result << endl;


	cout << endl << "[MRN] Unload function called.\n";
	cout << "[MRN] Unload response: " << driver::UNLOAD() << endl;

	
	system("PAUSE");
	return 0;
}