// dllmain.cpp : 定义 DLL 应用程序的入口点。

#include <Windows.h>
#include <tchar.h>
  
#define DEF_NTDLL "ntdll.dll"
#define NTSTATUS DWORD
#define DEF_ZWQUERYSYSTEMINFORMATION "ZwQuerySystemInformation"
#define g_szProcName L"notepad.exe"

char pOrgBytes[20] = {0};
PBYTE g_pOrgBytes = (PBYTE)pOrgBytes;

typedef struct _UNICODE_STRING {
  USHORT  Length;     
  USHORT  MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING ,*PUNICODE_STRING;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;  
    ULONG NumberOfThreads;  
    LARGE_INTEGER SpareLi1;  
    LARGE_INTEGER SpareLi2;  
    LARGE_INTEGER SpareLi3;  
    LARGE_INTEGER CreateTime;  
    LARGE_INTEGER UserTime;  
    LARGE_INTEGER KernelTime;  
    UNICODE_STRING ProcessName;  
    KPRIORITY BasePriority;  
    HANDLE UniqueProcessId;  
    HANDLE InheritedFromUniqueProcessId;  
    ULONG HandleCount;  
    ULONG SessionId;  
    ULONG_PTR PageDirectoryBase;  
    SIZE_T PeakVirtualSize;  
    SIZE_T VirtualSize;  
    ULONG PageFaultCount;  
    SIZE_T PeakWorkingSetSize;  
    SIZE_T WorkingSetSize;  
    SIZE_T QuotaPeakPagedPoolUsage;  
    SIZE_T QuotaPagedPoolUsage;  
    SIZE_T QuotaPeakNonPagedPoolUsage;  
    SIZE_T QuotaNonPagedPoolUsage;  
    SIZE_T PagefileUsage;  
    SIZE_T PeakPagefileUsage;  
    SIZE_T PrivatePageCount;  
    LARGE_INTEGER ReadOperationCount;  
    LARGE_INTEGER WriteOperationCount;  
    LARGE_INTEGER OtherOperationCount;  
    LARGE_INTEGER ReadTransferCount;  
    LARGE_INTEGER WriteTransferCount;  
    LARGE_INTEGER OtherTransferCount; 
}STSTEM_PROCESS_INFORMATION,*PSYSTEM_PROCESS_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
  SystemBasicInformation,
  SystemProcessorInformation,             // obsolete...delete
  SystemPerformanceInformation,
  SystemTimeOfDayInformation,
  SystemPathInformation,
  SystemProcessInformation,
  SystemCallCountInformation,
  SystemDeviceInformation,
  SystemProcessorPerformanceInformation,
  SystemFlagsInformation,
  SystemCallTimeInformation,
  SystemModuleInformation,
  SystemLocksInformation,
  SystemStackTraceInformation,
  SystemPagedPoolInformation,
  SystemNonPagedPoolInformation,
  SystemHandleInformation,
  SystemObjectInformation,
  SystemPageFileInformation,
  SystemVdmInstemulInformation,
  SystemVdmBopInformation,
  SystemFileCacheInformation,
  SystemPoolTagInformation,
  SystemInterruptInformation,
  SystemDpcBehaviorInformation,
  SystemFullMemoryInformation,
  SystemLoadGdiDriverInformation,
  SystemUnloadGdiDriverInformation,
  SystemTimeAdjustmentInformation,
  SystemSummaryMemoryInformation,
  SystemMirrorMemoryInformation,
  SystemPerformanceTraceInformation,
  SystemObsolete0,
  SystemExceptionInformation,
  SystemCrashDumpStateInformation,
  SystemKernelDebuggerInformation,
  SystemContextSwitchInformation,
  SystemRegistryQuotaInformation,
  SystemExtendServiceTableInformation,
  SystemPrioritySeperation,
  SystemVerifierAddDriverInformation,
  SystemVerifierRemoveDriverInformation,
  SystemProcessorIdleInformation,
  SystemLegacyDriverInformation,
  SystemCurrentTimeZoneInformation,
  SystemLookasideInformation,
  SystemTimeSlipNotification,
  SystemSessionCreate,
  SystemSessionDetach,
  SystemSessionInformation,
  SystemRangeStartInformation,
  SystemVerifierInformation,
  SystemVerifierThunkExtend,
  SystemSessionProcessInformation,
  SystemLoadGdiDriverInSystemSpace,
  SystemNumaProcessorMap,
  SystemPrefetcherInformation,
  SystemExtendedProcessInformation,
  SystemRecommendedSharedDataAlignment,
  SystemComPlusPackage,
  SystemNumaAvailableMemory,
  SystemProcessorPowerInformation,
  SystemEmulationBasicInformation,
  SystemEmulationProcessorInformation,
  SystemExtendedHandleInformation,
  SystemLostDelayedWriteInformation,
  SystemBigPoolInformation,
  SystemSessionPoolTagInformation,
  SystemSessionMappedViewInformation,
  SystemHotpatchInformation,
  SystemObjectSecurityMode,
  SystemWatchdogTimerHandler,
  SystemWatchdogTimerInformation,
  SystemLogicalProcessorInformation,
  SystemWow64SharedInformation,
  SystemRegisterFirmwareTableInformationHandler,
  SystemFirmwareTableInformation,
  SystemModuleInformationEx,
  SystemVerifierTriageInformation,
  SystemSuperfetchInformation,
  SystemMemoryListInformation,
  SystemFileCacheInformationEx,
  MaxSystemInfoClass  // MaxSystemInfoClass should always be the last enum
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS (WINAPI * PFZWQUERYSYSTEMINFORMATION)(SYSTEM_INFORMATION_CLASS SystemInformationClass,
														PVOID SystemInformation,
														ULONG SystemInformationLength,
														PULONG ReturnLength);


DWORD WINAPI NewZwQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass,
											PVOID SystemInformation,
											ULONG SystemInformationLength,
											PULONG ReturnLength);


BOOL hook_by_code(LPCSTR szDllName,LPCSTR szFuncName,PROC pfnNew,PBYTE pOrgBytes)
{
	FARPROC pfnOrg;
	DWORD dwOldProtect,dwAddress;
	BYTE pBuf[5] = {0xE9};
	PBYTE pByte;

	pfnOrg = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName),szFuncName);
	pByte = (PBYTE)pfnOrg;

	if(pByte[0] == 0xE9)
		return FALSE;
	
	VirtualProtect((LPVOID)pfnOrg,5,PAGE_EXECUTE_READWRITE,&dwOldProtect);

	memcpy(pOrgBytes,pfnOrg,5);

	dwAddress = (DWORD)pfnNew - (DWORD)pfnOrg - 5;
	memcpy(&pBuf[1],&dwAddress,4);

	memcpy(pfnOrg,pBuf,5);

	VirtualProtect((LPVOID)pfnOrg,5,dwOldProtect,&dwOldProtect);
	return TRUE;
}

BOOL unhook_by_code(LPCSTR szDllName,LPCSTR szFuncName,PBYTE pOrgBytes)
{
	FARPROC pFunc;
	DWORD dwOldProtect;
	PBYTE pByte;

	pFunc = GetProcAddress(GetModuleHandleA(szDllName),szFuncName);
	pByte = (PBYTE)pFunc;

	if(pByte[0] != 0xE9)
		return FALSE;
	VirtualProtect((LPVOID)pFunc,5,PAGE_EXECUTE_READWRITE,&dwOldProtect);
	memcpy(pFunc,pOrgBytes,5);

	VirtualProtect((LPVOID)pFunc,5,dwOldProtect,&dwOldProtect);

	return TRUE;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{

	char szCurProc[MAX_PATH] = {0};
	char * p = NULL;

	GetModuleFileNameA(NULL,szCurProc,MAX_PATH);
	p = strrchr(szCurProc,'\\');
	//if((p != NULL) && _stricmp(p+1,"injectmain.exe") )
	//	return TRUE;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		MessageBox(0,0,0,0);
		hook_by_code(DEF_NTDLL,DEF_ZWQUERYSYSTEMINFORMATION,(PROC)NewZwQuerySystemInformation,g_pOrgBytes);



	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		unhook_by_code(DEF_NTDLL,DEF_ZWQUERYSYSTEMINFORMATION,g_pOrgBytes);
		break;
	}
	return TRUE;
}

DWORD WINAPI NewZwQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass,
											PVOID SystemInformation,
											ULONG SystemInformationLength,
											PULONG ReturnLength)
{
	NTSTATUS status;
	FARPROC pFunc;
	PSYSTEM_PROCESS_INFORMATION pCur = NULL,pPrev = NULL;
	char szProcName[MAX_PATH] = {0};

	unhook_by_code(DEF_NTDLL,DEF_ZWQUERYSYSTEMINFORMATION,g_pOrgBytes);

	pFunc = GetProcAddress(GetModuleHandle(TEXT(DEF_NTDLL)),"ZwQuerySystemInformation");
	status = ((PFZWQUERYSYSTEMINFORMATION)pFunc)(SystemInformationClass,SystemInformation,SystemInformationLength,ReturnLength);
	
	if(SystemInformationClass == SystemProcessInformation)
	{
		pCur = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;

		while(pCur)
		{
			if(pCur->ProcessName != NULL)
			{
				
				if(!_tcsicmp((char*)pCur->ProcessName,(char*)g_szProcName))
				{
						if(pPrev)
						{
							if(pCur->NextEntryOffset == 0)
								pPrev->NextEntryOffset = 0;
							else
								pPrev->NextEntryOffset += pCur->NextEntryOffset;
						}
						else  
                        {  
                            if(pCur->NextEntryOffset)  
                            {  
                               
                                SystemInformation = (UCHAR*)SystemInformation + pCur->NextEntryOffset;  
                            }  
                            else 
                                SystemInformation = NULL;  
                        }
				}

			}
			pPrev = pCur;  
            if(pCur->NextEntryOffset)  
            {  
                pCur = (SYSTEM_PROCESS_INFORMATION *)((BYTE*)pCur + pCur->NextEntryOffset);  
            }  
            else   
            {  
                pCur = NULL;  
            }  
                      
        }  
    }  
      


	hook_by_code(DEF_NTDLL,DEF_ZWQUERYSYSTEMINFORMATION,(PROC)NewZwQuerySystemInformation,g_pOrgBytes);

	return status;
}