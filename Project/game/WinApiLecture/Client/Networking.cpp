#pragma comment(lib, "ws2_32")

#include "Networking.h"
#include "global.h"
#include <ws2tcpip.h>
#include <thread>
#include "..\..\CookierunServer\CookierunServer\protocol.h"

#include "CObject.h"
#include "CPlayer.h"

using namespace std;
bool recving = true;

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
    std::thread([this]() { recv_thread(client_socket); }).detach();
}

void Networking::Exit()
{
    closesocket(client_socket);
    WSACleanup();
}

void Networking::sendEnter()
{
    CS_Packet p;
    p.size = sizeof(p);
    p.id = client_id;
    p.type = ENTER;
    send(client_socket, (char*)&p, sizeof(p), 0);
}

void Networking::sendExit()
{
    CS_Packet p;
    p.size = sizeof(p);
    p.id = client_id;
    p.type = EXIT;
    send(client_socket, (char*)&p, sizeof(p), 0);
}

void Networking::sendJump()
{
    CS_Packet p;
    p.size = sizeof(p);
    p.id = client_id;
    p.type = JUMP;
    send(client_socket, (char*)&p, sizeof(p), 0);
}

void Networking::sendPos(int coin)
{
    CS_Packet p;
    p.size = sizeof(p);
    p.id = client_id;
    p.type = CIDLE;
    p.x = px;
    p.y = py;
    p.coin = coin;
    send(client_socket, (char*)&p, sizeof(p), 0);
}

CObject* Networking::CreatePlayer()
{
    // CObject : CPlayer �߰�
    CObject* pObj = new CPlayer;
    pObj->SetName(L"Player");
    pObj->SetPos(Vec2(px, py));
    pObj->SetScale(Vec2(267.f + rand() % 10, 133.f));

    return pObj;
}

void Networking::recv_thread(SOCKET client_socket)
{
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
            return;
        }
        else if (ret >= sizeof(SC_Packet)) {
            SC_Packet* sp = reinterpret_cast<SC_Packet*>(recvbuf);
            switch (sp->type) {
            case JOIN:
                other = JO;
                break;
            case QUIT:
                other = QU;
                break;
            case MOVE:
                // ������ �ٽ� ������ �ϱ�
                break;
            case CIDLE:
                other_coin = sp->coin;
                other = ID;
                break;
            default:
                break;
            }
        }
        else {
            cout << "���� �����Ͱ� ����� ��Ŷ ũ��� �ٸ��ϴ�." << endl;
        }
    }
}



