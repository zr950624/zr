#include <stdio.h>
#include <windows.h>
#include <winsock.h>


#pragma comment(lib,"ws2_32.lib")

DWORD WINAPI send_ser(void * sock);

int main()
{

	WSADATA a;
	WSAStartup(0x0201,&a);
	SOCKET sock = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in tcpaddr,accaddr;
	int port = 4444;

	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tcpaddr.sin_port = htons(port);

	bind(sock,(sockaddr*)&tcpaddr,sizeof(tcpaddr));
	listen(sock,100);
	int len = sizeof(sockaddr_in);	
	SOCKET sock_acc = accept(sock,(sockaddr*)&accaddr,&len);
	
	

	printf("recv from:%s\n",inet_ntoa(accaddr.sin_addr));
	CreateThread(NULL,0,send_ser,(PVOID)&sock_acc,0,NULL);
	int num_of_recv;
	while(1)
	{

		char buf[270] = {0};
		num_of_recv = recv(sock_acc,buf,270,0);
	
		if(num_of_recv != 0)
			printf("recv:%d,%c,%d\n",num_of_recv,buf,WSAGetLastError());

	}
	

	closesocket(sock_acc);
	closesocket(sock);

	return 0;
}

DWORD WINAPI send_ser(void * sock)
{
	
	char buf[270] = "cmd";

	SOCKET socket = *((SOCKET *)sock);
	int size = send(socket,buf,270,0);


	while(1)
	{
		printf("your command >");
		gets(buf);

		int size = send(socket,buf,270,0);
	

		if(size == SOCKET_ERROR)
		{
			printf("error(%d)\n",WSAGetLastError());

			return -1;
		}
	}
}






