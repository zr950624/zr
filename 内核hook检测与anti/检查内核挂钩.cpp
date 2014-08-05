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

PMODULE_ARRAY GetModuleArray()
{
	ULONG nBytes;
	PMODULE_ARRAY modArray;
	NTSTATUS ntStatus;

	ZwQuerySystemInformation(SystemModuleInformation,
								&nBytes,
								0,
								&nBytes);//获取Mudole结构体的大小

	modArray = (PMODULE_ARRAY)ExAllocatePool(PagedPool,nBytes);
	if(modArray == NULL)
	{
		DbgPrint("ExAllocatePool error!\n");
		return NULL;
	}

	ntStatus = ZwQuerySystemInformation(SystemModuleInformation,
											modArray,
											nBytes,
											NULL);
	if(!NT_SUCCESS(ntStatus))
	{
		ExFreePool(modArray);
		return NULL;
	}

	return modArray;
}


PSYSTEM_MODULE_INFORMATION GetModuleInformation(CHAR * ImageName,
												PMODULE_ARRAY modArray)
{
	for(int i=0;i<modArray->nModules;i++)
	{
		if(strcmp(imageName,(modArray->element[i]).ImageName) == 0)
		{
			return &(modArray->element[i]);
		}
	}

	return NULL;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver,PUNICODE_STRING path)
{
	//填充IRP分发函数等初始化工作

	PSYSTEM_MODULE_INFORMATION modules;
	PMODULE_ARRAY moduleArray = GetModuleArray();
	if(moduleArray == NULL)
	{
		DbgPrint("GetModuleArray() error!\n");
		return STATUS_UNSUCCESSFUL;
	}

	modules = GetModuleInformation(NAME_NTOSKRNL,moduleArray);
	if(module != NULL)
	{
		//检查SSDT,MSR,IA32_SYSENTER_EIP在不在ntkrnlpa.exe模块内,不在则被挂钩
	}

	return STATUS_SUCCESSl
}