#include<SDL.h>
#include<random>
#include<time.h>
#include<stack>

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

// keyboard specific
const Uint8* m_keystates;

bool isKeyDown(SDL_Scancode key)
{
	if (m_keystates != 0)
	{
		if (m_keystates[key] == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

int main(int argc, char* args[])
{
	srand(time(nullptr));

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		// if succeeded create our window
		g_pWindow = SDL_CreateWindow("Fightgeon",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			640, 480,
			SDL_WINDOW_SHOWN);
		// if the window creation succeeded create our renderer
		if (g_pWindow != 0)
		{
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
		}
	}
	else
	{
		return 1; // sdl could not initialize
	}

	bool running = true;
	Uint32 frameStart, frameTime;

	while (running) {
		frameStart = SDL_GetTicks();

		//handle events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYDOWN:
				m_keystates = SDL_GetKeyboardState(0);
				break;

			case SDL_KEYUP:
				m_keystates = SDL_GetKeyboardState(0);
				break;

			default:
				break;
			}
		}

		if (isKeyDown(SDL_SCANCODE_ESCAPE)) running = false;

		//update

		//render
		SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
		// clear the window to black
		SDL_RenderClear(g_pRenderer);

		/*Uint8 cr = 0, cg = 255, cb = 0;
		SDL_SetRenderDrawColor(g_pRenderer, cr, cg, cb, 255);
		SDL_Rect rect;
		rect.x = pila.top().first * (pathWidth + 1) + px;
		rect.y = pila.top().second * (pathWidth + 1) + py;
		rect.x *= cellWidth;
		rect.y *= cellHeight;
		rect.h = cellHeight;
		rect.w = cellWidth;
		SDL_RenderFillRect(g_pRenderer, &rect);*/

		// show the window
		SDL_RenderPresent(g_pRenderer);

		frameTime = SDL_GetTicks() - frameStart;

		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime));
		}
	}

	// clean up SDL
	SDL_DestroyWindow(g_pWindow);
	SDL_DestroyRenderer(g_pRenderer);
	SDL_Quit();
	return 0;
}