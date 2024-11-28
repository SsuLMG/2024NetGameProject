#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <queue>
#include <mutex>
#include "Packet.h"

using namespace std;
char map[MAP_SIZE][MAP_SIZE] = {};
struct character
{
    int x, y;
    char c;
};
vector<character> chars;
mutex mylock;

SOCKET listen_sock;
std::vector<SOCKET> Clients; // ���ӵ� ��� Ŭ���̾�Ʈ�� ������ ����

void UpdateMap(char map[MAP_SIZE][MAP_SIZE], vector<character>& chars)
{
    mylock.lock();
    for (auto& ch : chars)
        map[ch.x][ch.y] = ch.c;
    mylock.unlock();
}

// ĳ���͸� Ű�Է¿� ���� �̵���Ű�� �Լ� (�Ϸ�)
void moveCharacter(char map[MAP_SIZE][MAP_SIZE], vector<character>& chars, int id, char key) {
    if (id < 0 || id >= chars.size()) {
        cout << "�߸��� ID: " << id << endl;
        return;
    }

    mylock.lock();
    character& ch = chars[id];
    int oldx = ch.x;
    int oldy = ch.y;

    switch (key) {
    case 'w': ch.y -= 1; break;
    case 'a': ch.x -= 1; break;
    case 's': ch.y += 1; break;
    case 'd': ch.x += 1; break;
    default: break;
    }

    // ��� �ʰ� �˻�
    if (ch.x < 0 || ch.x >= MAP_SIZE || ch.y < 0 || ch.y >= MAP_SIZE) {
        ch.x = oldx;
        ch.y = oldy;
        mylock.unlock();
        cout << "�� ��� �ʰ�" << endl;
        return;
    }

    map[oldy][oldx] = '.'; // �̵� �� ��ǥ �ʱ�ȭ
    map[ch.y][ch.x] = ch.c; // �̵� �� ��ǥ�� ĳ���� �߰�
    mylock.unlock();
}

void sendMapToClients(char map[MAP_SIZE][MAP_SIZE]) {
    SC_TestPacket p;
    p.size = sizeof(p);
    memcpy_s(p.map, sizeof(p.map), map, sizeof(map));

    for (auto& sock : Clients) {
        int sentBytes = send(sock, (char*)&p, sizeof(p), 0);
        if (sentBytes == SOCKET_ERROR) {
            cout << "send ����: " << WSAGetLastError() << endl;
        }
    }
}


// ���� �ʱ�ȭ �Լ� (�Ϸ�)
int InitServer()
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

// Ŭ���̾�Ʈ�� �����ϸ� �����Ǵ� ���� ������
DWORD WINAPI WorkerThread(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << addr << ", ��Ʈ ��ȣ = " << ntohs(clientaddr.sin_port) << endl;

    srand(static_cast<unsigned int>(time(0)));
    int x = rand() % MAP_SIZE;
    int y = rand() % MAP_SIZE;
    character c = { x,y,'@' };
    chars.push_back(c);
    UpdateMap(map, chars);

    cout << "ĳ���� ����!! : " << " X : " << x << " Y :" << y << endl;
    for (int y = 0; y < MAP_SIZE; y++)
    {
        for (int x = 0; x < MAP_SIZE; x++)
            cout << map[x][y] << ' ';
        cout << endl;
    }

    char buf[BUFSIZE]{};

    while (true) {
        int retval = recv(client_sock, (char*)&buf, sizeof(buf), 0);

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
        else
        {
            CS_TestPacket* pp = reinterpret_cast<CS_TestPacket*>(buf);
           
            moveCharacter(map, chars, pp->id, pp->key);
            sendMapToClients(map);
            cout << "ĳ���� �̵�!!";
            break;
        }
    }
    closesocket(client_sock);
    return 0;
}

int main()
{
    // ���� �ʱ�ȭ
    if (InitServer())
        return -1;

    for (int y = 0; y < MAP_SIZE; y++)
        for (int x = 0; x < MAP_SIZE; x++)
            map[x][y] = '.';

    // ���� ���� ����
    while (true)
    {
        SOCKET client_sock;
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);

        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock != INVALID_SOCKET) {
            Clients.emplace_back(client_sock);
            HANDLE hWorkerThread = CreateThread(NULL, 0, WorkerThread, (LPVOID)client_sock, 0, NULL);
            if (hWorkerThread == NULL) {
                closesocket(client_sock);
            }
            else {
                CloseHandle(hWorkerThread);
            }
        }

        if (Clients.size() > 1)
        {
            UpdateMap(map, chars);

            SC_TestPacket p;
            p.size = sizeof(p);
            memcpy_s(p.map, sizeof(p.map), map, sizeof(map));

            // ������ ����
            for (auto& sock : Clients) {
                int sentBytes = send(sock, (char*)&p, sizeof(p), 0);
                if (sentBytes == SOCKET_ERROR) {
                    cout << "send ����: " << WSAGetLastError() << endl;
                }
                else {
                    cout << "send ����: " << sentBytes << " ����Ʈ ���۵�" << endl;
                }
            }
            Sleep(32);
        }
    }

    // ���� �� ��� ���� �ݱ�
    for (SOCKET sock : Clients)
    {
        closesocket(sock);
    }

    WSACleanup();
    return 0;
}

