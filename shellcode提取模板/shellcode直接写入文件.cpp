#include <windows.h>
#include <stdio.h>
#define STR_DEF_04(name,a1,a2,a3,a4) __asm _emit a1\
									__asm _emit a2\
									__asm _emit a3\
									 __asm _emit a4




typedef struct _FUNCTION_ADDRESS
{
	unsigned char LoadLibraryA[16];
	unsigned char GetProcAddress[16];
}FUNCTION_ADDRESS;



typedef struct _ADDRESS_TABLE
{
	FUNCTION_ADDRESS RoutinesFunction;

	unsigned char CRTDLL[16];
	unsigned char printfName[16];
	unsigned char string[5];
}ADDRESS_TABLE;





DWORD GetFuncAddress(DWORD base,char * FuncName);
DWORD GetKernel32Base();
unsigned long AddressTableStorage();

typedef int (*myprintf)(const char*,...);
typedef HMODULE (WINAPI *myLoadLibrary)(LPCTSTR);
typedef FARPROC (WINAPI *myGetProcAddress)(HMODULE,LPCTSTR);




int shellcode()
{
	ADDRESS_TABLE * AllSource = (ADDRESS_TABLE *)AddressTableStorage();

	DWORD k32base = GetKernel32Base();
	myLoadLibrary LoadLib = (myLoadLibrary)GetFuncAddress(k32base,(char*)AllSource->RoutinesFunction.LoadLibraryA);
	myGetProcAddress GetProcAddr = (myGetProcAddress)GetFuncAddress(k32base,(char*)AllSource->RoutinesFunction.GetProcAddress);

	HMODULE hDll = LoadLib(TEXT((char*)AllSource->CRTDLL));
	myprintf Print = (myprintf)GetProcAddr(hDll,(char*)AllSource->printfName);

	Print((char*)AllSource->string);




	return 0;

}

DWORD GetFuncAddress(DWORD base,char * FuncName)
{
	DWORD FunctionAddress;
	PIMAGE_DOS_HEADER pDosHead = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS pNtHead = (PIMAGE_NT_HEADERS)(base+pDosHead->e_lfanew);
	IMAGE_DATA_DIRECTORY IDD = pNtHead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	PIMAGE_EXPORT_DIRECTORY Export =  (PIMAGE_EXPORT_DIRECTORY)(base+IDD.VirtualAddress);
	DWORD * pNameTable = (DWORD*)(((DWORD)Export->AddressOfNames)+base);

	
	for(int i=0;i<Export->NumberOfNames;i++)
	{
		
		
		char * pName = (char *)(pNameTable[i]+base); 

	
		if(strcmp(pName,FuncName) == 0)
		{
	
			break;
		}
	}
	WORD * IndexTable = (WORD*)(Export->AddressOfNameOrdinals+base);
	WORD index = (IndexTable[i] + 1 - Export->Base);
	
	DWORD * FuncTable = (DWORD*)(((DWORD)Export->AddressOfFunctions)+base);
	FunctionAddress = (DWORD)(FuncTable[index]+base);
	

	return FunctionAddress;
}


DWORD GetKernel32Base()
{
	unsigned long Address;
	_asm
	{
		xor ebx,ebx
		mov ebx,fs:[0x30]
		mov ebx,[ebx+0x0c]
		mov ebx,[ebx+0x1C]
		mov ebx,[ebx]
		mov ebx,[ebx]
		mov ebx,[ebx+0x8]
		mov Address,ebx
	}
	

	return Address;
}

unsigned long AddressTableStorage()
{
	unsigned int tableAddress;
	_asm
	{
		call end

		STR_DEF_04(LoadLibraryA,'L','o','a','d')
		STR_DEF_04(LoadLibraryA,'L','i','b','r')
		STR_DEF_04(LoadLibraryA,'a','r','y','A')
		STR_DEF_04(LoadLibraryA,'\0','\0','\0','\0')

		STR_DEF_04(GetProcAddress,'G','e','t','P')
		STR_DEF_04(GetProcAddress,'r','o','c','A')
		STR_DEF_04(GetProcAddress,'d','d','r','e')
		STR_DEF_04(GetProcAddress,'s','s','\0','\0')

		STR_DEF_04(CRTDLL,'c','r','t','d')
		STR_DEF_04(CRTDLL,'l','l','.','d')
		STR_DEF_04(CRTDLL,'l','l','\0','\0')
		STR_DEF_04(CRTDLL,'\0','\0','\0','\0')

		STR_DEF_04(printfName,'p','r','i','n')
		STR_DEF_04(printfName,'t','f','\0','\0')
		STR_DEF_04(printfName,'\0','\0','\0','\0')
		STR_DEF_04(printfName,'\0','\0','\0','\0')


		STR_DEF_04(string,'o','k','!','\n')
		__asm _emit '\0'
	

		end:

		pop eax
		mov tableAddress,eax
	}
	return tableAddress;
}
void end(){printf("1");}

int main()
{
	FILE * fp;
	fp = fopen("E:\\shellcode.txt","wb");
	if(fp == NULL)
	{
		printf("error");
		return -1;
	}
	unsigned char * code = (unsigned char*)shellcode;
	int len = (int)end - (int)shellcode;
	printf("%p,%p",end,shellcode);

	for(int i=0;i<len;i++)
	{
		fprintf(fp,"\\x%.2x",code[i]);
		printf("1");
	}
	shellcode();

	return 0;
}
