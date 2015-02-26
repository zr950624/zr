#include "tool.h"


using namespace std;

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
		cout << "no config file \n" << endl;
		exit(-1);
	}

	DWORD FileSize = GetFileSize(hFile, NULL);
	ReadFile(hFile, file, FileSize,&ReadSize,NULL);

	string file_content(file);

	string regStr("\\[([^\\|]*)\\|([^\\]]*)\\]");	
	boost::regex reg(regStr);

	boost::cregex_iterator iterBegin(file, file+FileSize, reg);
	boost::cregex_iterator itrEnd;

	for (boost::cregex_iterator itr = iterBegin; itr != itrEnd; itr++)
	{
		cout << (*itr)[1] << "---------------" << (*itr)[2] << endl;
	}
	CloseHandle(hFile);
}