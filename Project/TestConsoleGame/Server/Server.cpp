#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <queue>
#include <mutex>
#include "protocol.h"

using namespace std;

#define SERVERPORT 9000
#define BUFSIZE    512
#define MAP_SIZE    10

SOCKET server_sock;
std::vector<SOCKET> Clients; // ���ӵ� ��� Ŭ���̾�Ʈ�� ������ ����

// ���� �ʱ�ȭ �Լ�
int Init()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        cout << "���� ���� ����" << endl;
        return 1;
    }

    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    int retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        cout << "Bind ����" << endl;
        return 1;
    }

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        cout << "Listen ����" << endl;
        return 1;
    }
    else {
        cout << "���� �غ� �Ϸ�" << endl;
        return 0;
    }
}

// Ŭ���̾�Ʈ�κ��� ��ġ ������ �����ϴ� ������
DWORD WINAPI RecvThread(LPVOID arg) {
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << addr << ", ��Ʈ ��ȣ = " << ntohs(clientaddr.sin_port) << endl;

    Packet packet;
    while (true) {
        int retval = recv(client_sock, (char*)&packet, sizeof(packet), 0);
        if (retval == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            cout << "recv ����. ���� �ڵ�: " << error_code << endl;
            break;
        }
        else if (retval == 0) {
            // ���� ����
            cout << "[TCP ����] Ŭ���̾�Ʈ ���� ����" << endl;
            break;
        }
        if (retval > 0) {
            cout << "���� ��Ŷ - ũ��: " << packet.size << ", Ÿ��: " << packet.type
                << ", x: " << packet.x << ", y: " << packet.y << endl;

            for (auto& sock : client_sockets) {
                if (sock != client_sock) { // �۽��ڿ��Դ� �������� ����
                    send(sock, (char*)&packet, sizeof(packet), 0);
                }
            }
        }
    }

    closesocket(client_sock);
    return 0;
}


int main() {
    if (Init())
        return -1;

    while (true) {
        SOCKET client_sock;
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);

        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock != INVALID_SOCKET) {
            client_sockets.push_back(client_sock);
            HANDLE hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)client_sock, 0, NULL);
            if (hThread == NULL) {
                closesocket(client_sock);
            }
            else {
                CloseHandle(hThread);
            }
        }
    }

    // ���� �� ��� ���� �ݱ�
    for (SOCKET sock : client_sockets) {
        closesocket(sock);
    }
    WSACleanup();
    return 0;
}

