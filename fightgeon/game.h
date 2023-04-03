#include <vector>
#include <list>
#include <SDL.h>
#include <SDL_image.h>
#include "Vector2D.h"
#include "AssetsManager.h"
#include "InputHandler.h"
#include "Entity.h"

////Game States
enum GAMESTATES { MENU, LOAD_LEVEL, GAME, END_GAME };

// Tiles.
enum class TILE {
	WALL_SINGLE,
	WALL_TOP_END,
	WALL_SIDE_RIGHT_END,
	WALL_BOTTOM_LEFT,
	WALL_BOTTOM_END,
	WALL_SIDE,
	WALL_TOP_LEFT,
	WALL_SIDE_LEFT_T,
	WALL_SIDE_LEFT_END,
	WALL_BOTTOM_RIGHT,
	WALL_TOP,
	WALL_BOTTOM_T,
	WALL_TOP_RIGHT,
	WALL_SIDE_RIGHT_T,
	WALL_TOP_T,
	WALL_INTERSECTION,
	WALL_DOOR_LOCKED,
	WALL_DOOR_UNLOCKED,
	WALL_ENTRANCE,
	FLOOR,
	FLOOR_ALT,
	EMPTY,
	COUNT
};

struct Tile {
	TILE type;
	int columnIndex;
	int rowIndex;
	SDL_Texture* texture;
	int H; //heuristic/movement cost to goal.
	int G; //movement cost. (total of entire path).
	int F; //estimated cost for full path. (G+H).
	Tile* parentNode; //node to reach this node.
};

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

private:
	Game();
	static Game* s_pInstance;
	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;

	player *p;
	int state = -1;
	std::vector<int> vhiscores;
	int lives, score;

	SDL_Color colorMod;
	int level[19][19];
	void populateLevel();
	void createPath(int columnIndex, int rowIndex);
	void createRooms(int roomCount);
	void calculateTextures();

	std::string numToTile[22] = { "wall_single", "wall_top_end", "wall_side_right_end", "wall_bottom_left", "wall_bottom_end",
						 "wall_side", "wall_top_left", "wall_side_left_t", "wall_side_left_end", "wall_bottom_right", "wall_top",
						 "wall_bottom_t", "wall_top_right", "wall_side_right_t", "wall_top_t", "wall_intersection",
						 "door_locked", "door_unlocked", "wall_entrance", "floor", "floor_alt", "empty" };


	std::list<Entity*> entities;
	bool isCollide(Entity *a, Entity *b);
	bool isCollideRect(Entity *a, Entity *b);
	//std::vector<GameObject*> m_gameObjects;
	//std::vector<std::unique_ptr<Item>> m_items;
	//std::vector<std::unique_ptr<Enemy>> m_enemies;

	bool m_bRunning;
	int m_gameWidth;
	int m_gameHeight;
};