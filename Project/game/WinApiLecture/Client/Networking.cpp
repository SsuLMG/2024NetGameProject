#pragma comment(lib, "ws2_32")

#include "Networking.h"
#include "global.h"
#include <ws2tcpip.h>
#include <thread>
#include "..\..\CookierunServer\CookierunServer\protocol.h"
using namespace std;

bool recving = true;

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
        else if (ret >= sizeof(SC_Packet)) {
            SC_Packet* pp = reinterpret_cast<SC_Packet*>(recvbuf);
           
        }
        else {
            cout << "���� �����Ͱ� ����� ��Ŷ ũ��� �ٸ��ϴ�." << endl;
        }

    }
}

int Networking::Init()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return -1;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVERPORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    is_connected = true;
    if (connect(client_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
    {
        cerr << "������ ������ϴ�." << endl;
        is_connected = false;
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }

    // ���� ������ ���� ��
    cerr << "������ ã�ҽ��ϴ�!!" << endl;
    int addrlen = sizeof(server_addr);
    getpeername(client_socket, (struct sockaddr*)&server_addr, &addrlen);
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &server_addr.sin_addr, addr, sizeof(addr));
    cerr << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << addr << ", ��Ʈ ��ȣ = " << ntohs(server_addr.sin_port) << endl;
    
    // ������ ���� id�� �޾Ƽ� ����
    int getid;
    recv(client_socket, (char*)&getid, sizeof(getid), 0);
    client_id = getid;
    return 0;
}

void Networking::Run()
{
    // ���� ����
    thread(recv_thread, client_socket).detach();

}

void Networking::Exit()
{
    closesocket(client_socket);
    WSACleanup();
}

void Networking::sendData(float x, float y)
{
    CS_Packet p;
    p.size = sizeof(p);
    p.id = client_id;
    p.x = x;
    p.y = y;

    send(client_socket, (char*)&p, sizeof(p), 0);
    cerr << "client send data!!" << endl;
    Sleep(100);
    return;
}
