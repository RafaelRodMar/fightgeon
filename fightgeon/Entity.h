#pragma once
#include <sdl.h>
#include <string>
#include "Vector2D.h"
#include "AssetsManager.h"

class Game;
enum class PLAYER_CLASS;
enum class POTION;

class Entity
{
public:
	//movement variables
	Vector2D m_position;
	Vector2D m_velocity;
	Vector2D m_acceleration;

	//size variables
	int m_width = 0, m_height = 0;

	//animation variables
	int m_currentRow = 0;
	int m_currentFrame = 0;
	int m_numFrames = 0;
	std::string m_textureID;

	//common boolean variables
	bool m_bUpdating = false;
	bool m_bDead = false;
	bool m_bDying = false;

	//rotation
	double m_angle = 0;

	//blending
	int m_alpha = 255;

	//game variables
	int m_radius;
	bool m_life;
	bool m_shield;
	int m_shieldTime;
	int m_waitTime = 0;
	std::string m_name;
	char m_direction = 'D';

	Entity()
	{
		m_life = true;
		m_shield = false;
	}

	void settings(const string &Texture, Vector2D pos, Vector2D vel, int Width, int Height, int nFrames, int row, int cframe, double Angle = 0, int radius = 1)
	{
		m_textureID = Texture;
		m_position = pos;
		m_velocity = vel;
		m_width = Width; m_height = Height;
		m_angle = Angle;
		m_radius = radius;
		m_numFrames = nFrames;
		m_currentRow = row;
		m_currentFrame = cframe;
	}

	virtual void update();
	virtual void handleEvents();
	virtual void draw();

	virtual ~Entity() {};
};

//class car : public Entity
//{
//public:
//	car()
//	{
//		m_name = "car";
//	}
//
//	void update();
//};
//
//
//class asteroid : public Entity
//{
//public:
//	asteroid()
//	{
//		m_velocity.m_x = rand() % 8 - 4;
//		m_velocity.m_y = rand() % 8 - 4;
//		m_name = "asteroid";
//	}
//
//	void  update();
//
//};

class bullet : public Entity
{
public:
	bullet()
	{
		m_name = "bullet";
	}

	void update();
	void draw();
};


class player : public Entity
{
public:
	int m_health = 0;
	int m_maxHealth = 0;
	int m_mana = 0;
	int m_maxMana = 0;
	int m_attack = 0;
	int m_defense = 0;
	int m_strength = 0;
	int m_dexterity = 0;
	int m_stamina = 0;
	PLAYER_CLASS m_class;
	std::string type = "";
	std::string projectileID = "";

	player()
	{
		m_name = "player";

		// Generate a random class. Not used, i let the player choose the class.
		//m_class = static_cast<PLAYER_CLASS>(std::rand() % static_cast<int>(PLAYER_CLASS::COUNT));
	}

	void configure();

	void update();
	void handleEvents();
	void draw();

	// Returns the player's class.
	PLAYER_CLASS getClass() const {	return m_class;	}
};

class Enemy : public Entity
{
public:
	int m_health = 0;
	int m_maxHealth = 0;
	int m_mana = 0;
	int m_maxMana = 0;
	int m_attack = 0;
	int m_defense = 0;
	int m_strength = 0;
	int m_dexterity = 0;
	int m_stamina = 0;
	std::string type = "";
	SDL_Color m_color = { 0,255,0,0 };
	float m_scale = 1;

	//armor
	int m_hasHelmet = -1;
	int m_hasTorso = -1;
	int m_hasLegs = -1;

	//pathfinding
	//the target positions
	std::vector<Vector2D> m_targetPositions;
	//current target
	Vector2D m_currentTarget;

	Enemy()
	{
		m_name = "enemy";
	}

	void update();
	void draw();

	void updatePathFinding();

};

class Slime : public Enemy
{
public:

	Slime()
	{
		m_name = "slime";
	}

	void draw();
};

class Humanoid : public Enemy
{
public:

	Humanoid()
	{
		m_name = "humanoid";
	}

	void draw();
};

class Gem : public Entity {
public:
	int m_scoreValue = 0;

	Gem() {
		m_name = "gem";
	}
};

class Key : public Entity {
public:
	Key() {
		m_name = "key";
	}
};

class Gold : public Entity {
public:
	int amount = 0;

	Gold() {
		m_name = "gold";
	}
};

class Heart : public Entity {
public:
	int m_health = 0;

	Heart() {
		m_name = "heart";
	}
};

class Potion : public Entity {
public:
	int m_statBoost = 0;
	POTION m_type;

	Potion() {
		m_name = "potion";
	}
};

class Torch : public Entity {
public:
	Torch() {
		m_name = "torch";
	}
};

class Projectile : public Entity {
public:
	Projectile() {
		m_name = "projectile";
	}
};