#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tool.h"



int main(int argc, char * argv[])
{
	printf("%d,%s,%s,%s\n",argc,argv[0],argv[1],argv[2]);
	
	int Operation = -1;

	if (argc > 3)
	{
		printf("bad movement\n");
		exit(-1);
	}
	if (argc == 1)
	{
		printf("-a:add path and key\n-g:use key to open\n-l:list all the key and path\n");
		exit(0);
	}

	if (strcmp(argv[1], "-l") == 0)
	{
		Operation = LISTDIR;
	}
	else if (strcmp(argv[1], "-a") == 0)
	{
		Operation = ADDDIR;
	}
	else if (strcmp(argv[1], "-g") == 0)
	{
		Operation = OPENDIR;
		if (argc != 3)
		{
			printf("no key!\n");
			exit(-1);
		}
	}

	if (Operation == -1)
	{
		printf("something wrong!\n");
		exit(-1);		
	}

	switch (Operation)
	{
	case LISTDIR:
		List_All_Dir();
		break;

	case ADDDIR:
		Add_Dir();
		break;

	case OPENDIR:
		Open_Dir(argv[2]);
		break;

	}

	return 0;
}