#pragma once


enum OTHER
{
	JO,
	QU,
	MO,
	ID
};


class Networking
{
public:
	SOCKET client_socket;
	bool is_connected = false;

	int client_id;
	OTHER other;

	float px = 0.f;
	float py = 384.f;

	int other_coin=0;
public:
	int Init();
	void Run();
	void Exit();

	void sendEnter();
	void sendExit();
	void sendJump();
	void sendPos(int coin);

	CObject* CreatePlayer();
private:
	void recv_thread(SOCKET client_socket);
};
