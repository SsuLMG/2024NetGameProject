#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include <vector>
#include <thread>
#include <mutex>

#include "protocol.h"
using namespace std;

struct Client
{
    SOCKET socket;
    int id;
};
vector<Client> Clients; // 접속된 모든 클라이언트의 소켓을 관리
mutex mylock;
// 기존 접속중인 플레이어 위치
float px, py;

// 서버 소켓
SOCKET listen_sock;
int Newid() { return Clients.size(); }

// 서버 초기화 함수 (완료)
int InitServer()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        cout << "소켓 생성 실패" << endl;
        return 1;
    }

    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    int retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        cout << "Bind 실패" << endl;
        return 1;
    }

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        cout << "Listen 실패" << endl;
        return 1;
    }
    else {
        cout << "서버 준비 완료" << ", PORT : " << SERVERPORT << endl;
        return 0;
    }
}

// 클라이언트가 접속하면 생성되는 전용 스레드
void WorkerThread(SOCKET client_sock, int id)
{
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    cout << "[TCP 서버] 클라이언트 접속, IP 주소 : " << addr << ", 포트 번호 : " << ntohs(clientaddr.sin_port) << ", ID : " << id << endl;

    char buffer[BUFSIZE];
    while (true)
    {
        int retval = recv(client_sock, buffer, BUFSIZE, 0);
        if (retval <= 0) break;
        CS_Packet* cp = reinterpret_cast<CS_Packet*>(buffer);

        cout << "ID: " << cp->id << " TYPE: " << cp->type << " COIN: " << cp->coin << endl;

        // 다른 클라이언트에게 데이터 전송
        mylock.lock();
        for (const auto& client : Clients) {
            if (client.socket != client_sock) {
                SC_Packet sp;
                sp.id = cp->id;
                sp.size = sizeof(sp);
                switch (cp->type)
                {
                case ENTER:
                {    
                    sp.type = SIDLE;
                    // 두번째로 접속한 플레이어
                    if (id > 0)
                    {
                        sp.x = px;
                        sp.y = py;
                        sp.type = JOIN;
                    }
                    break;
                }
                case EXIT:
                {
                    sp.type = QUIT;
                    break;
                }
                case JUMP:
                {
                    sp.type = MOVE;
                    break;
                }
                case CIDLE:
                {
                    sp.type = SIDLE;
                    px = cp->x;
                    py = cp->y;
                    sp.coin = cp->coin;
                    break;
                }
                default:
                    break;
                }
                send(client.socket, (char*)&sp, retval, 0);
            }
        }
        mylock.unlock();
    }

    // 클라이언트 목록에서 소켓 제거
    mylock.lock();
    Clients.erase(std::remove_if(Clients.begin(), Clients.end(),
        [client_sock](const Client& client) { return client.socket == client_sock; }), Clients.end());
    mylock.unlock();
}

int main()
{
    // 서버 초기화
    if (InitServer())
        return -1;

    // 서버 메인 루프
    while (true) {
        SOCKET clientSocket = accept(listen_sock, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) continue;

        int id = Newid();
        mylock.lock();
        Clients.push_back({ clientSocket, id });
        mylock.unlock();

        // 클라로 id 보내기
        send(clientSocket, (char*)&id, sizeof(id), 0);
        cout << "ID: " << id << "player is connect!!" << endl;

        // 클라로 인원수 보내기
        send(clientSocket, (char*)&id, sizeof(id), 0);

        thread(WorkerThread, clientSocket, id).detach();
    }

    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
