#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

class Scene;
class Character;

class Game
{
public:
	Game();
	~Game();
	void run();

protected:
	unsigned int w_width;
	unsigned int w_height;
	void findCharacters();

private:
	SDL_Window * window;
	SDL_Renderer * renderer;
	uint32_t isprite;
	std::vector<Character *> characters;
};

#endif // GAME_H

