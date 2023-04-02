#include<SDL.h>
#include<sdl_ttf.h>
#include<iostream>
#include<fstream>
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

	//read level data from file
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

	//no change, red, green, blue, yellow, magenta, cyan
	SDL_Color colorList[7] = { {255,255,255}, {255,0,0}, {0,255,0}, {0,0,255}, {255,253,1}, {236,5,229}, {0,255,255} };
	colorMod = colorList[rnd.getRndInt(0, 6)];
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

	state = GAME;

	return true;
}

void Game::render()
{
	SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer); // clear the renderer to the draw color

	if (state == MENU)
	{
	}

	if (state == GAME)
	{
		for (int i = 0; i < 19; i++) {
			for (int j = 0; j < 19; j++) {
				AssetsManager::Instance()->draw(numToTile[level[i][j]], j * 50, i * 50, 50, 50, Game::Instance()->getRenderer());
			}
		}

		for (auto i : entities)
			i->draw();
	}

	if (state == END_GAME)
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
	}

	if (state == GAME)
	{
	}

	if (state == END_GAME)
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
	if (state == GAME)
	{

		for (auto i = entities.begin(); i != entities.end(); i++)
		{
			Entity *e = *i;

			e->update();
		}
	}

}

void Game::populateLevel()
{

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
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			std::cout << level[i][j] << " ";
		}
		std::cout << std::endl;
	}
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
	if (Game::Instance()->init("Procedural Content Generation", 20, 20, 1024, 950,
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