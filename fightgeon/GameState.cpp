#include <fstream>
#include "GameState.h"
#include "game.h"

const STATES GameState::s_ID = GAME;

GameState::GameState()
{
	//read level data from file (the data is not used)
	std::ifstream in("assets/data/level_data.txt");
	if (in.good())
	{
		std::string str;
		int row = 0;
		while (std::getline(in, str))
		{
			std::stringstream ss(str);
			int data;
			for (int i = 0; i < 19; i++) {
				ss >> data;
				level[row][i] = data;
			}
			row++;
		}
		in.close();
	}
	else
	{
		std::cout << "Error loading level data" << std::endl;
	}

	//no change, red, green, yellow, magenta, cyan
	SDL_Color colorList[6] = { {255,255,255}, {255,0,0}, {0,255,0}, {255,253,1}, {236,5,229}, {0,255,255} };
	colorMod = colorList[rnd.getRndInt(0, 5)];
	//modify the color of the tiles
	for (int i = 0; i < 22; i++) {
		AssetsManager::Instance()->applyColorMod(numToTile[i], colorMod.r, colorMod.g, colorMod.b);
	}

	//now there is a map loaded from a file. Let´s create a randomized map

	//first create a grid of nodes (all walls, the nodes are empty)
	//the nodes are the ones with row and col are odd
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			if ((i % 2 != 0) && (j % 2 != 0))
			{
				level[i][j] = 21; //empty cell
			}
			else
			{
				level[i][j] = 10; //wall_top
			}
		}
	}

	//now carve passages between nodes.
	//1- choose a random direction and make a connection to the adjacent node if it
	//has not yet been visited. This node becomes the current node.
	//2- if all adjacent cells in each direction have already been visited, go back to 
	//the last cell.
	//3- if you're back at the start node, the algorithm is complete.
	createPath(1, 1);

	//add some rooms to the level to create some open space.
	createRooms(10);

	//give each tile the correct texture
	calculateTextures();

	populateLevel();

}


GameState::~GameState()
{
}

void GameState::update()
{
	/*for (auto i = entities.begin(); i != entities.end(); i++)
	{
		Entity *e = *i;

		e->update();
	}*/

	for (auto& i : m_items)
		i->update();

	Game::Instance()->p->update();
}

void GameState::render()
{
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			AssetsManager::Instance()->draw(numToTile[level[i][j]], j * m_tileWidth, i * m_tileHeight, m_tileWidth, m_tileHeight, Game::Instance()->getRenderer());
		}
	}

	for (auto& i : m_items)
		i->draw();

	Game::Instance()->p->draw();

	//ui
	int sw = Game::Instance()->getGameWidth();
	int sh = Game::Instance()->getGameHeight();
	int scw = Game::Instance()->getGameWidth() / 2;
	int sch = Game::Instance()->getGameHeight() / 2;
	AssetsManager::Instance()->draw("warrior_ui", 8, 8, 59, 59, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->draw("bar_outline", 105, 10, 221, 16, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->draw("bar_outline", 105, 30, 221, 16, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->draw("health_bar", 109, 15, 213, 8, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->draw("mana_bar", 109, 35, 213, 8, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->draw("gem_ui", scw - 160, 8, 84, 72, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	//set a number with leading zeroes
	std::stringstream ss;
	ss << std::setw(6) << std::setfill('0') << gemScore;
	std::string num = ss.str();
	AssetsManager::Instance()->Text(num, "fontad", scw - 50, 20, { 255,255,255,255 }, Game::Instance()->getRenderer());
	AssetsManager::Instance()->draw("coin_ui", scw + 90, 8, 96, 48, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	ss.str(std::string()); //clear the stringstream
	ss << std::setw(6) << std::setfill('0') << goldScore;
	num = ss.str();
	AssetsManager::Instance()->Text(num, "fontad", scw + 200, 20, { 255,255,255,255 }, Game::Instance()->getRenderer());
	AssetsManager::Instance()->draw("key_ui", sw - 180, sh - 90, 180, 90, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->draw("attack_ui", scw - 270, sh - 40, 35, 35, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->Text(std::to_string(Game::Instance()->p->m_attack), "fontad", scw - 210, sh - 35, { 255,255,255,255 }, Game::Instance()->getRenderer());
	AssetsManager::Instance()->draw("defense_ui", scw - 150, sh - 40, 35, 35, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->Text(std::to_string(Game::Instance()->p->m_defense), "fontad", scw - 90, sh - 35, { 255,255,255,255 }, Game::Instance()->getRenderer());
	AssetsManager::Instance()->draw("strength_ui", scw - 30, sh - 40, 45, 25, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->Text(std::to_string(Game::Instance()->p->m_strength), "fontad", scw + 30, sh - 35, { 255,255,255,255 }, Game::Instance()->getRenderer());
	AssetsManager::Instance()->draw("dexterity_ui", scw + 90, sh - 40, 35, 35, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->Text(std::to_string(Game::Instance()->p->m_dexterity), "fontad", scw + 150, sh - 35, { 255,255,255,255 }, Game::Instance()->getRenderer());
	AssetsManager::Instance()->draw("stamina_ui", scw + 210, sh - 40, 35, 35, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
	AssetsManager::Instance()->Text(std::to_string(Game::Instance()->p->m_stamina), "fontad", scw + 270, sh - 35, { 255,255,255,255 }, Game::Instance()->getRenderer());
}

void GameState::handleEvents()
{
	Game::Instance()->p->handleEvents();
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
	gem->m_position += Vector2D(m_tileWidth / 2 - gem->m_width / 2, m_tileHeight / 2 - gem->m_height / 2);
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
