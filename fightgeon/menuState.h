#pragma once

#include <string>
#include <vector>
#include "state.h"

class MenuState : public State
{
public:
	MenuState();
	~MenuState();

	virtual void update();
	virtual void render();

	virtual bool onEnter();
	virtual bool onExit();

	virtual std::string getStateID() const { return s_ID; }

private:
	static const std::string s_ID;
};