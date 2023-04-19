#pragma once

#include <string>
#include <vector>

enum STATES { MENU, CHOOSEHERO, GAME, ENDGAME };

class State    //abstract class
{
public:
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void handleEvents() = 0;
	virtual bool onEnter() = 0;
	virtual bool onExit() = 0;

	virtual STATES getStateID() const = 0;

protected:
};
