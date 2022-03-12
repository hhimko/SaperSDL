#pragma once
#include <SDL.h>

#include <vector>
#include <unordered_set>

enum class CellType {
	CELL_EMPTY,
	CELL_NEAR1,
	CELL_NEAR2,
	CELL_NEAR3,
	CELL_NEAR4,
	CELL_NEAR5,
	CELL_NEAR6,
	CELL_NEAR7,
	CELL_NEAR8,
	CELL_BOMB
};

struct Cell {
	uint8_t col, row;
	CellType type = CellType::CELL_EMPTY;
	SDL_Rect rect;

	bool hidden = true;
	bool flagged = false;
};

class Saper {
public:
	Saper(const uint8_t w, const uint8_t h);
	Saper(const uint8_t s);

	int run();

public:

private:
	bool initSDL(void);
	void initBoard();
	void placeBombs(float bombRatio);

	void gameloop();
	bool handleInput(SDL_Event* event);
	void render();

	void onMouseUp(SDL_MouseButtonEvent* event);
	void onMouseDown(SDL_MouseButtonEvent* event);

	Cell* getHoveredCell();
	void revealCell(Cell* cell);
	void revealNeighboringCells(Cell* cell);
	void flagCell(Cell* cell);

	uint32_t getCellColor(Cell* cell);
	void renderCell(Cell* cell);
	void renderCellLabel(Cell* cell);
	void renderBoard();

	void shutdown();

private:
	uint8_t mWidth, mHeight;
	std::vector<Cell> mBoard;

	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;
};