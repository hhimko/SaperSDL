#pragma once
#include <iostream>

#include <SDL.h>
#include "Saper.h"
#include "PXL.h"

#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 800

#define COLOR_BG 0x0B0D11FF
#define COLOR_TEXT         0x5C5D6AFF
#define COLOR_CELL_HIDDEN1 0x0EDEAfFF
#define COLOR_CELL_HIDDEN2 0x760EBEFF
#define COLOR_CELL_FLAGGED 0x000000FF
#define COLOR_CELL_EMPTY   0x5C5D6AFF
#define COLOR_CELL_NEAR    0x2B2E38FF
#define COLOR_CELL_BOMB	   0xB71F1FFF

#define CELL_SIZE   50 // TODO: automatic cell size measurement
#define CELL_OFFSET 5
#define TEXT_SIZE   (int)(CELL_SIZE / 8.)

#define RGBA(x) (x>>24)&0xFF, (x>>16)&0xFF, (x>>8)&0xFF, x&0xFF

class App {
public:
	App();
	~App();
	int run();

public:

private:
	bool initSDL();

	void mainloop();
	bool handleInput(SDL_Event* event);
	void render();

	void onMouseUp(SDL_MouseButtonEvent* event);
	void onMouseDown(SDL_MouseButtonEvent* event);

	uint32_t getCellColor(SaperCell* cell);
	SDL_Rect* getCellRect(SaperCell* cell);
	SaperCell* getHoveredCell();

	void renderCell(SaperCell* cell);
	void renderCellLabel(SaperCell* cell);
	void renderBoard();

	void shutdown();

private:
	Saper* mSaper = nullptr;

	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;

};