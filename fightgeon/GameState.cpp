#include "GameState.h"
#include "game.h"

const std::string GameState::s_ID = "GAMESTATE";

GameState::GameState()
{
}


GameState::~GameState()
{
}

void GameState::update()
{
}

void GameState::render()
{
}

void GameState::handleEvents()
{
}

bool GameState::onEnter()
{
	return false;
}

bool GameState::onExit()
{
	return false;
}

void GameState::populateLevel()
{
	std::vector<Vector2D> floorTiles;
	for (int j = 0; j < 19; j++) {
		for (int i = 0; i < 19; i++) {
			if (isFloor(Vector2D(i, j)))
			{
				floorTiles.push_back(Vector2D(j, i)); //inverted
			}
		}
	}

	//create a gem
	std::unique_ptr<Gem> gem = std::make_unique<Gem>();

	//set the gem settings
	int r = rnd.getRndInt(0, floorTiles.size() - 1);
	gem->settings("gem", floorTiles[r] * 50, Vector2D(0, 0), 168 / 8, 25, 8, 0, 0, 0.0, 1);
	//center the position of the item in the tile
	gem->m_position += Vector2D(Game::Instance()->getTileWidth() / 2 - gem->m_width / 2, Game::Instance()->getTileHeight() / 2 - gem->m_height / 2);
	//add the gem to the collection of all objects. (need to use std::move for unique_ptr)
	m_items.push_back(std::move(gem));

	floorTiles.erase(floorTiles.begin() + r); //so it can't be used again

	//create a key
	std::unique_ptr<Key> key = std::make_unique<Key>();
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	key->settings("key", floorTiles[r] * 50, Vector2D(0, 0), 240 / 8, 24, 8, 0, 0, 0.0, 1);
	key->m_position += Vector2D(m_tileWidth / 2 - key->m_width / 2, m_tileHeight / 2 - key->m_height / 2);
	m_items.push_back(std::move(key));
	floorTiles.erase(floorTiles.begin() + r);

	//create gold small
	std::unique_ptr<Gold> gold = std::make_unique<Gold>();
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	gold->settings("gold_small", floorTiles[r] * 50, Vector2D(0, 0), 96 / 8, 16, 8, 0, 0, 0.0, 1);
	gold->m_position += Vector2D(m_tileWidth / 2 - gold->m_width / 2, m_tileHeight / 2 - gold->m_height / 2);
	gold->amount = 20;
	m_items.push_back(std::move(gold));
	floorTiles.erase(floorTiles.begin() + r);

	//create gold medium
	std::unique_ptr<Gold> goldmed = std::make_unique<Gold>();
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	goldmed->settings("gold_medium", floorTiles[r] * 50, Vector2D(0, 0), 144 / 8, 19, 8, 0, 0, 0.0, 1);
	goldmed->m_position += Vector2D(m_tileWidth / 2 - goldmed->m_width / 2, m_tileHeight / 2 - goldmed->m_height / 2);
	goldmed->amount = 120;
	m_items.push_back(std::move(goldmed));
	floorTiles.erase(floorTiles.begin() + r);

	//create gold large
	std::unique_ptr<Gold> goldlarge = std::make_unique<Gold>();
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	goldlarge->settings("gold_large", floorTiles[r] * 50, Vector2D(0, 0), 192 / 8, 19, 8, 0, 0, 0.0, 1);
	goldlarge->m_position += Vector2D(m_tileWidth / 2 - goldlarge->m_width / 2, m_tileHeight / 2 - goldlarge->m_height / 2);
	goldlarge->amount = 220;
	m_items.push_back(std::move(goldlarge));
	floorTiles.erase(floorTiles.begin() + r);

	//create heart
	std::unique_ptr<Heart> heart = std::make_unique<Heart>();
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	heart->settings("heart", floorTiles[r] * 50, Vector2D(0, 0), 120 / 8, 22, 8, 0, 0, 0.0, 1);
	heart->m_position += Vector2D(m_tileWidth / 2 - heart->m_width / 2, m_tileHeight / 2 - heart->m_height / 2);
	m_items.push_back(std::move(heart));
	floorTiles.erase(floorTiles.begin() + r);

	//create 3 potions
	std::string potionNames[7] = { "potion_attack", "potion_defense", "potion_dexterity", "potion_health", "potion_mana", "potion_stamina", "potion_strength" };
	for (int i = 0; i < 3; i++) {
		int pName = rnd.getRndInt(0, 6);
		std::unique_ptr<Potion> potion = std::make_unique<Potion>();
		r = rnd.getRndInt(0, floorTiles.size() - 1);
		potion->settings(potionNames[pName], floorTiles[r] * 50, Vector2D(0, 0), 120 / 8, 30, 8, 0, 0, 0.0, 1);
		potion->m_position += Vector2D(m_tileWidth / 2 - potion->m_width / 2, m_tileHeight / 2 - potion->m_height / 2);
		m_items.push_back(std::move(potion));
		floorTiles.erase(floorTiles.begin() + r);
	}
}

//carve paths recursively to create a maze
void GameState::createPath(int columnIndex, int rowIndex) {
	//store the current tile
	int currentTileCol = columnIndex;
	int currentTileRow = rowIndex;
	//cout << "Tile: " << currentTileCol << "," << currentTileRow << endl;

	//create a list of possible directions and sort randomly
	vector< vector<int> > directions = { {0,-2}, {2,0}, {0,2}, {-2,0} };
	//cout << "tile: " << currentTileCol << "," << currentTileRow << " dir: " << directions[0][0] << "," << directions[0][1] << endl;
	random_shuffle(begin(directions), end(directions));
	//cout << "tile: " << currentTileCol << "," << currentTileRow << " dir: " << directions[0][0] << "," << directions[0][1] << endl;

	//for each direction
	for (int i = 0; i < 4; i++) {
		//cout << "dir: " << i << " ";
		//get the new tile position
		int dx = currentTileCol + directions[i][0];
		int dy = currentTileRow + directions[i][1];

		//if the tile is valid
		if (dx >= 0 && dy >= 0 && dx < 19 && dy < 19)
		{
			//cout << "is valid" << endl;
			//store the tile
			int tileCol = dx;
			int tileRow = dy;

			//if the tile has not yet been visited
			if (level[dy][dx] == 21)
			{
				//mark the tile as floor
				level[dy][dx] = 19;
				//cout << "mark the tile as floor" << endl;

				//knock the wall down
				int ddx = currentTileCol + (directions[i][0] / 2);
				int ddy = currentTileRow + (directions[i][1] / 2);

				level[ddy][ddx] = 19;

				//recursively call the function with the new tile
				createPath(dx, dy);
			}
		}
	}

}

//choose random places and convert tiles to floor
void GameState::createRooms(int roomCount) {
	for (int i = 0; i < roomCount; i++) {
		//generate a room size
		int roomWidth = rnd.getRndInt(1, 2);
		int roomHeight = rnd.getRndInt(1, 2);

		//choose a random starting location
		int startI = rnd.getRndInt(1, 17);
		int startY = rnd.getRndInt(1, 17);

		for (int j = -1; j < roomWidth; ++j) {
			for (int z = -1; z < roomHeight; ++z) {
				int newI = startI + j;
				int newY = startY + z;

				//check if the tile is valid
				if (newI > 0 && newY > 0 && newI < 18 && newY < 18)
				{
					level[newI][newY] = 19;
				}
			}
		}
	}
}

//calculates the correct texture for each tile in the level
void GameState::calculateTextures() {
	/*for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			std::cout << level[i][j] << " ";
		}
		std::cout << std::endl;
	}*/
	//for each tile in the grid
	for (int i = 0; i < 19; ++i) {
		for (int j = 0; j < 19; ++j) {
			//check if the tile is a wall block
			if ((level[i][j] >= 0 && level[i][j] <= 15) || level[i][j] == 18)
			{
				//calculate bit mask
				int value = 0;

				//store the current type as default
				int type = level[i][j];

				//top
				if ((level[i - 1][j] >= 0 && level[i - 1][j] <= 15) || level[i - 1][j] == 18)
				{
					value += 1;
				}

				//right
				if ((level[i][j + 1] >= 0 && level[i][j + 1] <= 15) || level[i][j + 1] == 18)
				{
					value += 2;
				}

				//bottom
				if ((level[i + 1][j] >= 0 && level[i + 1][j] <= 15) || level[i + 1][j] == 18)
				{
					value += 4;
				}

				//left
				if ((level[i][j - 1] >= 0 && level[i][j - 1] <= 15) || level[i][j - 1] == 18)
				{
					value += 8;
				}

				//set the new type
				level[i][j] = value;
			}
		}
	}
}
