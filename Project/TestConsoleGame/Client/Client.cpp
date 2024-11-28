#pragma comment(lib, "ws2_32")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "..\..\TestConsoleGame\Server\Packet.h"
#include <windows.h>
#include <string>
#include <thread>
using namespace std;
bool recving = true;
char client_map[MAP_SIZE][MAP_SIZE];

DWORD WINAPI recv_thread(LPVOID arg)
{
    SOCKET client_socket = (SOCKET)arg;
    char recvbuf[BUFSIZE];

    while (recving) {
        int ret = recv(client_socket, recvbuf, sizeof(recvbuf), 0);
        if (ret == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            cout << "recv ����. ���� �ڵ�: " << error_code << endl;
            break;
        }
        else if (ret == 0) {
            cout << "[TCP ����] ���� ����" << endl;
            recving = false;
            closesocket(client_socket);
            return 0;
            break;
        }
        else if (ret >= sizeof(TestPacket)) {
            TestPacket* pp = reinterpret_cast<TestPacket*>(recvbuf);
            memcpy_s(client_map, sizeof(client_map), pp->map, sizeof(pp->map));

            // �� ���
            for (int y = 0; y < MAP_SIZE; y++) {
                for (int x = 0; x < MAP_SIZE; x++) {
                    cout << client_map[y][x] << " ";
                }
                cout << endl;
            }
        }
        else {
            cout << "���� �����Ͱ� ����� ��Ŷ ũ��� �ٸ��ϴ�." << endl;
        }

        Sleep(16);
    }
}

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

    // ���� ����
    if (client_socket != INVALID_SOCKET) {
        HANDLE hWorkerThread = CreateThread(NULL, 0, recv_thread, (LPVOID)client_socket, 0, NULL);
        if (hWorkerThread == NULL) {
            closesocket(client_socket);
        }
        else {
            CloseHandle(hWorkerThread);
        }
    }

    // �۽� ����
    while (is_connected)
    {
        string message;
        getline(cin, message);

        if (message == "/quit")
        {
            is_connected = false;
            break;
        }
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
