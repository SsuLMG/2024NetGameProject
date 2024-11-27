#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>

using namespace std;

const int MAP_SIZE = 12;
const char EMPTY = '.';

struct character
{
    int characterX, characterY;
    int id;
    char CharType;
};

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
void placeCharacter(char map[MAP_SIZE][MAP_SIZE], character ch) {
    srand(static_cast<unsigned int>(time(0)));
    ch.characterX = rand() % MAP_SIZE;
    ch.characterY = rand() % MAP_SIZE;
    map[ch.characterY][ch.characterX] = ch.CharType;
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
void updateCharacterPosition(char map[MAP_SIZE][MAP_SIZE], character ch, int oldX, int oldY) {
    setCursorPosition(oldX * 2, oldY);
    cout << EMPTY;  // ���� ĳ���� ��ġ�� ����

    setCursorPosition(ch.characterX * 2, ch.characterY);
    cout << ch.CharType;  // ���ο� ĳ���� ��ġ ���
}

// ĳ���� �̵� �Լ�
void moveCharacter(char map[MAP_SIZE][MAP_SIZE], character ch, char direction) {
    int oldX = ch.characterX;
    int oldY = ch.characterY;

    switch (direction) {
    case 'w': if (ch.characterY > 0) ch.characterY--; break;
    case 'a': if (ch.characterX > 0) ch.characterX--; break;
    case 's': if (ch.characterY < MAP_SIZE - 1) ch.characterY++; break;
    case 'd': if (ch.characterX < MAP_SIZE - 1) ch.characterX++; break;
    default: return;  // ��ȿ���� ���� �Է�
    }

    // ���� ��ġ�� ���ο� ��ġ�� ������Ʈ
    map[oldY][oldX] = EMPTY;
    map[ch.characterY][ch.characterX] = ch.CharType;
    updateCharacterPosition(map, ch, oldX, oldY);
}

int main() {
    char map[MAP_SIZE][MAP_SIZE];
    char input;
    
    character ch{ 0,0,0,'@' };

    initializeMap(map);
    placeCharacter(map, ch);
    printMap(map);

    while (true) {
        if (_kbhit()) {  // Ű �Է��� �ִ��� Ȯ��
            input = _getch();
            if (input == 'q') {
                break;
            }
            moveCharacter(map, ch, input);
        }
    }

    setCursorPosition(0, MAP_SIZE + 1);  // ���� ���� �޽��� ��ġ ����
    cout << "Game Over!\n";
    return 0;
}
