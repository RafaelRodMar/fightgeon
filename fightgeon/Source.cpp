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

	SDL_Surface *surface = IMG_Load("assets/img/ui/spr_aim.png");
	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, 16, 16);
	SDL_SetCursor(cursor);

	ReadHiScores();

	//read level data from file (the data is not used in this game)
	/*std::ifstream in("assets/data/level_data.txt");
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
	}*/

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

	//set a random music
	Mix_Volume(-1, MIX_MAX_VOLUME);

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
				AssetsManager::Instance()->draw(numToTile[level[i][j].type], j * m_tileWidth, i * m_tileHeight, m_tileWidth, m_tileHeight, Game::Instance()->getRenderer());
			}
		}

		for (auto& i : m_items)
			i->draw();

		for (auto& i : m_enemies)
			i->draw();

		p->draw();

		//draw all enemy paths
		/*for (auto& j : m_enemies)
		{
			for (int i = 0; i < j->m_targetPositions.size(); i++) {
				AssetsManager::Instance()->draw("path", j->m_targetPositions[i].m_x * 50, j->m_targetPositions[i].m_y * 50, 50, 50, Game::Instance()->getRenderer(), SDL_FLIP_NONE);
			}
		}*/

		//ui
		int sw = Game::Instance()->m_gameWidth;
		int sh = Game::Instance()->m_gameHeight;
		int scw = Game::Instance()->m_gameWidth / 2;
		int sch = Game::Instance()->m_gameHeight / 2;
		AssetsManager::Instance()->draw(heroType[heroNum] + "_ui", 8, 8, 59, 59, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("bar_outline", 90, 10, 221, 16, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("bar_outline", 90, 30, 221, 16, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("health_bar", 96, 15, 213, 8, m_pRenderer, SDL_FLIP_NONE);
		AssetsManager::Instance()->draw("mana_bar", 96, 35, 213, 8, m_pRenderer, SDL_FLIP_NONE);
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
		
		if(hasKey)
			AssetsManager::Instance()->drawFrameSc("key_ui", sw - 180, sh - 70, 180, 90, 0.5, 0, 0, m_pRenderer, 0.0, 255, SDL_FLIP_NONE); //scaled, half the size
		else
			AssetsManager::Instance()->drawFrameSc("key_ui_transparent", sw - 180, sh - 70, 180, 90, 0.5, 0, 0, m_pRenderer, 0.0, 255, SDL_FLIP_NONE); //scaled, half the size

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
		// Draw the level goal if active.
		if (m_activeGoal)
		{
			AssetsManager::Instance()->Text(m_goalString, "fontad", scw / 2, sh - 75, { 255,255,255,255 }, m_pRenderer);
		}
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
	AssetsManager::Instance()->clearMusicAndSounds();
	AssetsManager::Instance()->clearTextureMap();
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
				//reset
				for (auto& i : m_items) {
					delete i;
				}
				m_items.clear();

				for (auto& i : m_enemies) {
					delete i;
				}
				m_enemies.clear();

				//create player
				if (p != nullptr) delete p;
				p = new player();
				p->configure();

				//create map
				//no change, red, green, yellow, magenta, cyan
				SDL_Color colorList[6] = { {255,255,255}, {255,0,0}, {0,255,0}, {255,253,1}, {236,5,229}, {0,255,255} };
				colorMod = colorList[rnd.getRndInt(0, 5)];
				//modify the color of the tiles
				for (int i = 0; i < 22; i++) {
					AssetsManager::Instance()->applyColorMod(numToTile[i], colorMod.r, colorMod.g, colorMod.b);
				}

				//first create a grid of nodes (all walls, the nodes are empty)
				//the nodes are the ones with row and col are odd
				for (int i = 0; i < 19; i++) {
					for (int j = 0; j < 19; j++) {
						if ((i % 2 != 0) && (j % 2 != 0))
						{
							level[i][j].type = (int)TILE::EMPTY; //empty cell
						}
						else
						{
							level[i][j].type = (int)TILE::WALL_TOP; //wall_top
						}
						level[i][j].rowIndex = i;
						level[i][j].columnIndex = j;
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

				//select and play music
				int musNum = rand() % 4 + 1; //numbers 1,2,3,4
				AssetsManager::Instance()->playMusic("music" + std::to_string(musNum), -1);

				gemScore = 0;
				goldScore = 0;
				hasKey = false;

				generateLevelGoals();

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
		if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_ESCAPE))
		{
			AssetsManager::Instance()->stopMusic();
			AssetsManager::Instance()->stopAllSounds();
			state = MENU;
		}
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

		for (auto& i : m_items) {
			if (i->m_name == "gold")
			{
				if (isCollideRect(p, i))
				{
					// Get the amount of gold.
					int goldValue = dynamic_cast<Gold&>(*i).amount;
					// Add to the gold total.
					goldScore += goldValue;
					// Play gold collect sound effect
					AssetsManager::Instance()->playSound("coin_pickup", 0);
					//mark the object for being destroyed later
					i->m_life = false;
					//check if we have an active level goal
					if (m_activeGoal)
					{
						m_goldGoal -= goldValue;
					}
				}
			}

			if (i->m_name == "gem")
			{
				if (isCollideRect(p, i))
				{
					// Get the amount of gold.
					int scoreValue = dynamic_cast<Gem&>(*i).m_scoreValue;
					// Add to the gold total.
					gemScore += scoreValue;
					// Play gold collect sound effect
					AssetsManager::Instance()->playSound("gem_pickup", 0);
					//mark the object for being destroyed later
					i->m_life = false;
					//check if we have an active level goal
					if (m_activeGoal)
					{
						--m_gemGoal;
					}
				}
			}

			if (i->m_name == "key")
			{
				if (isCollideRect(p, i))
				{
					hasKey = true;
					AssetsManager::Instance()->playSound("key_pickup", 0);
					i->m_life = false;
				}
			}

			if (i->m_name == "heart")
			{
				if (isCollideRect(p, i))
				{
					p->m_health = p->m_maxHealth;
					//AssetsManager::Instance()->playSound("heart_pickup", 0);
					i->m_life = false;
				}
			}

			if (i->m_name == "potion")
			{
				if (isCollideRect(p, i))
				{
					POTION pt = dynamic_cast<Potion*>(i)->m_type;
					
					switch (pt) {
					case POTION::ATTACK:
						p->m_attack++;
						break;
					case POTION::DEFENSE:
						p->m_defense++;
						break;
					case POTION::STRENGTH:
						p->m_strength++;
						break;
					case POTION::DEXTERITY:
						p->m_dexterity++;
						break;
					case POTION::STAMINA:
						p->m_stamina++;
						break;
					case POTION::HEALTH:
						p->m_health++;
						break;
					case POTION::MANA:
						p->m_mana++;
						break;
					default:
						break;
					}

					i->m_life = false;
				}
			}

			if (i->m_name == "bullet")
			{
				for (auto& j : m_enemies) {
					if (isCollideRect(j, i))
					{
						i->m_life = false;
						j->m_life = false;
						AssetsManager::Instance()->playSound("enemy_dead", 0);

						//if we have an active goal decrement killGoal
						if (m_activeGoal)
						{
							--m_killGoal;
						}
					}
				}
			}
		}

		//remove objects if needed
		for (auto i = m_items.begin(); i != m_items.end();)
		{
			Entity *e = *i;

			if (e->m_life == false) { i = m_items.erase(i); delete e; }
			else i++;
		}

		//remove enemies if needed
		for (auto i = m_enemies.begin(); i != m_enemies.end();)
		{
			Entity *e = *i;

			if (e->m_life == false) { i = m_enemies.erase(i); delete e; }
			else i++;
		}

		//play sound if torch near
		bool torchSound = false;
		for (auto& i : m_items) {
			if (i->m_name == "torch")
			{
				if (std::sqrt((i->m_position.m_x - p->m_position.m_x)*(i->m_position.m_x - p->m_position.m_x)
					+ (i->m_position.m_y - p->m_position.m_y)*(i->m_position.m_y - p->m_position.m_y)) < 100)
				{
					torchSound = true;
					if (!Mix_Playing(3))
					{
						Mix_PlayChannel(3, AssetsManager::Instance()->getSound("fire"), -1); //infinite loop
					}
				}
			}
		}
		if (torchSound == false) Mix_HaltChannel(3); //if not torch near then stop.

		for (auto& i : m_items)
			i->update();

		for (auto& i : m_enemies)
			i->update();

		p->update();

		//check if the player has moved grid square
		Tile* playerCurrentTile = getTile(p->m_position);

		if (m_playerPreviousTile != playerCurrentTile)
		{
			//store the new tile
			m_playerPreviousTile = playerCurrentTile;

			//update path finding for all enemies if within range of the player
			for (const auto& enemy : m_enemies) {
				if (sqrt((enemy->m_position.m_x - p->m_position.m_x) * (enemy->m_position.m_x - p->m_position.m_x) +
					(enemy->m_position.m_y - p->m_position.m_y) * (enemy->m_position.m_y - p->m_position.m_y)) < 300.0f)
				{
					enemy->updatePathFinding();
				}
			}
		}
	}

	//check if we have completed an active goal
	if (m_activeGoal)
	{
		if ((m_gemGoal <= 0) && (m_goldGoal <= 0) && (m_killGoal <= 0))
		{
			score += std::rand() % 1001 + 1000;
			m_activeGoal = false;
		}
		else
		{
			std::ostringstream ss;

			if (m_goldGoal > 0)
				ss << "Current Goal: Collect " << m_goldGoal << " gold";
			else if (m_gemGoal > 0)
				ss << "Current Goal: Collect " << m_gemGoal << " gem";
			else if (m_killGoal > 0)
				ss << "Current Goal: Kill " << m_killGoal << " enemies";

			m_goalString = ss.str();
		}
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
	int r = rnd.getRndInt(0, floorTiles.size() - 1);
	spawnItem(ITEM::GEM, floorTiles[r]);
	floorTiles.erase(floorTiles.begin() + r); //so it can't be used again

	//create a key
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	spawnItem(ITEM::KEY, floorTiles[r]);
	floorTiles.erase(floorTiles.begin() + r);

	//create gold small
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	spawnItem(ITEM::GOLD_SMALL, floorTiles[r]);
	floorTiles.erase(floorTiles.begin() + r);

	//create gold medium
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	spawnItem(ITEM::GOLD_MEDIUM, floorTiles[r]);
	floorTiles.erase(floorTiles.begin() + r);

	//create gold large
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	spawnItem(ITEM::GOLD_LARGE, floorTiles[r]);
	floorTiles.erase(floorTiles.begin() + r);

	//create heart
	r = rnd.getRndInt(0, floorTiles.size() - 1);
	spawnItem(ITEM::HEART, floorTiles[r]);
	floorTiles.erase(floorTiles.begin() + r);

	//create 3 potions
	for (int i = 0; i < 3; i++) {
		r = rnd.getRndInt(0, floorTiles.size() - 1);
		spawnItem(ITEM::POTION, floorTiles[r]);
		floorTiles.erase(floorTiles.begin() + r);
	}

	//set some torches
	std::vector<Vector2D> wallTiles;
	for (int j = 0; j < 19; j++) {
		for (int i = 0; i < 19; i++) {
			if (i > 0 && j > 0 && i < 18 && j < 18)
			{
				if (level[i][j].type == 0 || level[i][j].type == 10 || level[i][j].type == 11)
				{
					wallTiles.push_back(Vector2D(j, i)); //inverted
				}
			}
		}
	}

	for (int i = 0; i < 5; i++) {
		r = rnd.getRndInt(0, wallTiles.size() - 1);
		Torch* torch = new Torch();
		torch->settings("torch", wallTiles[r] * 50, Vector2D(0, 0), 90 / 5, 36, 5, 0, 0, 0.0, 1);
		torch->m_position += Vector2D(m_tileWidth / 2 - torch->m_width / 2, m_tileHeight / 2 - torch->m_height / 2);
		m_items.push_back(torch);
		wallTiles.erase(wallTiles.begin() + r);
	}

	//add some enemies
	for (int i = 0; i < 5; i++)
	{
		spawnEnemy(static_cast<ENEMY>(rnd.getRndInt(0, static_cast<int>(ENEMY::COUNT) - 1)));
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
			if (level[dy][dx].type == (int)TILE::EMPTY)
			{
				//mark the tile as floor
				level[dy][dx].type = (int)TILE::FLOOR;
				//cout << "mark the tile as floor" << endl;

				//knock the wall down
				int ddx = currentTileCol + (directions[i][0] / 2);
				int ddy = currentTileRow + (directions[i][1] / 2);

				level[ddy][ddx].type = (int)TILE::FLOOR;

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
					level[newI][newY].type = (int)TILE::FLOOR;
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
			if ((level[i][j].type >= 0 && level[i][j].type <= 15) || level[i][j].type == 18)
			{
				//calculate bit mask
				int value = 0;

				//store the current type as default
				int type = level[i][j].type;

				//top
				if (i > 0 && ((level[i - 1][j].type >= 0 && level[i - 1][j].type <= 15) || level[i - 1][j].type == 18))
				{
					value += 1;
				}

				//right
				if (j < 18 && ((level[i][j + 1].type >= 0 && level[i][j + 1].type <= 15) || level[i][j + 1].type == 18))
				{
					value += 2;
				}

				//bottom
				if (i < 18 && ((level[i + 1][j].type >= 0 && level[i + 1][j].type <= 15) || level[i + 1][j].type == 18))
				{
					value += 4;
				}

				//left
				if (j > 0 && ((level[i][j - 1].type >= 0 && level[i][j - 1].type <= 15) || level[i][j - 1].type == 18))
				{
					value += 8;
				}

				//set the new type
				level[i][j].type = value;
			}
		}
	}
}

//position is optional, if it is -1.0f,-1.0f then a random location is used.
void Game::spawnItem(ITEM itemType, Vector2D position)
{
	// Choose a random, unused spawn location.
	Vector2D spawnLocation;
	if ((position.m_x >= 0.f) || (position.m_y >= 0.f))
	{
		spawnLocation = position;
	}
	else
	{
		//get all floor tiles
		std::vector<Vector2D> floorTiles;
		for (int j = 0; j < 19; j++) {
			for (int i = 0; i < 19; i++) {
				if (isFloor(Vector2D(i, j)))
				{
					floorTiles.push_back(Vector2D(j, i)); //inverted
				}
			}
		}

		//choose one
		spawnLocation = floorTiles[rnd.getRndInt(0, floorTiles.size() - 1)];
	}
	// Check which type of object is being spawned.
	switch (itemType)
	{
	case ITEM::POTION:
	{
		std::string potionNames[7] = { "potion_attack", "potion_defense", "potion_dexterity", "potion_health", "potion_mana", "potion_stamina", "potion_strength" };
		int pName = rnd.getRndInt(0, static_cast<int>(POTION::COUNT)-1);
		Potion* item = new Potion();
		item->settings(potionNames[pName], spawnLocation * 50, Vector2D(0, 0), 120 / 8, 30, 8, 0, 0, 0.0, 1);
		item->m_position += Vector2D(m_tileWidth / 2 - item->m_width / 2, m_tileHeight / 2 - item->m_height / 2);
		item->m_statBoost = rnd.getRndInt(0, 10) + 5;
		item->m_type = static_cast<POTION>(pName);
		m_items.push_back(item);
		break;
	}
	case ITEM::GEM:
	{
		Gem* item = new Gem();
		item->settings("gem", spawnLocation * 50, Vector2D(0, 0), 168 / 8, 25, 8, 0, 0, 0.0, 1);
		//center the item in the tile
		item->m_position += Vector2D(m_tileWidth / 2 - item->m_width / 2, m_tileHeight / 2 - item->m_height / 2);
		item->m_scoreValue = rnd.getRndInt(0, 100);
		// Add the item to the list of all items.
		m_items.push_back(item);
		break;
	}
	case ITEM::GOLD_SMALL:
	{
		Gold* item = new Gold();
		item->settings("gold_small", spawnLocation * 50, Vector2D(0, 0), 96 / 8, 16, 8, 0, 0, 0.0, 1);
		item->m_position += Vector2D(m_tileWidth / 2 - item->m_width / 2, m_tileHeight / 2 - item->m_height / 2);
		item->amount = 20;
		m_items.push_back(item);
		break;
	}
	case ITEM::GOLD_MEDIUM:
	{
		Gold* item = new Gold();
		item->settings("gold_medium", spawnLocation * 50, Vector2D(0, 0), 144 / 8, 19, 8, 0, 0, 0.0, 1);
		item->m_position += Vector2D(m_tileWidth / 2 - item->m_width / 2, m_tileHeight / 2 - item->m_height / 2);
		item->amount = 120;
		m_items.push_back(item);
		break;
	}
	case ITEM::GOLD_LARGE:
	{
		Gold* item = new Gold();
		item->settings("gold_large", spawnLocation * 50, Vector2D(0, 0), 192 / 8, 19, 8, 0, 0, 0.0, 1);
		item->m_position += Vector2D(m_tileWidth / 2 - item->m_width / 2, m_tileHeight / 2 - item->m_height / 2);
		item->amount = 220;
		m_items.push_back(item);
		break;
	}
	case ITEM::KEY:
	{
		Key* item = new Key();
		item->settings("key", spawnLocation * 50, Vector2D(0, 0), 240 / 8, 24, 8, 0, 0, 0.0, 1);
		item->m_position += Vector2D(m_tileWidth / 2 - item->m_width / 2, m_tileHeight / 2 - item->m_height / 2);
		m_items.push_back(item);
		break;
	}
	case ITEM::HEART:
	{
		Heart* item = new Heart();
		item->settings("heart", spawnLocation * 50, Vector2D(0, 0), 120 / 8, 22, 8, 0, 0, 0.0, 1);
		item->m_position += Vector2D(m_tileWidth / 2 - item->m_width / 2, m_tileHeight / 2 - item->m_height / 2);
		item->m_health = rnd.getRndInt(0, 10) + 10;
		m_items.push_back(item);
		break;
	}
	default:
		break;
	}
}

// Spawns a given number of enemies in the level.
void Game::spawnEnemy(ENEMY enemyType, Vector2D position)
{
	// Spawn location of enemy.
	Vector2D spawnLocation;
	// Choose a random, unused spawn location.
	if ((position.m_x >= 0.f) || (position.m_y >= 0.f))
	{
		spawnLocation = position;
	}
	else
	{
		//get all floor tiles
		std::vector<Vector2D> floorTiles;
		for (int j = 0; j < 19; j++) {
			for (int i = 0; i < 19; i++) {
				if (isFloor(Vector2D(i, j)))
				{
					floorTiles.push_back(Vector2D(j, i)); //inverted
				}
			}
		}

		//choose one
		spawnLocation = floorTiles[rnd.getRndInt(0, floorTiles.size() - 1)];
	}

	// Create the enemy.
	Enemy* enemy;
	switch (enemyType)
	{
	case ENEMY::SLIME:
	{
		enemy = new Slime();
		enemy->m_scale = (std::rand() % 11 + 5) / 10.f; //between 0.0 and 1.5
		enemy->settings("slime_idle_down", spawnLocation * 50, Vector2D(0, 0), 33, 18, 1, 0, 0, 0.0, 1);
		enemy->m_position += Vector2D(m_tileWidth / 2 - enemy->m_width / 2, m_tileHeight / 2 - enemy->m_height / 2);
		enemy->type = "slime";
		enemy->m_color = AssetsManager::Instance()->getColorFromInt(std::rand() % static_cast<int>(COLOR::COUNT));
		enemy->m_color.a = std::rand() % 156 + 100; //set the transparency
		break;
	}
	case ENEMY::HUMANOID:
		enemy = new Humanoid();
		if (rnd.getRndInt(0, 1) == 0)
		{
			enemy->settings("goblin_idle_down", spawnLocation * 50, Vector2D(0, 0), 33, 33, 1, 0, 0, 0.0, 1);
			enemy->type = "goblin";
		}
		else
		{
			enemy->settings("skeleton_idle_down", spawnLocation * 50, Vector2D(0, 0), 33, 33, 1, 0, 0, 0.0, 1);
			enemy->type = "skeleton";
		}
		enemy->m_position += Vector2D(m_tileWidth / 2 - enemy->m_width / 2, m_tileHeight / 2 - enemy->m_height / 2);

		//randomly set armor parts with a 20% probability.
		if (rand() % 5 == 0)
		{
			int tierValue = std::rand() % 100 + 1;
			// Select which tier armor should be created.
			if (tierValue < 51) { enemy->m_hasHelmet = 0; }  //50% bronze
			else if (tierValue < 86) { enemy->m_hasHelmet = 1; }  //35% silver
			else { enemy->m_hasHelmet = 2; }  //15% gold
		}
		if (rand() % 5 == 0)
		{
			int tierValue = std::rand() % 100 + 1;
			// Select which tier armor should be created.
			if (tierValue < 51) { enemy->m_hasTorso = 0; }  //50% bronze
			else if (tierValue < 86) { enemy->m_hasTorso = 1; }  //35% silver
			else { enemy->m_hasTorso = 2; }  //15% gold
		}
		if (rand() % 5 == 0)
		{
			int tierValue = std::rand() % 100 + 1;
			// Select which tier armor should be created.
			if (tierValue < 51) { enemy->m_hasLegs = 0; }  //50% bronze
			else if (tierValue < 86) { enemy->m_hasLegs = 1; }  //35% silver
			else { enemy->m_hasLegs = 2; }  //15% gold
		}
		break;
	}
	// Add to list of all enemies.
	m_enemies.push_back(enemy);
}

void Game::resetNodes()
{
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			level[i][j].parentNode = nullptr;
			level[i][j].H = 0;
			level[i][j].G = 0;
			level[i][j].F = 0;
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

void Game::generateLevelGoals()
{
	std::ostringstream ss;

	//reset our goal variables.
	m_killGoal = 0;
	m_goldGoal = 0;
	m_gemGoal = 0;

	//choose which type of goal is to be generated.
	int goalType = rand() % 3;

	switch (goalType) {
	case 0:  //kill x enemies
		m_killGoal = rand() % 6 + 5;
		//string describing the goal
		ss << "Current Goal: Kill " << m_killGoal << " enemies!";
		break;
	case 1:  //collect X gold
		m_goldGoal = rand() % 51 + 50;
		ss << "Current Goal: Collect " << m_goldGoal << " gold!";
		break;
	case 2:  //collect X gems
		m_gemGoal = rand() % 6 + 5;
		ss << "Current Goal: Collect " << m_gemGoal << " gems!";
		break;
	default:
		break;
	}

	//store the goal string
	m_goalString = ss.str();
	//set the goal as active
	m_activeGoal = true;
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