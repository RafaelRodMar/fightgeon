#pragma once

#include <string>
#include <vector>
#include "state.h"

class GameState : public State
{
public:
	GameState();
	~GameState();

	virtual void update();
	virtual void render();
	virtual void handleEvents();
	virtual bool onEnter();
	virtual bool onExit();

	void populateLevel();
	void createPath(int columnIndex, int rowIndex);
	void createRooms(int roomCount);
	void calculateTextures();

	virtual std::string getStateID() const { return s_ID; }

	bool isFloor(Vector2D v) {
		if (level[(int)v.m_x][(int)v.m_y] == 19) return true;
		if (level[(int)v.m_x][(int)v.m_y] == 20) return true;
		return false;
	}

private:
	static const std::string s_ID;
};