#include "SDL.h"

#pragma once

#ifndef SDLDRAWER_H
#define SDLDRAWER_H

using namespace std;

class SDLdrawer
{
public:
	SDLdrawer(void);
	~SDLdrawer(void);
	void setupScreen(void);
	void drawDot(SDL_Renderer*, int, int);
	void drawGraphics();
private:
	SDL_Renderer* renderer;
	SDL_Window* MainWindow;
};

#endif
