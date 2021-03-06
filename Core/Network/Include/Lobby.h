#pragma once
#include <string>
#include <map>
#include <list>
#include <utility>
#include "UserInterface.h"

class Lobby
{
public:
	Lobby();
	~Lobby();

	void Initialize(bool isHost);

	bool Go();
	void AddPlayer(int id, std::string name, bool isReady);
	void RemovePlayer(std::string name);
	void PlayerChangeName(int id, std::string nName);
	void PlayerSetName(int id, std::string name);
	void PlayerReady(int id);
	void PlayerUnReady(int id);
	void Start();

	void DrawList();

	bool isReady();
	void isReady(bool);
private:
	std::map<int, std::pair<std::string, bool>> m_players;
	int player_amount;
	bool isHost_;
	bool isReady_;
	bool started_;
	bool isDrawListCalled_;

	UserInterface UI;
};
