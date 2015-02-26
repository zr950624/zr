#include <Windows.h>
#include "tool.h"

using namespace std;

void Open_Dir(char * key)
{
	char file[MAX_FILE_LENGHT] = { 0 };
	DWORD ReadSize = 0;

	HANDLE hFile = CreateFile(TEXT("config"),
		GENERIC_READ,
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
	ReadFile(hFile, file, FileSize, &ReadSize, NULL);

	string file_content(file);

	string regStr = "\\[([^\\|]*)\\|" + string(key) + "\\]";
	boost::regex reg(regStr);
	
	boost::smatch m;
	if (boost::regex_search(file_content, m, reg))
	{
		cout << m[1] << endl;
		//system(m[1].str);
	}
	else
	{
		cout << "no such key" << endl;
	}
	
	CloseHandle(hFile);
}