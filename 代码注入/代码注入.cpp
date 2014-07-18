#include <windows.h>
#include <stdio.h>
#include <string.h>

DWORD WINAPI ThreadProc(LPVOID lParam);
BOOL InjectCode(DWORD dwPID);


typedef struct _THREAD_PARAM
{
	FARPROC pFunc[2];
	char szBuf[4][128];
}THREAD_PARAM,* PTHREAD_PARAM;

typedef FARPROC (WINAPI * PFGETPROCADDRESS)(HMODULE hModule,LPCSTR lpProcName);
typedef HMODULE (WINAPI * PFLOADLIBRARYA)(LPCSTR lpLibFileName);
typedef int (WINAPI * PFMESSAGEBOXA)(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType);

int main()
{
	DWORD dwPID = 14492;//process's PID
	InjectCode(dwPID);
	printf("%d\n",(DWORD)InjectCode - (DWORD)ThreadProc);

	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lParam)
{
	PTHREAD_PARAM pParam = (PTHREAD_PARAM)lParam;
	HMODULE hMod = NULL;
	FARPROC pFunc = NULL;

	hMod = ((PFLOADLIBRARYA)pParam->pFunc[0])(pParam->szBuf[0]);//loadlibrary("user32.dll");
	pFunc = ((PFGETPROCADDRESS)pParam->pFunc[1])(hMod,pParam->szBuf[1]);//GetProcAddress(hUser32,"MessageBox");

	((PFMESSAGEBOXA)pFunc)(0,pParam->szBuf[2],pParam->szBuf[3],0);//MessageBoxA(0,str,str,0);

	return 0;
}

BOOL InjectCode(DWORD dwPID)
{
	HMODULE hMod = NULL;
	THREAD_PARAM param = {0};
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	LPVOID pRemoteBuf[2] = {0};
	DWORD dwSize = 0;

	hMod = GetModuleHandle("kernel32.dll");

	param.pFunc[0] = GetProcAddress(hMod,"LoadLibraryA");
	param.pFunc[1] = GetProcAddress(hMod,"GetProcAddress");
	strcpy(param.szBuf[0],"user32.dll");
	strcpy(param.szBuf[1],"MessageBoxA");
	strcpy(param.szBuf[2],"ok");
	strcpy(param.szBuf[3],"ok");

	hProcess = OpenProcess(PROCESS_ALL_ACCESS,
							FALSE,
							dwPID);

	if(hProcess == NULL)
	{
		printf("error in open process(%d)\n",GetLastError());
		return -1;
	}


	dwSize = sizeof(THREAD_PARAM);
	pRemoteBuf[0] = VirtualAllocEx(hProcess,
									NULL,
									dwSize,
									MEM_COMMIT,
									PAGE_READWRITE);
	if(pRemoteBuf[0] == NULL)
	{
		printf("alloc1 error(%d)\n",GetLastError());
		return -1;
	}
	if(!WriteProcessMemory(hProcess,
							pRemoteBuf[0],
							(LPVOID)&param,
							dwSize,
							NULL))
	{
		printf("error in write remote process1(%d)\n",GetLastError());
		return -1;
	}

	dwSize = (DWORD)InjectCode - (DWORD)ThreadProc;
	pRemoteBuf[1] = VirtualAllocEx(hProcess,
									NULL,
									dwSize,
									MEM_COMMIT,
									PAGE_EXECUTE_READWRITE);
	if(pRemoteBuf[0] == NULL)
	{
		printf("alloc2 error(%d)\n",GetLastError());
		return -1;
	}
	if(!WriteProcessMemory(hProcess,
							pRemoteBuf[1],
							(LPVOID)ThreadProc,
							dwSize,
							NULL))
	{
		printf("error in write remote process2(%d)\n",GetLastError());
		return -1;
	}

	hThread = CreateRemoteThread(hProcess,
									NULL,
									0,
									(LPTHREAD_START_ROUTINE)pRemoteBuf[1],
									pRemoteBuf[0],
									0,
									NULL);
	if(hThread == NULL)
	{
		printf("create remote process error(%d)\n",GetLastError());
		return -1;
	}

	printf("inject ok!\n");

	getchar();
	CloseHandle(hThread);
	CloseHandle(hProcess);


	return TRUE;
}