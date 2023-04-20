#pragma once

#include "state.h"
#include <vector>

enum STATES { MENU, GAME };

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void changeState(STATES st);

	void update();
	void render();
	void clean();

	State* m_menuState;
	State* m_gameState;

	State* m_currentState;

};