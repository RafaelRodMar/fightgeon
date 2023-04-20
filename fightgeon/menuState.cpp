#include "menuState.h"
#include "game.h"
const std::string MenuState::s_ID = "MENUSTATE";

MenuState::MenuState()
{
	button0 = { 0,0,100,100 };
	r0 = 0; g0 = 0; b0 = 255; a0 = 255;
	button1 = { 105,0, 100,100 };
	r1 = 0; g1 = 255; b1 = 0; a1 = 255;
}


MenuState::~MenuState()
{
}

void MenuState::update()
{
	Vector2D* mousePos = InputHandler::Instance()->getMousePosition();

	b0 = 255; g1 = 255;
	if (mousePos->m_x > button0.x && mousePos->m_x < button0.x + button0.w &&
		mousePos->m_y > button0.y && mousePos->m_y < button0.y + button0.h)
	{
		b0 = 180;
	}

	if (mousePos->m_x > button1.x && mousePos->m_x < button1.x + button1.w &&
		mousePos->m_y > button1.y && mousePos->m_y < button1.y + button1.h)
	{
		g1 = 180;
	}
}

void MenuState::render()
{
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(Game::Instance()->getRenderer(), &r, &g, &b, &a);

	SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r0, g0, b0, a0);
	SDL_RenderFillRect(Game::Instance()->getRenderer(), &button0);
	SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r1, g1, b1, a1);
	SDL_RenderFillRect(Game::Instance()->getRenderer(), &button1);

	SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r, g, b, a);
}

void MenuState::handleEvents()
{
}

bool MenuState::onEnter()
{
	return false;
}

bool MenuState::onExit()
{
	return false;
}
