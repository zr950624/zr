#ifdef _TOOL_

#else
#define _TOOL_

#include <boost\regex.hpp>
#include <iostream>
#include <string>
#include <Windows.h>

#define LISTDIR 1
#define ADDDIR 2
#define OPENDIR 3

#define MAX_FILE_LENGHT 65536

extern void List_All_Dir(void);
extern void Add_Dir(void);
extern void Open_Dir(char * key);


#endif