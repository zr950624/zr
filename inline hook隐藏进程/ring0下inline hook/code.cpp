//ʵ����ûɶ����Ҳ�ǻ�ȡZwQuerySystemInformation��ַȻ�����jmp
//����ring3������GetProcAddress��ntdll.dll�е���������ַ����ring0�»��ַ�ʽ��

//1.

NTKERNELAPI NTSTATUS ZwQuerySystemInformation(
        IN  ULONG SystemInformationClass,
        IN  OUT PVOID SystemInformation,
        IN  ULONG SystemInformationLength,
        OUT PULONG ReturnLength OPTIONAL
);//ֱ�ӵ���

UCHAR oldFunc[5] = {0};

InLineHook()
{
	UCHAR * pFunc = (UCHAR*)ZwQuerySystemInformation;
	strncpy(oldFunc,pFunc,5);
	pFunc[0] = xxxx;
	pFunc[1] = xxxx;
	pFunc[0] = xxxx;
	pFunc[1] = xxxx;
	pFunc[0] = xxxx;//inline hook�޸ĵ�5���ֽ�
	.....//ʣ�º�ring3�����
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
	pFunc[0] = xxxx;//inline hook�޸ĵ�5���ֽ�,jmp���ҵĺ���
	.....//ʣ�º�ring3�����
}


//inLine Hook��˼����������Ȳ���һ��������ȥǿ�ƿ��ƺ�����������
//����������������������1.��ֱ����������ʹ�õģ��������NTKERNELAPI�����Դ˷����Ǵӱ������е�IAT���ҵ��ĵ�ַ
//��2.����MmGetSystemRoutineAddress������ntoskrnl.exe��EAT�в��ҵģ�������ϵͳ�����������ܵ��ã�������ַ��
//ʵ���ϲ�𲻴󣬷��������ܹ�����������



