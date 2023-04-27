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

void player::configure()
{
	m_textureID = Game::Instance()->getHeroType();
	type = m_textureID;
	settings(m_textureID + "_idle_down", Vector2D(50, 50), Vector2D(0, 0), 33, 33, 1, 0, 0, 0.0, 1);
	m_position += Vector2D(Game::Instance()->getTileWidth() / 2 - m_width / 2, Game::Instance()->getTileHeight() / 2 - m_height / 2);
	m_class = static_cast<PLAYER_CLASS>(static_cast<int>(Game::Instance()->getHeroNum()));

	// Set class-specific variables.
	switch (m_class)
	{
	case PLAYER_CLASS::WARRIOR:
		m_strength += std::rand() % 6 + 5;
		projectileID = "sword";
		break;
	case PLAYER_CLASS::MAGE:
		m_defense = std::rand() % 6 + 5;
		projectileID = "magic_ball";
		break;
	case PLAYER_CLASS::ARCHER:
		m_dexterity = std::rand() % 6 + 5;
		projectileID = "arrow";
		break;
	case PLAYER_CLASS::THIEF:
		m_stamina = std::rand() % 6 + 5;
		projectileID = "dagger";
		break;
	}

	//set the stats
	int m_statPoints = 50;
	float attackBias = rand() % 101;
	float defenseBias = rand() % 101;
	float strengthBias = rand() % 101;
	float dexterityBias = rand() % 101;
	float staminaBias = rand() % 101;

	float total = attackBias + defenseBias + strengthBias + dexterityBias + staminaBias;

	m_attack += m_statPoints * (attackBias / total);
	m_defense += m_statPoints * (defenseBias / total);
	m_strength += m_statPoints * (strengthBias / total);
	m_dexterity += m_statPoints * (dexterityBias / total);
	m_stamina += m_statPoints * (staminaBias / total);
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

	m_textureID = type; //archer, thief, warrior or mage
	if (newDirection == '0')
	{
		//if no key pressed
		if (m_direction == 'U') m_textureID += "_idle_up";
		if (m_direction == 'D') m_textureID += "_idle_down";
		if (m_direction == 'L') m_textureID += "_idle_left";
		if (m_direction == 'R') m_textureID += "_idle_right";
		m_width = 33; m_height = 33; m_numFrames = 1; m_currentRow = 0; m_currentFrame = 0;
	}
	else
	{
		if (newDirection == 'U') m_textureID += "_walk_up";
		if (newDirection == 'D') m_textureID += "_walk_down";
		if (newDirection == 'L') m_textureID += "_walk_left";
		if (newDirection == 'R') m_textureID += "_walk_right";
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

void Enemy::update()
{
	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Enemy::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

void Slime::update()
{
	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Slime::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

void Humanoid::update()
{
	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void Humanoid::draw()
{
	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}