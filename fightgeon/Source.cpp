#include<SDL.h>
#include<sdl_ttf.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<random>
#include<time.h>
#include<chrono>
#include<stack>
#include "game.h"

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

//la clase juego:
Game* Game::s_pInstance = 0;

Game::Game()
{
	m_pRenderer = NULL;
	m_pWindow = NULL;
}

Game::~Game()
{

}

bool Game::init(const char* title, int xpos, int ypos, int width,
	int height, bool fullscreen)
{
	// almacenar el alto y ancho del juego.
	m_gameWidth = width;
	m_gameHeight = height;

	// attempt to initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		int flags = 0;
		if (fullscreen)
		{
			flags = SDL_WINDOW_FULLSCREEN;
		}

		std::cout << "SDL init success\n";
		// init the window
		m_pWindow = SDL_CreateWindow(title, xpos, ypos,
			width, height, flags);
		if (m_pWindow != 0) // window init success
		{
			std::cout << "window creation success\n";
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != 0) // renderer init success
			{
				std::cout << "renderer creation success\n";
				SDL_SetRenderDrawColor(m_pRenderer,
					255, 255, 255, 255);
			}
			else
			{
				std::cout << "renderer init fail\n";
				return false; // renderer init fail
			}
		}
		else
		{
			std::cout << "window init fail\n";
			return false; // window init fail
		}
	}
	else
	{
		std::cout << "SDL init fail\n";
		return false; // SDL init fail
	}
	if (TTF_Init() == 0)
	{
		std::cout << "sdl font initialization success\n";
	}
	else
	{
		std::cout << "sdl font init fail\n";
		return false;
	}

	std::cout << "init success\n";
	m_bRunning = true; // everything inited successfully, start the main loop

	//load images, sounds, music and fonts
	AssetsManager::Instance()->loadAssetsJson(); //ahora con formato json
	Mix_Volume(-1, 16); //adjust sound/music volume for all channels

	ReadHiScores();

	//read level data from file (the data is not used in this game)
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

	//create player
	p = std::make_unique<player>();
	p->settings("warrior_idle_down", Vector2D(50,50), Vector2D(0, 0), 33, 33, 1, 0, 0, 0.0, 1);
	p->m_position += Vector2D(m_tileWidth / 2 - p->m_width / 2, m_tileHeight / 2 - p->m_height / 2);
	p->type = "warrior"; //default character

	//set the stats
	int m_statPoints = 50;
	float attackBias = rnd.getRndInt(0, 100);
	float defenseBias = rnd.getRndInt(0, 100);
	float strengthBias = rnd.getRndInt(0, 100);
	float dexterityBias = rnd.getRndInt(0, 100);
	float staminaBias = rnd.getRndInt(0, 100);

	float total = attackBias + defenseBias + strengthBias + dexterityBias + staminaBias;

	p->m_attack += m_statPoints * (attackBias / total);
	p->m_defense += m_statPoints * (defenseBias / total);
	p->m_strength += m_statPoints * (strengthBias / total);
	p->m_dexterity += m_statPoints * (dexterityBias / total);
	p->m_stamina += m_statPoints * (staminaBias / total);

	//menu init
	button0 = { 0,0,100,100 };
	r0 = 0; g0 = 0; b0 = 255; a0 = 255;
	button1 = { 105,0, 100,100 };
	r1 = 0; g1 = 255; b1 = 0; a1 = 255;

	//choose hero init
	button2 = { 0,0,100,100 };
	r2 = 0; g2 = 0; b2 = 255; a2 = 255;
	button3 = { 105, 0, 100, 50 };
	r3 = 0; g3 = 255; b3 = 0; a3 = 255;
	button4 = { 105, 51, 100, 50 };
	r4 = 0; g4 = 150; b4 = 0; a4 = 255;

	state = MENU;

	return true;
}

void Game::render()
{
	SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer); // clear the renderer to the draw color

	if (state == MENU)
	{
		Uint8 r, g, b, a;
		SDL_GetRenderDrawColor(Game::Instance()->getRenderer(), &r, &g, &b, &a);

		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r0, g0, b0, a0);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &button0);
		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r1, g1, b1, a1);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &button1);

		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r, g, b, a);
		AssetsManager::Instance()->Text("Start", "fontad", 10, 40, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->Text("Exit", "fontad", 130, 40, { 255,255,255,255 }, m_pRenderer);
	}

	if (state == CHOOSEHERO)
	{
		Uint8 r, g, b, a;
		SDL_GetRenderDrawColor(Game::Instance()->getRenderer(), &r, &g, &b, &a);

		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r2, g2, b2, a2);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &button2);
		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r3, g3, b3, a3);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &button3);
		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r4, g4, b4, a4);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &button4);

		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), r, g, b, a);
		AssetsManager::Instance()->Text(" GO ", "fontad", 10, 40, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->Text("next", "fontad", 120, 20, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->Text("last", "fontad", 120, 60, { 255,255,255,255 }, m_pRenderer);

		AssetsManager::Instance()->draw(heroType[heroNum] + "_ui", 250, 20, 59, 59, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->Text(heroType[heroNum], "fontad", 250, 90, { 255,255,255,255 }, m_pRenderer);
	}

	if (state == GAME)
	{
		for (int i = 0; i < 19; i++) {
			for (int j = 0; j < 19; j++) {
				AssetsManager::Instance()->draw(numToTile[level[i][j]], j * m_tileWidth, i * m_tileHeight, m_tileWidth, m_tileHeight, Game::Instance()->getRenderer());
			}
		}

		/*for (auto& i : entities)
			i->draw();*/

		for (auto& i : m_items)
			i->draw();

		p->draw();

		//ui
		int sw = Game::Instance()->m_gameWidth;
		int sh = Game::Instance()->m_gameHeight;
		int scw = Game::Instance()->m_gameWidth / 2;
		int sch = Game::Instance()->m_gameHeight / 2;
		AssetsManager::Instance()->draw(heroType[heroNum] + "_ui", 8, 8, 59, 59, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("bar_outline", 105, 10, 221, 16, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("bar_outline", 105, 30, 221, 16, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("health_bar", 109, 15, 213, 8, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("mana_bar", 109, 35, 213, 8, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("gem_ui", scw - 160, 8, 84, 72, m_pRenderer, SDL_FLIP_NONE);
		//set a number with leading zeroes
		std::stringstream ss;
		ss << std::setw(6) << std::setfill('0') << gemScore;
		std::string num = ss.str();
		AssetsManager::Instance()->Text(num, "fontad", scw - 50, 20, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->draw("coin_ui", scw + 90, 8, 96, 48, m_pRenderer, SDL_FLIP_NONE);
		ss.str(std::string()); //clear the stringstream
		ss << std::setw(6) << std::setfill('0') << goldScore;
		num = ss.str();
		AssetsManager::Instance()->Text(num, "fontad", scw + 200, 20, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->draw("key_ui", sw - 180, sh - 90, 180, 90, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("attack_ui", scw - 270, sh - 40, 35, 35, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->Text(std::to_string(p->m_attack), "fontad", scw - 210, sh - 35, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->draw("defense_ui", scw - 150, sh - 40, 35, 35, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->Text(std::to_string(p->m_defense), "fontad", scw - 90, sh - 35, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->draw("strength_ui", scw - 30, sh - 40, 45, 25, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->Text(std::to_string(p->m_strength), "fontad", scw + 30, sh - 35, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->draw("dexterity_ui", scw + 90, sh - 40, 35, 35, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->Text(std::to_string(p->m_dexterity), "fontad", scw + 150, sh - 35, { 255,255,255,255 }, m_pRenderer);
		AssetsManager::Instance()->draw("stamina_ui", scw + 210, sh - 40, 35, 35, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->Text(std::to_string(p->m_stamina), "fontad", scw + 270, sh - 35, { 255,255,255,255 }, m_pRenderer);
	}

	if (state == ENDGAME)
	{
	}

	SDL_RenderPresent(m_pRenderer); // draw to the screen
}

void Game::quit()
{
	m_bRunning = false;
}

void Game::clean()
{
	WriteHiScores();
	std::cout << "cleaning game\n";
	InputHandler::Instance()->clean();
	AssetsManager::Instance()->clearFonts();
	TTF_Quit();
	Game::Instance()->m_bRunning = false;
	SDL_DestroyWindow(m_pWindow);
	SDL_DestroyRenderer(m_pRenderer);
	SDL_Quit();
}

void Game::handleEvents()
{
	InputHandler::Instance()->update();

	//HandleKeys
	if (state == MENU)
	{
		if (InputHandler::Instance()->getMouseButtonState(LEFT))
		{
			Vector2D* mousePos = InputHandler::Instance()->getMousePosition();

			if (mousePos->m_x > button0.x && mousePos->m_x < button0.x + button0.w &&
				mousePos->m_y > button0.y && mousePos->m_y < button0.y + button0.h)
			{
				state = CHOOSEHERO;
			}

			if (mousePos->m_x > button1.x && mousePos->m_x < button1.x + button1.w &&
				mousePos->m_y > button1.y && mousePos->m_y < button1.y + button1.h)
			{
				quit();
			}

			InputHandler::Instance()->reset(); //reset the buttons
		}
	}

	if (state == CHOOSEHERO)
	{
		if (InputHandler::Instance()->getMouseButtonState(LEFT))
		{
			Vector2D* mousePos = InputHandler::Instance()->getMousePosition();

			if (mousePos->m_x > button2.x && mousePos->m_x < button2.x + button2.w &&
				mousePos->m_y > button2.y && mousePos->m_y < button2.y + button2.h)
			{
				p->type = heroType[heroNum];
				state = GAME;
			}

			if (mousePos->m_x > button3.x && mousePos->m_x < button3.x + button3.w &&
				mousePos->m_y > button3.y && mousePos->m_y < button3.y + button3.h)
			{
				if(heroNum < 3) heroNum++;
			}

			if (mousePos->m_x > button4.x && mousePos->m_x < button4.x + button4.w &&
				mousePos->m_y > button4.y && mousePos->m_y < button4.y + button4.h)
			{
				if (heroNum > 0) heroNum--;
			}

			InputHandler::Instance()->reset(); //reset the buttons
		}
	}

	if (state == GAME)
	{
		p->handleEvents();
		if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_ESCAPE)) state = MENU;
	}

	if (state == ENDGAME)
	{

	}

}

bool Game::isCollide(Entity *a, Entity *b)
{
	return (b->m_position.m_x - a->m_position.m_x)*(b->m_position.m_x - a->m_position.m_x) +
		(b->m_position.m_y - a->m_position.m_y)*(b->m_position.m_y - a->m_position.m_y) <
		(a->m_radius + b->m_radius)*(a->m_radius + b->m_radius);
}

bool Game::isCollideRect(Entity *a, Entity * b) {
	if (a->m_position.m_x < b->m_position.m_x + b->m_width &&
		a->m_position.m_x + a->m_width > b->m_position.m_x &&
		a->m_position.m_y < b->m_position.m_y + b->m_height &&
		a->m_height + a->m_position.m_y > b->m_position.m_y) {
		return true;
	}
	return false;
}

void Game::update()
{
	if (state == MENU)
	{
		Vector2D* mousePos = InputHandler::Instance()->getMousePosition();

		b0 = 255; g1 = 255;
		if (mousePos->m_x > button0.x && mousePos->m_x < button0.x + button0.w &&
			mousePos->m_y > button0.y && mousePos->m_y < button0.y + button0.h)
		{
			b0 = 180;
		}

		if (mousePos->m_x > button1.x && mousePos->m_x < button1.x + button1.w &&
			mousePos->m_y > button1.y && mousePos->m_y < button1.y + button1.h)
		{
			g1 = 180;
		}
	}

	if (state == CHOOSEHERO)
	{

	}

	if (state == GAME)
	{

		/*for (auto i = entities.begin(); i != entities.end(); i++)
		{
			Entity *e = *i;

			e->update();
		}*/

		for (auto& i : m_items)
			i->update();

		p->update();
	}

}

void Game::populateLevel()
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
void Game::createPath(int columnIndex, int rowIndex) {
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
void Game::createRooms(int roomCount) {
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
void Game::calculateTextures() {
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

void Game::UpdateHiScores(int newscore)
{
	//new score to the end
	vhiscores.push_back(newscore);
	//sort
	sort(vhiscores.rbegin(), vhiscores.rend());
	//remove the last
	vhiscores.pop_back();
}

void Game::ReadHiScores()
{
	std::ifstream in("hiscores.dat");
	if (in.good())
	{
		std::string str;
		getline(in, str);
		std::stringstream ss(str);
		int n;
		for (int i = 0; i < 5; i++)
		{
			ss >> n;
			vhiscores.push_back(n);
		}
		in.close();
	}
	else
	{
		//if file does not exist fill with 5 scores
		for (int i = 0; i < 5; i++)
		{
			vhiscores.push_back(0);
		}
	}
}

void Game::WriteHiScores()
{
	std::ofstream out("hiscores.dat");
	for (int i = 0; i < 5; i++)
	{
		out << vhiscores[i] << " ";
	}
	out.close();
}

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

int main(int argc, char* args[])
{
	srand(time(NULL));

	Uint32 frameStart, frameTime;

	std::cout << "game init attempt...\n";
	if (Game::Instance()->init("Procedural Content Generation", 20, 20, 950, 950,
		false))
	{
		std::cout << "game init success!\n";
		while (Game::Instance()->running())
		{
			frameStart = SDL_GetTicks(); //tiempo inicial

			Game::Instance()->handleEvents();
			Game::Instance()->update();
			Game::Instance()->render();

			frameTime = SDL_GetTicks() - frameStart; //tiempo final - tiempo inicial

			if (frameTime < DELAY_TIME)
			{
				SDL_Delay((int)(DELAY_TIME - frameTime)); //esperamos hasta completar los 60 fps
			}
		}
	}
	else
	{
		std::cout << "game init failure - " << SDL_GetError() << "\n";
		return -1;
	}
	std::cout << "game closing...\n";
	Game::Instance()->clean();
	return 0;
}