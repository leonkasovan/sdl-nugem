#ifndef GAME_H
#define GAME_H

#include <SDL.h>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

class Scene;

class Game
{
public:
	Game();
	~Game();
	void run();

protected:
	unsigned int w_width;
	unsigned int w_height;

private:
	SDL_Window * window;
	SDL_Renderer * renderer;
	uint32_t isprite;
};

#endif // GAME_H

