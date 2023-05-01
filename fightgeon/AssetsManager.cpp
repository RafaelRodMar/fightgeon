#include "AssetsManager.h"
#include "game.h"
#include <sdl.h>
#include <SDL_ttf.h>
#include <fstream>
#include <sstream>

AssetsManager* AssetsManager::s_pInstance = 0;

//TEXTURES

bool AssetsManager::loadTexture(string fileName, string id, SDL_Renderer* pRenderer)
{
	SDL_Surface* pTempSurface = IMG_Load(fileName.c_str()); //create Surface with contents of fileName

	if (pTempSurface == 0) //exception handle load error
	{
		cout << "error loading file: " << fileName << endl;
		cout << SDL_GetError() << " " << fileName << endl;
		return false;
	}

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface); //Pass Surface to Texture
	SDL_FreeSurface(pTempSurface); //Delete Surface

	if (pTexture == 0) //exception handle transiction Surfacte to Texture
	{
		cout << "error creating Texture of file: " << fileName << endl;
		return false;
	}

	m_textureMap[id] = pTexture; //save texture.
	return true;
}

void AssetsManager::draw(std::string id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip)
{
	SDL_Rect srcRect; //source rectangle
	SDL_Rect destRect; //destination rectangle

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x; //select x axis on game window
	destRect.y = y; //select y axis on game window

	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, 0, flip); //Load image to render buffer.
}

void AssetsManager::drawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer *pRenderer, double angle, int alpha, SDL_RendererFlip flip)
{
	SDL_Rect srcRect; //source rectangle
	SDL_Rect destRect; //destination rectangle

	srcRect.x = width * currentFrame;
	srcRect.y = height * (currentRow);
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;

	SDL_SetTextureAlphaMod(m_textureMap[id], alpha);
	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, angle, 0, flip); //Load current frame on the buffer game.
}

void AssetsManager::drawFrameSc(string id, int x, int y, int width, int height, float scale, int currentRow, int currentFrame, SDL_Renderer * pRenderer, double angle, int alpha, SDL_RendererFlip flip)
{
	SDL_Rect srcRect; //source rectangle
	SDL_Rect destRect; //destination rectangle

	srcRect.x = width * currentFrame;
	srcRect.y = height * (currentRow);
	srcRect.w = width;
	srcRect.h = height;
	destRect.x = x + (width - width * scale) / 2;
	destRect.y = y + (height - height * scale) / 2;
	destRect.w = width * scale;
	destRect.h = height * scale;

	SDL_SetTextureAlphaMod(m_textureMap[id], alpha);
	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, angle, 0, flip); //Load current frame on the buffer game.
}

void AssetsManager::drawTile(std::string id, int margin, int spacing, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer *pRenderer)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;
	srcRect.x = margin + (spacing + width) * currentFrame;
	srcRect.y = margin + (spacing + height) * currentRow;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, 0, SDL_FLIP_NONE);
}

void AssetsManager::applyColorMod(string id, int r, int g, int b) {
	SDL_SetTextureColorMod(m_textureMap[id], r, g, b);
}

SDL_Color AssetsManager::getColorFromInt(int colorNum) {
	if (colorNum == 0) return getColorFromString("black");
	if (colorNum == 1) return getColorFromString("white");
	if (colorNum == 2) return getColorFromString("red");
	if (colorNum == 3) return getColorFromString("green");
	if (colorNum == 4) return getColorFromString("blue");
	if (colorNum == 5) return getColorFromString("yellow");
	if (colorNum == 6) return getColorFromString("magenta");
	if (colorNum == 7) return getColorFromString("cyan");
	return { 0,0,0,255 };
}

SDL_Color AssetsManager::getColorFromString(std::string colorName) {
	// Convert the color name to lowercase to simplify comparison
	std::transform(colorName.begin(), colorName.end(), colorName.begin(), ::tolower);

	// Map color names to corresponding SDL_Color values
	if (colorName == "red") {
		return { 255, 0, 0, 255 };
	}
	else if (colorName == "green") {
		return { 0, 255, 0, 255 };
	}
	else if (colorName == "blue") {
		return { 0, 0, 255, 255 };
	}
	else if (colorName == "white") {
		return { 255, 255, 255, 255 };
	}
	else if (colorName == "black") {
		return { 0, 0, 0, 255 };
	}
	else if (colorName == "yellow") {
		return { 255,253,1,255 };
	}
	else if (colorName == "magenta") {
		return { 236,5,229,255 };
	}
	else if (colorName == "cyan") {
		return { 0,255,255,255 };
	}
	else {
		// Default to black if an unknown color name is provided
		std::cout << "Unknown color name: " << colorName << std::endl;
		return { 0, 0, 0, 255 };
	}
}

void AssetsManager::clearFromTextureMap(string id)
{
	m_textureMap.erase(id);
}


// SOUND / MUSIC
bool AssetsManager::loadSound(std::string fileName, std::string id, sound_type type)
{
	if (type == SOUND_MUSIC)
	{
		Mix_Music* pMusic = Mix_LoadMUS(fileName.c_str());
		if (pMusic == 0)
		{
			std::cout << "Could not load music: ERROR - " << Mix_GetError() << std::endl;
			return false;
		}

		m_music[id] = pMusic;
		return true;
	}
	else if (type == SOUND_SFX)
	{
		Mix_Chunk* pChunk = Mix_LoadWAV(fileName.c_str());
		if (pChunk == 0)
		{
			std::cout << "Could not load SFX: ERROR - " << Mix_GetError() << std::endl;
			return false;
		}

		m_sfxs[id] = pChunk;
		return true;
	}
	return false;
}

void AssetsManager::playMusic(std::string id, int loop)
{
	Mix_PlayMusic(m_music[id], loop);
}

void AssetsManager::playSound(std::string id, int loop)
{
	Mix_PlayChannel(-1, m_sfxs[id], loop);
}

//FONTS
bool AssetsManager::loadFont(const string &fileName, string id, int size)
{
	TTF_Font* f = TTF_OpenFont(fileName.c_str(), size);
	if (f == NULL)
	{
		cout << "error opening font file" << endl;
		return false;
	}

	m_fonts[id] = f;
	return true;
}

SDL_Texture* AssetsManager::renderText(const std::string &message, const std::string &font,
	SDL_Color color, SDL_Renderer *renderer)
{
	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(m_fonts[font], message.c_str(), color);
	if (surf == nullptr) {
		cout << "error creating text" << endl;
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr) {
		cout << "error creatig texture" << endl;
	}
	//Clean up the surface
	SDL_FreeSurface(surf);
	return texture;
}

void AssetsManager::Text(const std::string &message, const std::string &font, int x, int y, SDL_Color color, SDL_Renderer *renderer)
{
	SDL_Rect *sr = new SDL_Rect();
	SDL_Rect *ds = new SDL_Rect();

	sr->x = 0; sr->y = 0;
	ds->x = x; ds->y = y;

	SDL_Texture* sf = renderText(message.c_str(), font, color, renderer);

	int texturewidth, textureheight;
	SDL_QueryTexture(sf, NULL, NULL, &texturewidth, &textureheight);
	sr->w = texturewidth; sr->h = textureheight;
	ds->w = texturewidth; ds->h = textureheight;

	SDL_RenderCopy(renderer, sf, sr, ds);

	delete sr;
	delete ds;
	SDL_DestroyTexture(sf);
}

void AssetsManager::loadAssets()
{
	std::ifstream in("assets/assets.txt");
	if (in.good())
	{
		std::string str;
		while (std::getline(in, str))
		{
			std::stringstream ss(str);
			std::string datatype, name, filename;
			ss >> datatype;
			ss >> name;
			ss >> filename;

			if (datatype == "img") loadTexture("assets/img/" + filename, name, Game::Instance()->getRenderer());
			if (datatype == "snd") loadSound("assets/snd/" + filename, name, SOUND_SFX);
			if (datatype == "mus") loadSound("assets/mus/" + filename, name, SOUND_MUSIC);
			if (datatype == "fnt") loadFont("assets/fnt/" + filename, name, 24);
		}
		in.close();
	}
	else
	{
		std::cout << "Error loading assets" << std::endl;
	}
}

template<class UnaryFunction>
void recursive_iterate(const nlohmann::json& j, UnaryFunction f) {
	for (auto it = j.begin(); it != j.end(); ++it) {
		if (it->is_structured())
		{
			recursive_iterate(*it, f);
		}
		else
		{
			f(it);
		}
	}
}

void AssetsManager::loadAssetsJson()
{
	std::ifstream in("assets/assets.json");
	if (in.good())
	{
		nlohmann::json j;
		in >> j;

		string type[5] = { "fnt", "img", "mus", "snd" };

		for (int i = 0; i < 5; i++) {
			nlohmann::json data = j[type[i]]; //obtengo los datos del tipo
			
			for (auto& x : data.items()) {
				string name = x.key();
				string filename = x.value().get<std::string>();

				if (type[i] == "img") loadTexture("assets/img/" + filename, name, Game::Instance()->getRenderer());
				if (type[i] == "snd") loadSound("assets/snd/" + filename, name, SOUND_SFX);
				if (type[i] == "mus") loadSound("assets/mus/" + filename, name, SOUND_MUSIC);
				if (type[i] == "fnt") loadFont("assets/fnt/" + filename, name, 24);
			}
			
		}

		//cout << j["img"] << endl;

		//por cada tipo obtengo el nombre y el nombre del archivo
		/*for (auto& x : j.items() ) {
			//cout << x.key() << " " << x.value() << endl;
			nlohmann::json k = x.value();
			for (auto& y : k.items()) {
				//cout << y.key() << " " << y.value().get<std::string>() << endl;

			}
		}*/

		//recursive_iterate(j, [](nlohmann::json::const_iterator it) {std::cout << *it << std::endl; });
	}
	else
	{
		std::cout << "Error loading assets" << std::endl;
	}
}
