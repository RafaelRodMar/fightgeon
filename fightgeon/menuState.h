#pragma once

#include <string>
#include <vector>
#include "state.h"
#include<SDL.h>

class MenuState : public State
{
public:
	MenuState();
	~MenuState();

	virtual void update();
	virtual void render();
	virtual void handleEvents();
	virtual bool onEnter();
	virtual bool onExit();

	virtual std::string getStateID() const { return s_ID; }

	SDL_Rect button0;
	Uint8 r0, g0, b0, a0;
	SDL_Rect button1;
	Uint8 r1, g1, b1, a1;

private:
	static const std::string s_ID;
};