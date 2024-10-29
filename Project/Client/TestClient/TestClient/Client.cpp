#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>

using namespace std;

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
void moveCharacter(char map[MAP_SIZE][MAP_SIZE], char direction) {
    int oldX = characterX;
    int oldY = characterY;

    switch (direction) {
    case 'w': if (characterY > 0) characterY--; break;
    case 'a': if (characterX > 0) characterX--; break;
    case 's': if (characterY < MAP_SIZE - 1) characterY++; break;
    case 'd': if (characterX < MAP_SIZE - 1) characterX++; break;
    default: return;  // ��ȿ���� ���� �Է�
    }

    // ���� ��ġ�� ���ο� ��ġ�� ������Ʈ
    map[oldY][oldX] = EMPTY;
    map[characterY][characterX] = CHARACTER;
    updateCharacterPosition(map, oldX, oldY);
}

int main() {
    char map[MAP_SIZE][MAP_SIZE];
    char input;

    initializeMap(map);
    placeCharacter(map);
    printMap(map);

    while (true) {
        if (_kbhit()) {  // Ű �Է��� �ִ��� Ȯ��
            input = _getch();
            if (input == 'q') {
                break;
            }
            moveCharacter(map, input);
        }
    }

    setCursorPosition(0, MAP_SIZE + 1);  // ���� ���� �޽��� ��ġ ����
    cout << "Game Over!\n";
    return 0;
}
