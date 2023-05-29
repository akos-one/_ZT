#pragma once
#include "c_main.h"


enum Request {
	GETBASE = 0,
	READPROCESSMEMORY = 1,
	WRITEPROCESSMEMORY = 2,
	OPENHANDLE = 3,
	UNLOAD = 4,
};

struct Communication {

	Request Request;
	DWORD processID;
	DWORD Reason; // must be 0xDEADBEEF....
	PVOID Outbase; // output image base for process.



	/*
	* READ/WRITE PROCESS MEMORY.
	*/

	PVOID Buffer;
	PVOID Address;
	PVOID Value;
	PVOID result;
	size_t size;
};







BOOL Setup(LPCSTR routineName);
PVOID GetBaseAddress(DWORD processID);





namespace driver {
	PVOID RVM(DWORD processID, PVOID imageBase, PVOID address);
	PVOID WVM(DWORD processID, PVOID imageBase, PVOID address);
	PVOID UNLOAD();
}







uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

DWORD FindProcessId(const std::wstring& processName);

DWORD GetProcessID(LPCWSTR processName);
