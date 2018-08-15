#include "SDLdrawer.h"
#include "chip8.h"

SDLdrawer::SDLdrawer(void)
{
}


SDLdrawer::~SDLdrawer(void)
{
}

void SDLdrawer::setupScreen(void){
	//Initialization
    SDL_Init(SDL_INIT_VIDEO);

    //Window
    MainWindow = SDL_CreateWindow("CHIP8",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  640, 320,
                                  SDL_WINDOW_SHOWN
                                  );

    //Renderer
	renderer = SDL_CreateRenderer(MainWindow, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer); // fill the scene with black
	SDL_RenderPresent(renderer); // copy to screen

}

void SDLdrawer::drawGraphics(void){
	bool empty = true;
	
	//rect array to be displayed on screen
	SDL_Rect rect[2048];
	for(int i = 0; i < 2048; ++i){
		if(chip8::gfx[i] == 1){

			rect[i].x = ((i*10) % 640);
			rect[i].y = 10*i/64;
			rect[i].h = 10;
			rect[i].w = 10;
		}
		if(chip8::gfx[i] == 0){
			rect[i].x = 0;
			rect[i].y = 0;
			rect[i].h = 0;
			rect[i].w = 0;
		}
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // the rect color (black, background)
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // the rect color (solid white)
	SDL_RenderFillRects(renderer, rect, 2048);
	SDL_RenderPresent(renderer);
}

void SDLdrawer::drawDot(SDL_Renderer* renderer, int x, int y){


	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // the rect color (solid white)
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.h = 10;
	rect.w = 10; // the rectangle

	SDL_RenderFillRect(renderer, &rect);
	SDL_RenderPresent(renderer); // copy to screen

}