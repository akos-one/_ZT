#pragma once
#include "d_krnlapi.h"

BOOL ReadVirtualMemory(PVOID dest, PVOID src, size_t size);
BOOL WriteVirtualMemory(PVOID dest, PVOID src, size_t size);
PVOID GetModuleBase(LPCSTR moduleName);


PIMAGE_NT_HEADERS getHeader(PVOID module);
PBYTE FindPattern(PVOID base, LPCSTR pattern, LPCSTR mask);