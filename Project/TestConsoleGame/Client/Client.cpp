#pragma comment(lib, "ws2_32")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "..\..\TestConsoleGame\Server\Packet.h"
#include <windows.h>
using namespace std;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return -1;

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVERPORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    bool is_connected = true;
    if (connect(client_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
    {
        cout << "������ ������ϴ�." << endl;
        is_connected = false;
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }

    // ���� ������ ���� ��
    cout << "������ ã�ҽ��ϴ�!!" << endl;
    int addrlen = sizeof(server_addr);
    getpeername(client_socket, (struct sockaddr*)&server_addr, &addrlen);
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &server_addr.sin_addr, addr, sizeof(addr));
    cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << addr << ", ��Ʈ ��ȣ = " << ntohs(server_addr.sin_port) << endl;

    char client_map[MAP_SIZE][MAP_SIZE];
    for (int y = 0; y < MAP_SIZE; y++)
        for (int x = 0; x < MAP_SIZE; x++)
            client_map[x][y] = '.';

    char recvbuf[BUFSIZE];

    while (true)
    {    
        int ret = recv(client_socket, recvbuf, sizeof(recvbuf), 0);
        if (ret == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            cout << "recv ����. ���� �ڵ�: " << error_code << endl;
            break;
        }
        else if (ret == 0) {
            // ���� ����
            cout << "[TCP ����] Ŭ���̾�Ʈ ���� ����" << endl;
            break;
        }
        else
        {
           TestPacket* pp = reinterpret_cast<TestPacket*>(recvbuf);
           memcpy_s(client_map, sizeof(client_map), pp->map, sizeof(pp->map));
           break;
        }

        for (int y = 0; y < MAP_SIZE; y++)
        {
            for (int x = 0; x < MAP_SIZE; x++)
                cout << client_map[x][y] << " ";
            cout << endl;
        }
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
