#pragma once

#include <string>
#include <vector>
#include "state.h"

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
	TILE type;
	int columnIndex;
	int rowIndex;
	SDL_Texture* texture;
	int H; //heuristic/movement cost to goal.
	int G; //movement cost. (total of entire path).
	int F; //estimated cost for full path. (G+H).
	Tile* parentNode; //node to reach this node.
};

class GameState : public State
{
public:
	GameState();
	~GameState();

	virtual void update();
	virtual void render();
	virtual void handleEvents();
	virtual bool onEnter();
	virtual bool onExit();

	virtual STATES getStateID() const { return s_ID; }

	int getTileWidth() const { return m_tileWidth; }
	int getTileHeight() const { return m_tileHeight; }
	bool isFloor(Vector2D v) {
		if (level[(int)v.m_x][(int)v.m_y] == 19) return true;
		if (level[(int)v.m_x][(int)v.m_y] == 20) return true;
		return false;
	}

private:
	static const STATES s_ID;

	std::string numToTile[22] = { "wall_single", "wall_top_end", "wall_side_right_end", "wall_bottom_left", "wall_bottom_end",
					 "wall_side", "wall_top_left", "wall_side_left_t", "wall_side_left_end", "wall_bottom_right", "wall_top",
					 "wall_bottom_t", "wall_top_right", "wall_side_right_t", "wall_top_t", "wall_intersection",
					 "door_locked", "door_unlocked", "wall_entrance", "floor", "floor_alt", "empty" };


	SDL_Color colorMod;
	int level[19][19];

	int m_tileWidth = 50;
	int m_tileHeight = 50;
	
	void createPath(int columnIndex, int rowIndex);
	void createRooms(int roomCount);
	void calculateTextures();
	void populateLevel();

	int gemScore = 0;
	int goldScore = 0;

	std::vector<std::unique_ptr<Entity>> m_items;
	//std::vector<std::unique_ptr<Enemy>> m_enemies;
};