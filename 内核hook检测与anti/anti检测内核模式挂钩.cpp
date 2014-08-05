#define SystemModuleInformation 11
#define NAME_NTOSKRNL "\\SystemRoot\\system32\\ntkrnlpa.exe"

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Reserved[2];
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT MoudleNameOffset;
	CHAR ImageName[SIZE_FILENAME];
}SYSTEM_MUDOLE_INFORMATION,* PSYSTEM_MODULE_INFORMATION;


typedef struct _MODULE_ARRAY
{
	int nModules;
	SYSTEM_MUDOLE_INFORMATION element[];
}MODULE_ARRAY,* PMODULE_ARRAY;


NTSTATUS newZwQuerySystemInformation(ULONG SystemInformationClass,
									 PVOID SystemInformation,
									 ULONG SystemInformationLength,
									 PULONG ReturnLength)
{
	PMODULE_ARRAY mySystemInformation = (PMODULE_ARRAY)SystemInformation;
	

	if(SystemInformationClass == SystemModuleInformation)
	{

		ULONG nBytes;
		PMODULE_ARRAY modArray;
		NTSTATUS ntStatus;

		oldZwQuerySystemInformation(SystemModuleInformation,
									&nBytes,
									0,
									&nBytes);//获取Mudole结构体的大小

		modArray = (PMODULE_ARRAY)ExAllocatePool(PagedPool,nBytes);
		if(modArray == NULL)
		{
			DbgPrint("ExAllocatePool error!\n");
			return NULL;
		}

		ntStatus = oldZwQuerySystemInformation(SystemModuleInformation,
											modArray,
											nBytes,
											NULL);
		if(!NT_SUCCESS(ntStatus))
		{
			ExFreePool(modArray);
			return NULL;
		}
		for(int i=0,j=0;i<modArray->nModules;i++,j++)
		{
			if(strcmp(imageName,(modArray->element[i]).ImageName) == 0)
			{
				j--;
				continue;
			}
			(mySystemInformation->element[j]).Base = (modArray->element[i]).Base;
			//...依次填充
		}


		
	}

	ExFreePool(modArray);
	return ntStatus;
}