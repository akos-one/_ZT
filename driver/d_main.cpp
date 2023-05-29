#pragma once
#include "d_core.h"
#include "d_structs.h"
#include "d_krnlapi.h"
#include "d_memory.h"


PVOID GameBase = 0;
PVOID(__fastcall* NtCompareSigningLevelsOrig)(PVOID, PVOID);

PVOID NtCompareSigningLevelsHook(Communication* arg0, PBYTE arg1) {
	if (ExGetPreviousMode() != UserMode) {
		return NtCompareSigningLevelsOrig(arg0, arg1);
	}

	Communication comms = {};
	if (!ReadVirtualMemory(&comms, arg0, sizeof(Communication)) || comms.Reason != COMMUNICATION_KEY) {
		return NtCompareSigningLevelsOrig(arg0, arg1);
	}

	auto args = (Communication*)arg0;

	switch (comms.Request) {
	case Request::GETBASE: {
		LOG_DEBUG("Called GetBase");

		if (comms.processID) {
			LOG_DEBUG("ProcessID in GetBase: %u", args->processID);

			PEPROCESS process = { 0 };
			PsLookupProcessByProcessId((HANDLE)args->processID, &process);

			args->Outbase = PsGetProcessSectionBaseAddress(process);

			LOG_DEBUG("BaseAddress in GetBase: 0x%x", args->Outbase);

			GameBase = args->Outbase;
			ObDereferenceObject(process);
		}
		break;
	}

	case Request::READPROCESSMEMORY: {
		if (comms.processID && comms.Buffer && comms.size > 0) {
			PEPROCESS process = nullptr;
			NTSTATUS status = PsLookupProcessByProcessId((HANDLE)comms.processID, &process);
			if (NT_SUCCESS(status)) {
				PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, comms.size, MEM_TAG);
				if (buffer != nullptr) {
					SIZE_T bytesRead = 0;
					status = MmCopyVirtualMemory(process, comms.Buffer, PsGetCurrentProcess(), buffer, comms.size, KernelMode, &bytesRead);
					if (NT_SUCCESS(status)) {
						// ReadVirtualMemory succeeded, update the size
						KdPrint(("Read memory: "));
						for (SIZE_T i = 0; i < bytesRead; i++) {
							KdPrint(("%02X ", ((PUCHAR)buffer)[i]));
							args->result = buffer;
						}
						KdPrint(("\n"));
						RtlCopyMemory(comms.Buffer, buffer, bytesRead);
					}
					ExFreePoolWithTag(buffer, MEM_TAG);
				}
				ObDereferenceObject(process);
			}
		}
		break;
	}

	case Request::WRITEPROCESSMEMORY: {
		if (comms.processID && comms.Buffer && comms.size > 0) {
			PEPROCESS process = nullptr;
			NTSTATUS status = PsLookupProcessByProcessId((HANDLE)comms.processID, &process);
			if (NT_SUCCESS(status)) {
				PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, comms.size, MEM_TAG);
				if (buffer != nullptr) {
					// Copy the memory from the user-mode buffer to the kernel buffer
					status = MmCopyVirtualMemory(PsGetCurrentProcess(), comms.Buffer, process, buffer, comms.size, KernelMode, nullptr);
					if (NT_SUCCESS(status)) {
						// WriteVirtualMemory succeeded
						// Perform any necessary operations on the buffer

						// For example, print the written memory
						KdPrint(("Written memory: "));
						for (SIZE_T i = 0; i < comms.size; i++) {
							KdPrint(("%02X ", ((PUCHAR)buffer)[i]));
							args->result = buffer;
						}
						KdPrint(("\n"));
					}
					ExFreePoolWithTag(buffer, MEM_TAG);
				}
				ObDereferenceObject(process);
			}
		}
		break;
	}

	case Request::UNLOAD: {
		LOG_DEBUG("Driver unloading...");
		return NULL;
		break;
	}



	}

	return NULL;
}


VOID UnloadRoutine(PDRIVER_OBJECT driver) {
	// Perform cleanup operations here
	// Release resources, close handles, etc.

	LOG_DEBUG("Driver unloading...");

	// Delete the symbolic link (optional)
	UNICODE_STRING symbolicLinkName;
	RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\MyDriver");
	IoDeleteSymbolicLink(&symbolicLinkName);

	// Delete the device object
	if (driver->DeviceObject) {
		IoDeleteDevice(driver->DeviceObject);
	}

	// Set the DriverUnload field to NULL
	driver->DriverUnload = NULL;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING regPath) {

	driver->DriverUnload = UnloadRoutine;

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	RTL_OSVERSIONINFOW info = { 0 };

	if (!info.dwBuildNumber) {
		RtlGetVersion(&info);
	}

	auto base = GetModuleBase(0);

	if (!base) {
		return status;
	}

	LOG_DEBUG("ntoskrnl.exe: 0x%x \n", base);

	auto addr = FindPattern(base,
		"\x4C\x8B\x05\x00\x00\x00\x00\x33\xC0\x4D\x85\xC0\x74\x08\x49\x8B\xC0\xE8\x00\x00\x00\x00\xF7\xD8",
		"xxx????xxxxxxxxxxx????xx");

	if (!addr) {
		LOG_DEBUG("Unable to find signature! \n");
		return status;
	}

	addr = RVA(addr, 7);
	LOG_DEBUG("Address: 0x%x \n", addr);

	*(PVOID*)&NtCompareSigningLevelsOrig =
		InterlockedExchangePointer(
			(volatile PVOID*)addr,
			NtCompareSigningLevelsHook
		);

	LOG_DEBUG("Swapped pointer: 0x%x to 0x%x \n", addr, &NtCompareSigningLevelsHook);

	status = STATUS_SUCCESS;
	return status;
}
