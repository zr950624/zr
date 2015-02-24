#include <stdio.h>
#include <Windows.h>
#include "tool.h"

void Add_Dir()
{

	char path[80] = { 0 }, key[10] = { 0 }, store[100] = { 0 };
	DWORD len;

	printf("please input the path:");
	gets_s(path);
	printf("please input the key:");
	gets_s(key);

	if (strlen(key) > 9 || strlen(path) > 79)
	{
		printf("already overflow\n");
		exit(-1);
	}
	strcat_s(store, "[");
	strcat_s(store, path);
	strcat_s(store, "|");
	strcat_s(store, key);
	strcat_s(store, "]");
	printf("\n\n%s\n", store);
	
	HANDLE hFile = CreateFile(TEXT("config"),
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);

	if (hFile == NULL)
	{
		printf("CreateFile error (%d)\n", GetLastError());
		exit(-1);
	}

	SetFilePointer(hFile, 0, NULL, FILE_END);
	WriteFile(hFile, store, strlen(store), &len, NULL);
	CloseHandle(hFile);
	
}

