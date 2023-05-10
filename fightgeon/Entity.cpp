#include<random>
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

	int tw = Game::Instance()->getTileWidth();
	int th = Game::Instance()->getTileHeight();

	//collisions with walls
	if(!Game::Instance()->isFloor(Vector2D(m_position.m_y / th, m_position.m_x / tw))) m_life = 0;
	if (!Game::Instance()->isFloor(Vector2D((m_position.m_y + m_height) / th, (m_position.m_x + m_width) / tw))) m_life = 0;
	if (!Game::Instance()->isFloor(Vector2D(m_position.m_y / th, (m_position.m_x + m_width) / tw))) m_life = 0;
	if (!Game::Instance()->isFloor(Vector2D((m_position.m_y + m_height) / th, m_position.m_x / tw))) m_life = 0;

	m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
}

void bullet::draw()
{
	bool hflip = false;
	bool vflip = false;
	if (m_angle < 0 && m_angle > -90) {
		hflip = false; vflip = true;
	}
	if (m_angle > 0 && m_angle < 90) {
		hflip = false; vflip = false;
	}
	if (m_angle < 0 && m_angle < -90) {
		hflip = true; vflip = true;
	}
	if (m_angle > 0 && m_angle > 90) {
		hflip = true; vflip = false;
	}
	
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (hflip) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
	if (vflip) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

	AssetsManager::Instance()->drawFrame(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), 0.0, m_alpha, flip );
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

	// give player two traits, a boost to two properties.
	std::vector<int> v(PLAYER_TRAIT_COUNT);
	std::iota(std::begin(v), std::end(v), 0); //fill with 0,1,2,3,4...
	auto rng = std::default_random_engine{};
	std::shuffle(std::begin(v), std::end(v), rng);

	//now simply get the two first elements of the vector
	for (int i = 0; i < PLAYER_TRAIT_COUNT;i++) {
		switch (v[i]) {
		case static_cast<int>(PLAYER_TRAIT::ATTACK):
			m_attack += rand() % 6 + 5;
			break;
		case static_cast<int>(PLAYER_TRAIT::DEFENSE):
			m_defense += rand() % 6 + 5;
			break;
		case static_cast<int>(PLAYER_TRAIT::STRENGTH):
			m_strength += rand() % 6 + 5;
			break;
		case static_cast<int>(PLAYER_TRAIT::DEXTERITY):
			m_dexterity += rand() % 6 + 5;
			break;
		case static_cast<int>(PLAYER_TRAIT::STAMINA):
			m_stamina += rand() % 6 + 5;
			break;
		default:
			break;
		}
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

	//set health and mana
	m_health = m_maxHealth = 100;
	m_mana = m_maxMana = 100;
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
	if (InputHandler::Instance()->getMouseButtonState(LEFT))
	{
		Vector2D* mousePos = InputHandler::Instance()->getMousePosition();

		bullet* b = new bullet();
		int w = 0, h = 0;
		if (projectileID == "arrow") { w = 24; h = 24; }
		if (projectileID == "dagger") { w = 24; h = 15; }
		if (projectileID == "magic_ball") { w = 21; h = 21; }
		if (projectileID == "sword") { w = 24; h = 21; }

		//get the angle with the mouse in radians
		double dx = mousePos->m_x - m_position.m_x;
		double dy = mousePos->m_y - m_position.m_y;
		double radians = std::atan2(dy, dx);
		//convert the angle to degrees
		double degrees = radians * 180.0 / 3.14159265359;

		b->settings(projectileID, m_position, Vector2D(5, 5), w, h, 1, 0, 0, degrees, 1);
		Game::Instance()->addItem(b);
		AssetsManager::Instance()->playSound("player_shoot", 0);

		InputHandler::Instance()->reset(); //reset the buttons
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

void Slime::draw()
{
	SDL_SetTextureColorMod(AssetsManager::Instance()->getTexture(m_textureID), m_color.r, m_color.g, m_color.b);
	AssetsManager::Instance()->drawFrameSc(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height, m_scale, 
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
}

void Humanoid::draw()
{
	AssetsManager::Instance()->drawFrameSc(m_textureID, m_position.m_x, m_position.m_y, m_width, m_height, m_scale,
		m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);

	if (m_hasHelmet >= 0) {
		SDL_Color c;
		if (m_hasHelmet == 0) c = { 110, 55, 28, 255 }; //bronze
		if (m_hasHelmet == 1) c = { 209, 208, 201, 255 }; //silver
		if (m_hasHelmet == 2) c = { 229, 192, 21, 255 }; //gold
		SDL_SetTextureColorMod(AssetsManager::Instance()->getTexture("helmet_idle_front"), c.r, c.g, c.b);
		AssetsManager::Instance()->drawFrameSc("helmet_idle_front", m_position.m_x, m_position.m_y, m_width, m_height, m_scale,
			m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
	}

	if (m_hasTorso >= 0) {
		SDL_Color c;
		if (m_hasTorso == 0) c = { 110, 55, 28, 255 }; //bronze
		if (m_hasTorso == 1) c = { 209, 208, 201, 255 }; //silver
		if (m_hasTorso == 2) c = { 229, 192, 21, 255 }; //gold
		SDL_SetTextureColorMod(AssetsManager::Instance()->getTexture("torso_idle_front"), c.r, c.g, c.b);
		AssetsManager::Instance()->drawFrameSc("torso_idle_front", m_position.m_x, m_position.m_y, m_width, m_height, m_scale,
			m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
	}

	if (m_hasLegs >= 0) {
		SDL_Color c;
		if (m_hasLegs == 0) c = { 110, 55, 28, 255 }; //bronze
		if (m_hasLegs == 1) c = { 209, 208, 201, 255 }; //silver
		if (m_hasLegs == 2) c = { 229, 192, 21, 255 }; //gold
		SDL_SetTextureColorMod(AssetsManager::Instance()->getTexture("legs_idle_front"), c.r, c.g, c.b);
		AssetsManager::Instance()->drawFrameSc("legs_idle_front", m_position.m_x, m_position.m_y, m_width, m_height, m_scale,
			m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha, SDL_FLIP_NONE);
	}
}
