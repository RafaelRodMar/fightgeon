#include <vector>
#include <list>
#include <SDL.h>
#include <SDL_image.h>
#include "Vector2D.h"
#include "AssetsManager.h"
#include "InputHandler.h"
#include "Entity.h"
#include "statemachine.h"
#include "menuState.h"
#include "gamestate.h"
#include <chrono>
#include <random>

class Rnd {
public:
	std::mt19937 rng;

	Rnd()
	{
		std::mt19937 prng(std::chrono::steady_clock::now().time_since_epoch().count());
		rng = prng;
	}

	int getRndInt(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(rng);
	}

	double getRndDouble(double min, double max)
	{
		std::uniform_real_distribution<double> distribution(min, max);
		return distribution(rng);
	}
} rnd;

class Game {
public:
	static Game* Instance()
	{
		if (s_pInstance == 0)
		{
			s_pInstance = new Game();
			return s_pInstance;
		}
		return s_pInstance;
	}

	SDL_Renderer* getRenderer() const { return m_pRenderer; }

	~Game();

	bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void render();
	void update();
	void handleEvents();
	void clean();
	void quit();

	bool running() { return m_bRunning; }

	int getGameWidth() const { return m_gameWidth; }
	int getGameHeight() const { return m_gameHeight; }

	void UpdateHiScores(int newscore);
	void ReadHiScores();
	void WriteHiScores();

	std::unique_ptr<player> p;

private:
	Game();
	static Game* s_pInstance;
	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;

	StateMachine* m_state;

	int state = -1;
	std::vector<int> vhiscores;
	int lives, score;

	//std::list<Entity*> entities;
	bool isCollide(Entity *a, Entity *b);
	bool isCollideRect(Entity *a, Entity *b);

	bool m_bRunning;
	int m_gameWidth;
	int m_gameHeight;
};