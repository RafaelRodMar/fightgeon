#include "Entity.h"
#include "game.h"

float DEGTORAD = 0.017453f; //pi/180

void Entity::handleEvents()
{

}

void Entity::update()
{
	if (m_name == "explosion")
		m_currentFrame++;
	else
		m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Entity::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height, 
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

//void  asteroid::update()
//{
//	m_position.m_x += m_velocity.m_x;
//	m_position.m_y += m_velocity.m_y;
//
//	if (m_position.m_x > Game::Instance()->getGameWidth()) m_position.m_x = 0;
//	if (m_position.m_x < 0) m_position.m_x = Game::Instance()->getGameWidth();
//	if (m_position.m_y > Game::Instance()->getGameHeight()) m_position.m_y = 0;
//	if (m_position.m_y < 0) m_position.m_y = Game::Instance()->getGameHeight();
//
//	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
//}

void bullet::update()
{
	m_velocity.m_x = cos(m_angle*DEGTORAD) * 6;
	m_velocity.m_y = sin(m_angle*DEGTORAD) * 6;
	// angle+=rand()%6-3;
	m_position.m_x += m_velocity.m_x;
	m_position.m_y += m_velocity.m_y;

	if (m_position.m_x > Game::Instance()->getGameWidth() || m_position.m_x<0 || m_position.m_y>Game::Instance()->getGameHeight() || m_position.m_y < 0) m_life = 0;

	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void bullet::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle+90, m_alpha, SDL_FLIP_NONE);
}

void player::update()
{
	m_position += m_velocity;

	m_velocity = Vector2D(0, 0);
	if (m_position.m_x + m_width >= Game::Instance()->getGameWidth()) m_position.m_x = Game::Instance()->getGameWidth() - m_width;
	if (m_position.m_x < 0) m_position.m_x = 0;
	if (m_position.m_y + m_height >= Game::Instance()->getGameHeight()) m_position.m_y = Game::Instance()->getGameHeight() - m_height;
	if (m_position.m_y < 0) m_position.m_y = 0;

	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void player::handleEvents()
{
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_W))
	{
		m_textureID = "warrior_idle_up";
		m_velocity.m_y -= 3;
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_S))
	{
		m_textureID = "warrior_idle_down";
		m_velocity.m_y += 3;
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_A))
	{
		m_textureID = "warrior_idle_left";
		m_velocity.m_x -= 3;
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_D))
	{
		m_textureID = "warrior_idle_right";
		m_velocity.m_x += 3;
	}
}

void player::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height, 
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

//void car::update()
//{
//	m_position.m_y += m_velocity.m_y;
//
//	if (m_position.m_y < 0)
//	{
//		m_position.m_y = Game::Instance()->getGameHeight();
//	}
//	else
//	{
//		if (m_position.m_y > Game::Instance()->getGameHeight())
//			m_position.m_y = 0;
//	}
//}

void Gem::update()
{
	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Gem::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

void Key::update()
{
	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Key::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

void Gold::update()
{
	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Gold::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

void Heart::update()
{
	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Heart::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

void Potion::update()
{
	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Potion::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}