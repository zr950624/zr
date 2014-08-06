#include <windows.h>
#include <stdio.h>
#define MYHOOK extern "C" _declspec(dllexport)

#include "hook.h"

int _stdcall myMessageBoxA(HWND,unsigned short *,unsigned short *,unsigned int)
{
	
	printf("MessageBoxA Hook OK!\n");

	return 0;
}

int _stdcall myMessageBoxW(HWND,unsigned short *,unsigned short *,unsigned int)
{
	printf("MessageBoxW Hook OK!\n");
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInstance,DWORD fdwReason,PVOID fImpload)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:

		MessageBox(0,TEXT("1"),TEXT("1"),MB_OK);
		break;
	}

	return TRUE;
}

MYHOOK void IATHook(DWORD base)
{
	PIMAGE_DOS_HEADER pDosHead = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS pNtHead = (PIMAGE_NT_HEADERS)(base+pDosHead->e_lfanew);
	IMAGE_DATA_DIRECTORY Import = pNtHead->OptionalHeader.DataDirectory[1];
	int NumOfDll = Import.Size/sizeof(IMAGE_IMPORT_DESCRIPTOR);
	PIMAGE_IMPORT_DESCRIPTOR DataOfDll =  (PIMAGE_IMPORT_DESCRIPTOR)(base+Import.VirtualAddress);


	for(int i=0;i<NumOfDll-1;i++)
	{
		PCSTR  Dllname = (PCSTR)(base+DataOfDll[i].Name);
		if(strcmp(Dllname,"USER32.dll")==0)
		{
			//
			PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(base+DataOfDll[i].FirstThunk);
			PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)(base+DataOfDll[i].OriginalFirstThunk);
			int num=0;
			for(;;pINT++)
			{		
				if((*(int *)pINT)==0)
					break;
				else
					num++;
			}

			for(int n=0;;n++)
			{

				int* itd = (int*)(base+DataOfDll[i].OriginalFirstThunk+n*sizeof(IMAGE_THUNK_DATA));
				PIMAGE_IMPORT_BY_NAME pNameA = (PIMAGE_IMPORT_BY_NAME)(base+(*itd));
				PCSTR  p=(PCSTR)pNameA->Name;
				if(strcmp(p,"MessageBoxA")==0)
				{
		
					PIMAGE_THUNK_DATA pIAT1 = pIAT+n;
					pIAT1->u1.Function = (PDWORD)myMessageBoxA;
					 
					break;	 
				}
			}//for

			for(int n2=0;;n++)
			{

				int* itd = (int*)(base+DataOfDll[i].OriginalFirstThunk+n2*sizeof(IMAGE_THUNK_DATA));
				PIMAGE_IMPORT_BY_NAME pNameW = (PIMAGE_IMPORT_BY_NAME)(base+(*itd));
				PCSTR  p=(PCSTR)pNameW->Name;
				if(strcmp(p,"MessageBoxW")==0)
				{
		
					PIMAGE_THUNK_DATA pIAT1 = pIAT+n2;
					pIAT1->u1.Function = (PDWORD)myMessageBoxW;
					 
					break;
				}
			}//for




		}
	}//for
}
