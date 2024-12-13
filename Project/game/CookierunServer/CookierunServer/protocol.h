#define SERVERPORT 9000
#define BUFSIZE    512

enum CSTYPE
{
    ENTER,
    EXIT,
    JUMP,
    CIDLE
};

enum SCTYPE
{
    JOIN,
    QUIT,
    MOVE,
    SIDLE
};

#pragma once
#pragma pack(push, 1)

struct SC_Packet
{
    unsigned char size;
    int id;
    // 1�� �÷��̾�� 0���÷��̾� ��ġ�� ��
    float x, y;
    int coin;
    SCTYPE type;
};

struct CS_Packet
{
    unsigned char size;
    int id;
    float x, y;
    int coin;
    CSTYPE type;
};


#pragma pack(pop)