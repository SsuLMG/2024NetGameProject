#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
using namespace std;

#define SERVERPORT 9000
#define BUFSIZE    512

const int MAP_SIZE = 12;
const char EMPTY = '.';
const char CHARACTER = '@';

int characterX, characterY;

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

// ĳ���͸� ������ ��ġ�� ��ġ
void placeCharacter(char map[MAP_SIZE][MAP_SIZE]) {
    srand(static_cast<unsigned int>(time(0)));
    characterX = rand() % MAP_SIZE;
    characterY = rand() % MAP_SIZE;
    map[characterY][characterX] = CHARACTER;
}

// ���� ó�� �� ���� �׸��� �Լ�
void printMap(char map[MAP_SIZE][MAP_SIZE]) {
    setCursorPosition(0, 0);  // ȭ���� �� ���� Ŀ�� �̵�
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            cout << map[i][j] << ' ';
        }
        cout << endl;
    }
}

// ĳ���� ��ġ ������Ʈ �Լ�
void updateCharacterPosition(char map[MAP_SIZE][MAP_SIZE], int oldX, int oldY) {
    setCursorPosition(oldX * 2, oldY);
    cout << EMPTY;  // ���� ĳ���� ��ġ�� ����

    setCursorPosition(characterX * 2, characterY);
    cout << CHARACTER;  // ���ο� ĳ���� ��ġ ���
}

// ĳ���� �̵� �Լ�
void moveCharacter(char map[MAP_SIZE][MAP_SIZE], char direction, SOCKET sock) {
    int oldX = characterX;
    int oldY = characterY;

    switch (direction) {
    case 'w': if (characterY > 0) characterY--; break;
    case 'a': if (characterX > 0) characterX--; break;
    case 's': if (characterY < MAP_SIZE - 1) characterY++; break;
    case 'd': if (characterX < MAP_SIZE - 1) characterX++; break;
    default: return;
    }

    // ���� ��ġ�� ���ο� ��ġ�� ������Ʈ
    map[oldY][oldX] = EMPTY;
    map[characterY][characterX] = CHARACTER;
    updateCharacterPosition(map, oldX, oldY);

    // ���ο� ��ġ�� ������ ���� (int �������� ����)
    int position[2] = { characterX, characterY };
    send(sock, (char*)position, sizeof(position), 0);
}

int main()
{
    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // ���� ����
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        cout << "���� ���� ����" << endl;
        return 1;
    }

    // connect()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);
    int retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        cout << "Connect ����, ������ ������ �� �����ϴ�" << endl;
        return 1;
    }

    cout << "������ ã�ҽ��ϴ�" << endl;

    char map[MAP_SIZE][MAP_SIZE];
    char input;

    initializeMap(map);
    placeCharacter(map);
    printMap(map);

    while (true) {
        if (_kbhit()) {
            char input = _getch();
            if (input == 'q') {
                break;
            }
            moveCharacter(map, input, sock); // sock�� ���ڷ� ����
        }
    }

    // ���� �ݱ� �� ���� ����
    closesocket(sock);
    WSACleanup();
    return 0;
}
