#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include "..\..\TestConsoleGame\Server\protocol.h"
#include <vector>
using namespace std;

#define SERVERPORT 9000
#define BUFSIZE    512

const int MAP_SIZE = 12;
const char EMPTY = '.';
const char CHARACTER = '@';
const char OTHERCTER = '0';
char map[MAP_SIZE][MAP_SIZE];

struct character {
    char charactertype;
    int characterX, characterY;
    int oldX, oldY;
};

vector<character> characters;

// Ŀ�� ��ġ ���� �Լ�
void setCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hConsole, pos);
}

// ���� �ʱ�ȭ
void initializeMap(char map[MAP_SIZE][MAP_SIZE]) {
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            map[i][j] = EMPTY;
        }
    }
}

// ĳ���͸� ������ ��ġ�� ��ġ�ϰ� ���Ϳ� �߰�
void placeCharacter(char map[MAP_SIZE][MAP_SIZE], char type) {
    character newChar;
    newChar.charactertype = type;

    // ������ ��ġ�� ã�� ������ �ݺ�
    do {
        newChar.characterX = rand() % MAP_SIZE;
        newChar.characterY = rand() % MAP_SIZE;
    } while (map[newChar.characterY][newChar.characterX] != EMPTY);

    newChar.oldX = newChar.characterX;
    newChar.oldY = newChar.characterY;
    characters.push_back(newChar);

    map[newChar.characterY][newChar.characterX] = newChar.charactertype;
}

// ���� ó�� �� ���� �׸��� �Լ�
void printMap(char map[MAP_SIZE][MAP_SIZE]) {
    setCursorPosition(0, 0);

    // �� �ʱ�ȭ
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            map[i][j] = EMPTY;
        }
    }

    // �� ĳ������ ��ġ�� �ʿ� ǥ��
    for (const auto& ch : characters) {
        map[ch.characterY][ch.characterX] = ch.charactertype;
    }

    // �� ���
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            cout << map[i][j] << ' ';
        }
        cout << endl;
    }
}

// ĳ���� ��ġ ������Ʈ �Լ�
void updateCharacterPosition(character& ch) {
    setCursorPosition(ch.oldX * 2, ch.oldY);
    cout << EMPTY;

    setCursorPosition(ch.characterX * 2, ch.characterY);
    cout << ch.charactertype;

    ch.oldX = ch.characterX;
    ch.oldY = ch.characterY;
}

// ĳ���� �̵� �Լ�
void moveCharacter(character& ch, char direction, SOCKET sock) {
    ch.oldX = ch.characterX;
    ch.oldY = ch.characterY;

    // Ű �Է¿� ���� ĳ���� �̵�
    switch (direction) {
    case 'w': if (ch.characterY > 0) ch.characterY--; break;
    case 'a': if (ch.characterX > 0) ch.characterX--; break;
    case 's': if (ch.characterY < MAP_SIZE - 1) ch.characterY++; break;
    case 'd': if (ch.characterX < MAP_SIZE - 1) ch.characterX++; break;
    default: return;
    }

    move_Packet packet;
    packet.size = sizeof(move_Packet);
    packet.type = 1; // ��ġ ������Ʈ Ÿ��
    packet.x = ch.characterX;
    packet.y = ch.characterY;

    send(sock, (char*)&packet, sizeof(packet), 0); // ������ ��ġ ����

    updateCharacterPosition(ch);
}

// ���� ������ �Լ�
DWORD WINAPI ReceiveThread(LPVOID arg) {
    SOCKET sock = *(SOCKET*)arg;
    char packet[BUFSIZE];
    while (true) {
        int retval = recv(sock, (char*)&packet, sizeof(packet), 0);
        if (retval > 0) {
            if (packet[1] == 1) {  // ������ ������Ʈ
                // ���� ĳ���� ��ġ ������Ʈ ����
                // ...
            }
            else if (packet[1] == 0) {  // �α��� ��Ŷ ���� ��
                login_Packet* lp = reinterpret_cast<login_Packet*>(packet);

                character othercharacter;
                othercharacter.charactertype = OTHERCTER;
                othercharacter.characterX = lp->x;
                othercharacter.characterY = lp->y;
                othercharacter.oldX = lp->x;
                othercharacter.oldY = lp->y;

                characters.push_back(othercharacter);  // othercharacter �߰�

                printMap(map);  // ȭ�鿡 ���ο� ĳ���� ������ �ٽ� ���
            }
        }
        else if (retval == 0 || retval == SOCKET_ERROR) {
            cout << "���� ���� ����" << endl;
            break;
        }
    }
    return 0;
}


int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == INVALID_SOCKET) {
        cout << "���� ���� ����" << endl;
        return 1;
    }

    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);
    int retval = connect(client_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        cout << "Connect ����, ������ ������ �� �����ϴ�" << endl;
        return 1;
    }

    cout << "������ ã�ҽ��ϴ�" << endl;

    HANDLE hRecvThread = CreateThread(NULL, 0, ReceiveThread, (LPVOID)&client_sock, 0, NULL);
    if (hRecvThread == NULL) {
        closesocket(client_sock);
    }
    else {
        CloseHandle(hRecvThread);
    }

    initializeMap(map);
    placeCharacter(map, CHARACTER);  // ĳ���� ��ġ
    printMap(map);

    while (true) {
        if (_kbhit()) {
            char input = _getch();
            if (input == 'q') break;
            moveCharacter(characters[0], input, client_sock); // ù ��° ĳ���� �̵�
        }
    }

    closesocket(client_sock);
    WSACleanup();
    return 0;
}
