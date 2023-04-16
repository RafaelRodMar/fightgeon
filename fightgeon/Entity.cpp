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
	char newDirection = '0';
	int tw = Game::Instance()->getTileWidth();
	int th = Game::Instance()->getTileHeight();

	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_W))
	{
		newDirection = 'U';
		m_velocity.m_y -= 3;
		//check collisions
		bool check = false;
		Vector2D v = Vector2D((m_position.m_y + m_velocity.m_y) / th, (m_position.m_x + m_velocity.m_x) / tw);
		if (!Game::Instance()->isFloor(v)) check = true; //relocate the player.
		v = Vector2D((m_position.m_y + m_velocity.m_y) / th, (m_position.m_x + m_width + m_velocity.m_x) / tw);
		if (!Game::Instance()->isFloor(v)) check = true; //relocate the player.

		if (check == true) m_velocity.m_y += 3;
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_S))
	{
		newDirection = 'D';
		m_velocity.m_y += 3;
		//check collisions
		bool check = false;
		Vector2D v = Vector2D((m_position.m_y + m_height + m_velocity.m_y) / th, (m_position.m_x + m_velocity.m_x) / tw);
		if (!Game::Instance()->isFloor(v)) check = true; //relocate the player.
		v = Vector2D((m_position.m_y + m_height + m_velocity.m_y) / th, (m_position.m_x + m_width + m_velocity.m_x) / tw);
		if (!Game::Instance()->isFloor(v)) check = true; //relocate the player.

		if (check == true) m_velocity.m_y -= 3;
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_A))
	{
		newDirection = 'L';
		m_velocity.m_x -= 3;
		//check collision to the left comparing the two corners up-down/left with the map
		bool check = false;
		Vector2D v = Vector2D((m_position.m_y + m_velocity.m_y) / th, (m_position.m_x + m_velocity.m_x) / tw);
		if (!Game::Instance()->isFloor(v)) check = true; //relocate the player.
		v = Vector2D((m_position.m_y + m_height + m_velocity.m_y) / th, (m_position.m_x + m_velocity.m_x) / tw);
		if (!Game::Instance()->isFloor(v)) check = true; //relocate the player.

		if (check == true) m_velocity.m_x += 3;
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_D))
	{
		newDirection = 'R';
		m_velocity.m_x += 3;
		//check collision
		bool check = false;
		Vector2D v = Vector2D((m_position.m_y + m_velocity.m_y) / th, (m_position.m_x + m_width + m_velocity.m_x) / tw);
		if (!Game::Instance()->isFloor(v)) check = true; //relocate the player.
		v = Vector2D((m_position.m_y + m_height + m_velocity.m_y) / th, (m_position.m_x + m_width + m_velocity.m_x) / tw);
		if (!Game::Instance()->isFloor(v)) check = true; //relocate the player.

		if (check == true) m_velocity.m_x -= 3;
	}

	if (newDirection == '0')
	{
		//if no key pressed
		if (m_direction == 'U') m_textureID = "warrior_idle_up";
		if (m_direction == 'D') m_textureID = "warrior_idle_down";
		if (m_direction == 'L') m_textureID = "warrior_idle_left";
		if (m_direction == 'R') m_textureID = "warrior_idle_right";
		m_width = 33; m_height = 33; m_numFrames = 1; m_currentRow = 0; m_currentFrame = 0;
	}
	else
	{
		if (newDirection == 'U') m_textureID = "warrior_walk_up";
		if (newDirection == 'D') m_textureID = "warrior_walk_down";
		if (newDirection == 'L') m_textureID = "warrior_walk_left";
		if (newDirection == 'R') m_textureID = "warrior_walk_right";
		m_width = 264/8; m_height = 33; m_numFrames = 8; m_currentRow = 0; m_currentFrame = 0;
		m_direction = newDirection;
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