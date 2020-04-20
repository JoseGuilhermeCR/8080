#ifndef __TERMINAL_HH__
#define __TERMINAL_HH__

#include <SDL2/SDL.h>

class Terminal {
	private:
		SDL_Window *window;
	public:
		Terminal();
		~Terminal();

		void update();
};

#endif