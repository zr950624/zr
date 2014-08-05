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
								&nBytes);//��ȡMudole�ṹ��Ĵ�С

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
	//���IRP�ַ������ȳ�ʼ������

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
		//���SSDT,MSR,IA32_SYSENTER_EIP�ڲ���ntkrnlpa.exeģ����,�����򱻹ҹ�
	}

	return STATUS_SUCCESSl
}