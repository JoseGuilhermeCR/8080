#include "terminal.hh"
#include <string>
#include <memory>

Terminal::Terminal() : window(nullptr)
{
	SDL_Init(SDL_INIT_VIDEO);

	constexpr unsigned WINDOW_WIDTH = 1280;
	constexpr unsigned WINDOW_HEIGHT = 720;

	window = SDL_CreateWindow(
		"Terminal Emulator",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL
	);

	if (window != nullptr) {
		SDL_Delay(3000);
	} else {
		throw std::runtime_error("Couldn't create window.");
	}
}

Terminal::~Terminal() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}