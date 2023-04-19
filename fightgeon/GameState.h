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

	virtual std::string getStateID() const { return s_ID; }

private:
	static const std::string s_ID;
};