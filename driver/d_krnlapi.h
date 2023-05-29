#include "d_structs.h"

extern "C" NTSTATUS NTAPI
MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

NTKERNELAPI NTSTATUS
PsLookupProcessByProcessId(
	_In_ HANDLE ProcessId,
	_Outptr_ PEPROCESS* Process
);

extern "C" NTKERNELAPI PVOID
PsGetProcessSectionBaseAddress(
	__in PEPROCESS Process
);

extern "C" NTSTATUS
ZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	ULONG* ReturnLength
);
