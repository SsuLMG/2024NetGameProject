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

SOCKET listen_sock;
std::vector<SOCKET> client_sockets; // 접속된 모든 클라이언트의 소켓을 관리

// 서버 초기화 함수
int Init()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        cout << "소켓 생성 실패" << endl;
        return 1;
    }

    struct sockaddr_in serveraddr;
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
        cout << "서버 준비 완료" << endl;
        return 0;
    }
}

// 클라이언트로부터 위치 정보를 수신하는 스레드
DWORD WINAPI RecvThread(LPVOID arg) {
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    cout << "[TCP 서버] 클라이언트 접속 : IP 주소 = " << addr << ", 포트 번호 = " << ntohs(clientaddr.sin_port) << endl;

    Packet packet;
    while (true) {
        int retval = recv(client_sock, (char*)&packet, sizeof(packet), 0);
        if (retval == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            cout << "recv 실패. 에러 코드: " << error_code << endl;
            break;
        }
        else if (retval == 0) {
            // 연결 종료
            cout << "[TCP 서버] 클라이언트 연결 종료" << endl;
            break;
        }
        if (retval > 0) {
            cout << "수신 패킷 - 크기: " << packet.size << ", 타입: " << packet.type
                << ", x: " << packet.x << ", y: " << packet.y << endl;

            for (auto& sock : client_sockets) {
                if (sock != client_sock) { // 송신자에게는 전송하지 않음
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

    // 종료 시 모든 소켓 닫기
    for (SOCKET sock : client_sockets) {
        closesocket(sock);
    }
    WSACleanup();
    return 0;
}

