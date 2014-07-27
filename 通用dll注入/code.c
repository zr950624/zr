#include <windows.h>
#include <stdio.h>

BOOL IsVistaOrLater();
BOOL MyCreateRemoteThread(HANDLE hProcess,
						  LPTHREAD_START_ROUTINE pThreadProc,
						  LPVOID pRemoteBuf);

typedef DWORD (WINAPI * PFNTCREATETHREADEX)(PHANDLE ThreadHandle,
											ACCESS_MASK DesiredAccess,
											LPVOID ObjectAttributes,
											HANDLE ProcessHandle,
											LPTHREAD_START_ROUTINE lpStartAddress,
											LPVOID lpParameter,
											BOOL CreateSuspended,
											DWORD dwStackSize,
											DWORD dw1,
											DWORD dw2,
											LPVOID Unknown);




int main()
{
	char szDllName[MAX_PATH] = {0};
	DWORD dwPID;
	printf("please put in the Process's PID:");
	scanf("%d",&dwPID);
	printf("please put in the path of your dll:");
	scanf("%s",szDllName);
	getchar();

	HANDLE hProcess = NULL;
	LPVOID pRemoteBuf = NULL;
	FARPROC pThreadProc = NULL;
	DWORD dwBufSize = strlen(szDllName)+1;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS,
							FALSE,
							dwPID);
	if(hProcess == NULL)
	{
		printf("error in open process(%d)\n",GetLastError());
		return -1;
	}

	pRemoteBuf = VirtualAllocEx(hProcess,
								NULL,
								dwBufSize,
								MEM_COMMIT,
								PAGE_READWRITE);
	if(pRemoteBuf == NULL)
	{
		printf("cannot alloc memory(%d)\n",GetLastError());
		CloseHandle(hProcess);
		return -1;
	}
	else
	{
		WriteProcessMemory(hProcess,pRemoteBuf,(LPVOID)szDllName,dwBufSize,NULL);
	}
	int flag = 0;
	printf("you want load(1) or unload(2):");
	scanf("%d",&flag);

	if(flag == 1)
		pThreadProc = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),"LoadLibraryA");
	if(flag == 2)
		pThreadProc = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),"FreeLibrary");

	if(!MyCreateRemoteThread(hProcess,
							(LPTHREAD_START_ROUTINE)pThreadProc,
							pRemoteBuf))
	{
		printf("MyCreateRemoteThread error(%d)\n",GetLastError());
		CloseHandle(hProcess);
		return -1;
	}

	VirtualFreeEx(hProcess,pRemoteBuf,0,MEM_RELEASE);
	CloseHandle(hProcess);

	return 0;
}



BOOL IsVistaOrLater()
{
	OSVERSIONINFO osvi = {0};
	osvi.dwOSVersionInfoSize = sizeof(osvi);

	GetVersionEx(&osvi);

	if(osvi.dwMajorVersion == 6)
		return TRUE;

	return FALSE;
}//vista开始的内核版本号大于6



BOOL MyCreateRemoteThread(HANDLE hProcess,
						  LPTHREAD_START_ROUTINE pThreadProc,
						  LPVOID pRemoteBuf)
{
	HANDLE hThread = NULL;
	FARPROC pFunc = NULL;

	if(IsVistaOrLater())
	{
		pFunc = GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),"NtCreateThreadEx");

		if(pFunc == NULL)
		{
			printf("GetProcAddress:NtCreateThreadEx fail!(%d)\n",GetLastError());

			return FALSE;
		}

		((PFNTCREATETHREADEX)pFunc)(&hThread,
									0x1FFFFF,
									NULL,
									hProcess,
									pThreadProc,
									pRemoteBuf,
									FALSE,
									NULL,
									NULL,
									NULL,
									NULL);
		if(hThread == NULL)
		{
			printf("CreateRemoteThread error in NtCreateThreadEx(%d)\n",GetLastError());

			return FALSE;
		}
	}
	else
	{
		hThread = CreateRemoteThread(hProcess,
									NULL,
									0,
									pThreadProc,
									pRemoteBuf,
									0,
									NULL);
		if(hThread == NULL)
		{
			printf("CreateRemoteThread error in CreateRemoteThread(%d)\n",GetLastError());

			return FALSE;
		}
	}

	if(WAIT_FAILED == WaitForSingleObject(hThread,INFINITE))
	{
		printf("error in WaitForSingleObject(%d)\n",GetLastError());
		return FALSE;
	}

	return TRUE;

}




