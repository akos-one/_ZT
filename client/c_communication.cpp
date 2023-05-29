#include "c_communication.h"


using namespace std;




NTSTATUS(*NtCompareSigningLevels)(PVOID, PVOID) = nullptr;



BOOL Setup(LPCSTR routineName) {
	auto ntdll = LoadLibraryA("ntdll.dll");
	if (!ntdll) {
		cout << "[MRN] failed to load ntdll!\n";
		return FALSE;
	}

	auto addr = GetProcAddress(ntdll, routineName);
	if (!addr) {
		cout << "[MRN] failed to find routine address!\n";
		return FALSE;
	}

	*(PVOID*)&NtCompareSigningLevels = addr;
	return TRUE;
}

PVOID GetBaseAddress(DWORD processID) {
	Communication request = {};
	SecureZeroMemory(&request, sizeof(Communication));

	request.Request = Request::GETBASE;
	request.Reason = 0x4D524E;
	request.processID = processID;
	request.Outbase = 0;

	NtCompareSigningLevels(&request, 0);
	return request.Outbase;
}










DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

DWORD GetProcessID(LPCWSTR processName) {
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	DWORD procID = NULL;

	if (handle == INVALID_HANDLE_VALUE)
		return procID;

	PROCESSENTRY32W entry = { 0 };
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(handle, &entry)) {
		if (!_wcsicmp(processName, entry.szExeFile)) {
			procID = entry.th32ProcessID;
		}
		else while (Process32NextW(handle, &entry)) {
			if (!_wcsicmp(processName, entry.szExeFile)) {
				procID = entry.th32ProcessID;
			}
		}
	}

	CloseHandle(handle);
	return procID;
}


namespace driver {
	PVOID RVM(DWORD processID, PVOID imageBase, PVOID address) {
		Communication request = {};
		SecureZeroMemory(&request, sizeof(Communication));

		request.Request = Request::READPROCESSMEMORY;
		request.Reason = 0x4D524E;
		request.processID = processID;
		request.Outbase = imageBase;
		request.Address = address;


		NtCompareSigningLevels(&request, 0);
		return request.result;
	}

	PVOID WVM(DWORD processID, PVOID imageBase, PVOID address) {
		Communication request = {};
		SecureZeroMemory(&request, sizeof(Communication));

		request.Request = Request::WRITEPROCESSMEMORY;
		request.Reason = 0x4D524E;
		request.processID = processID;
		request.Outbase = imageBase;
		request.Address = address;


		NtCompareSigningLevels(&request, 0);
		return request.result;
	}

	PVOID UNLOAD()
	{
		Communication request = {};
		SecureZeroMemory(&request, sizeof(Communication));

		request.Request = Request::UNLOAD;
		request.Reason = 0x4D524E;
		request.processID = NULL;
		request.Outbase = NULL;
		request.Address = NULL;


		NtCompareSigningLevels(&request, 0);
		return request.result;
	}
}