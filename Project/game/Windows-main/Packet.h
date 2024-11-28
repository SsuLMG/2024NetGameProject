 #pragma once
#define SERVERPORT 9000
#define BUFSIZE    5164
#define MAP_SIZE    10

#pragma once
enum PTYPE
{
    LOGIN, MOVE
};

typedef struct BAKCGROUND {
    int MAX_X, MAX_Y, kind;
}background;
typedef struct CHARACTER {
    int x, y, state, kind, diff;
    int speed, num_bubble, bubble_len;
}Character;
typedef struct BUBBLE {
    int x, y, len, time, on, pop;
}Bubble;
typedef struct OBJECT {
    int x, y, kind, random_block;
}Object;
typedef struct ITEM {
    int kind, on;
}Item;

typedef struct {
    Object object[13][15];
    Character players[2];  // �� �÷��̾� ������
    Item items[13][15];    // �� ���� ������ ���� (��: �ִ� 100��)
    Bubble bubbles[14]; // ���� ���� (��: �ִ� 20��)
} GameStatePacket;

struct Packet {
    char size;
    char type;
    GameStatePacket gsp;
};

#pragma pack(pop)