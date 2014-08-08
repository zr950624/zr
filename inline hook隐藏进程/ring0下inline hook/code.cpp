//实际上没啥区别，也是获取ZwQuerySystemInformation地址然后进行jmp
//不过ring3下是用GetProcAddress从ntdll.dll中导出函数地址，在ring0下换种方式：

//1.

NTKERNELAPI NTSTATUS ZwQuerySystemInformation(
        IN  ULONG SystemInformationClass,
        IN  OUT PVOID SystemInformation,
        IN  ULONG SystemInformationLength,
        OUT PULONG ReturnLength OPTIONAL
);//直接导出

UCHAR oldFunc[5] = {0};

InLineHook()
{
	UCHAR * pFunc = (UCHAR*)ZwQuerySystemInformation;
	strncpy(oldFunc,pFunc,5);
	pFunc[0] = xxxx;
	pFunc[1] = xxxx;
	pFunc[0] = xxxx;
	pFunc[1] = xxxx;
	pFunc[0] = xxxx;//inline hook修改的5个字节
	.....//剩下和ring3差不多了
}
	
//2.


typedef NTSTATUS (* pFunc)(
        IN  ULONG SystemInformationClass,
        IN  OUT PVOID SystemInformation,
        IN  ULONG SystemInformationLength,
        OUT PULONG ReturnLength OPTIONAL);

UCHAR oldFunc[5] = {0};
InLineHook()
{
	UNICODE_STRING FuncName;
	RtlInitUnicodeString(&FuncName,L"ZwQuerySystemInformation");
	MmGetSystemRoutineAddress(FuncName);
	strncpy(oldFunc,pFunc,5);
	pFunc[0] = xxxx;
	pFunc[1] = xxxx;
	pFunc[0] = xxxx;
	pFunc[1] = xxxx;
	pFunc[0] = xxxx;//inline hook修改的5个字节,jmp到我的函数
	.....//剩下和ring3差不多了
}


//inLine Hook的思想就是类似热补丁一样的做法去强制控制函数工作流程
//以上两个做法的区别在于1.是直接申明导出使用的，具体见宏NTKERNELAPI，所以此方法是从本驱动中的IAT中找到的地址
//而2.是用MmGetSystemRoutineAddress函数在ntoskrnl.exe的EAT中查找的（必须是系统导出函数才能调用）导出地址。
//实际上差别不大，反正都不能过主动防御。



