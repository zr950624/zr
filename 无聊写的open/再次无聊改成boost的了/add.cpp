#include "tool.h"

using namespace std;

void Add_Dir()
{

	char file[MAX_FILE_LENGHT] = { 0 };
	DWORD len;
	string key, path;

	cout << "please input the path:" << endl;
	cin >> path;
	cout << "please input the key:" << endl;
	cin >> key;
	string store_str = '[' + path + '|' + key + ']';

	
	
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
		cout << "CreateFile error (" << GetLastError() << ")" << endl;
		exit(-1);
	}

	DWORD FileSize = GetFileSize(hFile, NULL);
	ReadFile(hFile, file, FileSize, &len, NULL);

	string file_content(file);
	string regStr = "\\|" + key + "\\]";
	cout << regStr << ' ' << file_content << endl;
	boost::regex reg(regStr);
	boost::smatch m;

	
	if (boost::regex_search(file_content, m, reg))
	{
		cout << "this key already exist" << endl;
		exit(-1);
	}


	SetFilePointer(hFile, 0, NULL, FILE_END);
	WriteFile(hFile, store_str.c_str(), store_str.size(), &len, NULL);
	CloseHandle(hFile);
	
}

