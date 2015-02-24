#include <Windows.h>
#include <stdio.h>
#include <malloc.h>
#include "tool.h"



void List_All_Dir(void)
{
	char file[MAX_FILE_LENGHT] = { 0 };
	DWORD ReadSize = 0;
	int Num = 0;

	HANDLE hFile = CreateFile(TEXT("config"),
		GENERIC_READ ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile == NULL)
	{
		printf("no config file \n");
		exit(-1);
	}

	DWORD FileSize = GetFileSize(hFile, NULL);
	ReadFile(hFile, file, FileSize,&ReadSize,NULL);
	for (int i = 0; i < FileSize; i++)
	{
		if (file[i] == '[')
		{
			Num++;
		}
	}
	PFILE buf = (PFILE)malloc(sizeof(_FILE_)*Num);
	memset(buf, 0, sizeof(_FILE_)*Num);
	Num = 0;
	for (int i = 0, posation = -1,iskey = 0; i < FileSize; i++, posation++)
	{
		if (file[i] == ']')
		{
			Num++;
			posation = -2;
			iskey = 0;
		}
		if (file[i] == '|')
		{
			iskey = 1;
			posation = -1;
		}
			
		if (file[i] != '[' && file[i] != ']' && file[i] != '|')
		{
			if (iskey == 0)
			{
				buf[Num].path[posation] = file[i];
			}
			else
			{
				buf[Num].key[posation] = file[i];
			}
		}		

	}

	for (int i = 0; i < Num; i++)
	{
		printf("%s-----%s\n", buf[i].path, buf[i].key);
	}
	free(buf);
	CloseHandle(hFile);
}