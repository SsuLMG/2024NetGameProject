#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <queue>

#define SERVERPORT 9000
#define BUFSIZE    512

// ���� ����
SOCKET listen_sock;
// RecvQueue
std::queue<char*> RecvQueue;

// ���� �ʱ�ȭ
int Init()
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		std::cout << "���� ���� ����" << std::endl;
		return 1;
	}

	// ���� �ּ� ����
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	int retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		std::cout << "Bind ����" << std::endl;
		return 1;
	}

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		std::cout << "Lisen ����" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "���� �غ� ��" << std::endl;
		return 0;
	}
}

// Recv ������
DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen = sizeof(clientaddr);
	char buf[BUFSIZE];

	// Ŭ���̾�Ʈ ���� ���
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (1)
	{
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			std::cout << "Recv ����" << std::endl;
			break;
		}
		else if (retval == 0)
			break;

		RecvQueue.push(buf);
	}

	// ���� �ݱ�
	closesocket(client_sock);
	std::cout << "[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� : " << addr << " ��Ʈ ��ȣ : " << ntohs(clientaddr.sin_port) << std::endl;
	return 0;
}

int main()
{
	if (Init())
		return -1;

	while (1)
	{
		// ������ ��ſ� ����� ����
		SOCKET client_sock;
		struct sockaddr_in6 clientaddr;  // IPv6�� ����ü
		int addrlen;
		HANDLE hThread;

		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock != INVALID_SOCKET)
		{
			// ������ Ŭ���̾�Ʈ ���� ��� (IPv6)
			char addr[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, &clientaddr.sin6_addr, addr, sizeof(addr));  // IPv6 �ּ� ��� , ��Ʈ���
			std::cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << addr << ", ��Ʈ ��ȣ = " << ntohs(clientaddr.sin6_port) << std::endl;

			// �ش� Ŭ���̾�Ʈ���� Recv������ ����
			hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)client_sock, 0, NULL);
			if (hThread == NULL) { closesocket(client_sock); }
			else { CloseHandle(hThread); }
		}

		// ���� �����Ͱ� �ִٸ�
		if (RecvQueue.size() > 0)
		{
			
		}
	}

	// ���� �ݱ�
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}