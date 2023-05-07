#include <vector>
#include <list>
#include <SDL.h>
#include <SDL_image.h>
#include "Vector2D.h"
#include "AssetsManager.h"
#include "InputHandler.h"
#include "Entity.h"

static const int MAX_ITEM_SPAWN_COUNT = 50;
static const int MAX_ENEMY_SPAWN_COUNT = 20;
static const int PLAYER_TRAIT_COUNT = 2;

// colors
enum class COLOR {
	BLACK,
	WHITE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	MAGENTA,
	CYAN,
	TRANSPARENT,
	COUNT
};

////Game States
enum GAMESTATES { MENU, CHOOSEHERO, GAME, ENDGAME };

// Spawnable items.
enum class ITEM {
	HEART,
	GEM,
	GOLD_SMALL,
	GOLD_MEDIUM,
	GOLD_LARGE,
	POTION,
	KEY,
	COUNT
};

// Potions
enum class POTION {
	ATTACK,
	DEFENSE,
	STRENGTH,
	DEXTERITY,
	STAMINA,
	HEALTH,
	MANA,
	COUNT
};

// Player classes.
enum class PLAYER_CLASS {
	WARRIOR,
	MAGE,
	ARCHER,
	THIEF,
	COUNT
};

// Enemy types.
enum class ENEMY {
	SLIME,
	HUMANOID,
	COUNT
};

// Enemy humanoid types.
enum class HUMANOID {
	GOBLIN,
	SKELETON,
	COUNT
};

// Player traits.
enum class PLAYER_TRAIT {
	ATTACK,
	DEFENSE,
	STRENGTH,
	DEXTERITY,
	STAMINA,
	COUNT
};

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

//the map is in the format row,column.
struct Tile {
	TILE type;  //don't forget to make possible alternative sprites for the same tile (floor, floor_alt)
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
	int getTileWidth() const { return m_tileWidth; }
	int getTileHeight() const { return m_tileHeight; }
	std::string getHeroType() const { return heroType[heroNum]; }
	int getHeroNum() const { return heroNum; }
	bool isFloor(Vector2D v) {
		if (level[(int)v.m_x][(int)v.m_y] == 19) return true;
		if (level[(int)v.m_x][(int)v.m_y] == 20) return true;
		return false;
	}

	void UpdateHiScores(int newscore);
	void ReadHiScores();
	void WriteHiScores();

private:
	Game();
	static Game* s_pInstance;
	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;

	//menu
	SDL_Rect button0;
	Uint8 r0, g0, b0, a0;
	SDL_Rect button1;
	Uint8 r1, g1, b1, a1;

	//choose hero
	int heroNum = 0;
	std::string heroType[4] = { "warrior", "mage", "archer", "thief" };
	SDL_Rect button2;
	Uint8 r2, g2, b2, a2;
	SDL_Rect button3;
	Uint8 r3, g3, b3, a3;
	SDL_Rect button4;
	Uint8 r4, g4, b4, a4;

	player* p;
	int state = -1;
	std::vector<int> vhiscores;
	int lives, score;

	//map
	SDL_Color colorMod;
	int level[19][19];
	void populateLevel();
	int m_tileSize = 50;
	int m_tileWidth = 50;
	int m_tileHeight = 50;
	void createPath(int columnIndex, int rowIndex);
	void createRooms(int roomCount);
	void calculateTextures();
	void spawnItem(ITEM itemType, Vector2D position = { -1.f, -1.f });
	void spawnEnemy(ENEMY enemyType, Vector2D position = { -1.f, -1.f });
	int gemScore = 0;
	int goldScore = 0;
	bool hasKey = false;
	
	std::string numToTile[22] = { "wall_single", "wall_top_end", "wall_side_right_end", "wall_bottom_left", "wall_bottom_end",
						 "wall_side", "wall_top_left", "wall_side_left_t", "wall_side_left_end", "wall_bottom_right", "wall_top",
						 "wall_bottom_t", "wall_top_right", "wall_side_right_t", "wall_top_t", "wall_intersection",
						 "door_locked", "door_unlocked", "wall_entrance", "floor", "floor_alt", "empty" };


	bool isCollide(Entity *a, Entity *b);
	bool isCollideRect(Entity *a, Entity *b);
	std::vector<Entity*> m_items;
	std::vector<Enemy*> m_enemies;

	bool m_bRunning;
	int m_gameWidth;
	int m_gameHeight;
};