#include "Entity.h"
#include "game.h"

float DEGTORAD = 0.017453f; //pi/180

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

void  asteroid::update()
{
	m_position.m_x += m_velocity.m_x;
	m_position.m_y += m_velocity.m_y;

	if (m_position.m_x > Game::Instance()->getGameWidth()) m_position.m_x = 0;
	if (m_position.m_x < 0) m_position.m_x = Game::Instance()->getGameWidth();
	if (m_position.m_y > Game::Instance()->getGameHeight()) m_position.m_y = 0;
	if (m_position.m_y < 0) m_position.m_y = Game::Instance()->getGameHeight();

	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

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
	if (m_position.m_x > Game::Instance()->getGameWidth()) m_position.m_x = Game::Instance()->getGameWidth();
	if (m_position.m_x < 0) m_position.m_x = 0;
	if (m_position.m_y > Game::Instance()->getGameHeight()) m_position.m_y = Game::Instance()->getGameHeight();
	if (m_position.m_y < 0) m_position.m_y = 0;
}

void player::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height, 
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);

	if (m_shield)
	{
		AssetsManager::Instance()->drawFrame("shield", m_position.m_x, m_position.m_y, 40, 40, 0, 0, 
			Game::Instance()->getRenderer(), 0, m_alpha / 2, SDL_FLIP_NONE);
	}
}

void car::update()
{
	m_position.m_y += m_velocity.m_y;

	if (m_position.m_y < 0)
	{
		m_position.m_y = Game::Instance()->getGameHeight();
	}
	else
	{
		if (m_position.m_y > Game::Instance()->getGameHeight())
			m_position.m_y = 0;
	}
}