#include "statemachine.h"

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::changeState(STATES newState)
{
	if (newState == MENU) m_currentState = m_menuState;
	if (newState == GAME) m_currentState = m_gameState;
}

void StateMachine::update() {
	m_currentState->update();
}

void StateMachine::render()
{
	m_currentState->render();
}

void StateMachine::handleEvents()
{
}

void StateMachine::clean() {
	m_currentState = nullptr;

	m_menuState->onExit();
	delete m_menuState;

	m_gameState->onExit();
	delete m_gameState;
}