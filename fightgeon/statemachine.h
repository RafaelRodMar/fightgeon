#pragma once

#include "state.h"
#include <vector>

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void changeState(STATES st);

	void update();
	void render();
	void handleEvents();
	void clean();

	State* m_menuState;
	State* m_gameState;

	State* m_currentState;

};