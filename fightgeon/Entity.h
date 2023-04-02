#pragma once
#include <sdl.h>
#include <string>
#include "Vector2D.h"
#include "AssetsManager.h"

class Game;

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

	virtual void draw();

	virtual ~Entity() {};
};

class car : public Entity
{
public:
	car()
	{
		m_name = "car";
	}

	void update();
};


class asteroid : public Entity
{
public:
	asteroid()
	{
		m_velocity.m_x = rand() % 8 - 4;
		m_velocity.m_y = rand() % 8 - 4;
		m_name = "asteroid";
	}

	void  update();

};

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

	player()
	{
		m_name = "player";
	}

	void update();
	void draw();

};

