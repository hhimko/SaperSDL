#include "Saper.h"
#include "PXL.h"

#include <iostream>
#include <math.h> 

#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 820

#define BOMB_RATIO 0.15f // TODO: unhardcode difficulty

#define COLOR_BG		   0x0B0D11FF
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

static uint32_t mixRGB(uint32_t x, uint32_t y, double ratio)
{
	uint32_t a = (x & 0xFF) + (int)((y & 0xFF) - (x & 0xFF)) * ratio;
	uint32_t b = ((x >> 8) & 0xFF) + (int)(((y >> 8) & 0xFF) - ((x >> 8) & 0xFF)) * ratio;
	uint32_t g = ((x >> 16) & 0xFF) + (int)(((y >> 16) & 0xFF) - ((x >> 16) & 0xFF)) * ratio;
	uint32_t r = ((x >> 24) & 0xFF) + (int)(((y >> 24) & 0xFF) - ((x >> 24) & 0xFF)) * ratio;

	return a + b * 256 + g * 65536 + r * 16777216;
}

// TODO: unhardcode board dimentions
Saper::Saper(const uint8_t w, const uint8_t h)
	: mWidth(w), mHeight(h)
{
	initBoard();
}

Saper::Saper(const uint8_t s)
	: Saper(s,s)
{

}

int Saper::run()
{
	if (!initSDL())
		return 1;

	gameloop();

	shutdown();
	return 0;
}

bool Saper::initSDL() 
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	uint32_t winFlags = SDL_WINDOW_SHOWN;
	mWindow = SDL_CreateWindow(
		"SaperSDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, winFlags
	);

	if (mWindow == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	uint32_t renFlags = SDL_RENDERER_ACCELERATED;
	mRenderer = SDL_CreateRenderer(mWindow, -1, renFlags);

	if (mRenderer == NULL)
	{
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
}

void Saper::placeBombs(float bombRatio)
{
	uint16_t bombCount = mBoard.size() * bombRatio;
	std::unordered_set<uint16_t> bombCoords;

	srand(time(0));
	while (bombCoords.size() < bombCount) {
		auto rolled = rand() % mBoard.size();
		bombCoords.insert(rolled);
	}

	Cell* cell = nullptr;
	Cell* neighbour = nullptr;
	for (const auto& coord : bombCoords) {
		cell = &mBoard[coord];
		cell->type = CellType::CELL_BOMB;

		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0)
					continue;

				int x = cell->col + dx;
				int y = cell->row + dy;

				if (0 <= x && x < mWidth && 0 <= y && y < mHeight) {
					uint16_t i = y * mWidth + x;

					neighbour = &mBoard[i];
					if (neighbour->type != CellType::CELL_BOMB)
						neighbour->type = (CellType)((int)neighbour->type + 1);
				}
			}
		}
	}

}

void Saper::initBoard() 
{
	// TODO: initialize board only after first reveal so bombs dont appear under first pick
	mBoard = std::vector<Cell>(mWidth * mHeight);

	uint8_t col = 0;
	uint8_t row = 0;
	for (auto &cell : mBoard) {
		cell.col = col;
		cell.row = row;

		cell.rect = { col*(CELL_SIZE + CELL_OFFSET), row*(CELL_SIZE + CELL_OFFSET), CELL_SIZE, CELL_SIZE };

		++col %= mWidth;
		row += col == 0;
	}

	placeBombs(BOMB_RATIO);
}

void Saper::gameloop()
{
	bool running = true;

	SDL_Event event;
	while (running) {
		running = handleInput(&event);
		render();

		SDL_Delay(10); // TODO: introduce FPS capping system
	}
}

bool Saper::handleInput(SDL_Event* event) 
{
	bool result = true;

	while (SDL_PollEvent(event)) {
		switch (event->type) {
		case SDL_QUIT:
			result = false;
			break;

		case SDL_MOUSEBUTTONUP:
			onMouseUp(&event->button);
			break;

		case SDL_MOUSEBUTTONDOWN:
			onMouseDown(&event->button);
			break;

		case SDL_TEXTINPUT:
			// TODO: listen to reset on 'r' key 
			// TODO: introduce keyboard controls
			break;
		}
	}

	return result;
}

void Saper::render() 
{
	// TODO: render GUI elements:
	//		- restart button
	//		- board size manipulation buttons
	//		- flags placed/bombs label
	//		- time elapsed label

	SDL_SetRenderDrawColor(mRenderer, RGBA(COLOR_BG));
	SDL_RenderClear(mRenderer);

	renderBoard();

	SDL_RenderPresent(mRenderer);
}

void Saper::onMouseUp(SDL_MouseButtonEvent* event)
{
	// TODO: reveal cells on mouse up event, only if the same cell was clicked initially
}

void Saper::onMouseDown(SDL_MouseButtonEvent* event)
{
	// TODO: add GUI buttons
	// TODO: listen for game win event on all cells revealed and bombs flagged
	if (event->button == SDL_BUTTON_LEFT) {
		Cell* hovered = getHoveredCell();
		if (hovered != nullptr)
			revealCell(hovered);
	}
	else if (event->button == SDL_BUTTON_RIGHT) {
		Cell* hovered = getHoveredCell();
		if (hovered != nullptr)
			flagCell(hovered);
	}
}

Cell* Saper::getHoveredCell()
{
	SDL_Point cursor;
	SDL_GetMouseState(&cursor.x, &cursor.y);

	size_t i = (cursor.y / (CELL_SIZE + CELL_OFFSET)) * mWidth + cursor.x / (CELL_SIZE + CELL_OFFSET);
	if (i >= mBoard.size())
		return nullptr;

	Cell* cell = &mBoard[i];
	if (SDL_PointInRect(&cursor, &cell->rect))
		return cell;

	return nullptr;
}

void Saper::revealCell(Cell* cell)
{
	// TODO: end the game upon revealing a bomb
	if (!cell->hidden || cell->flagged)
		return;

	cell->hidden = false;

	if (cell->type == CellType::CELL_EMPTY)
		revealNeighboringCells(cell);
}

void Saper::revealNeighboringCells(Cell* cell)
{
	Cell* neighbour = nullptr;

	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			if (dx == 0 && dy == 0)
				continue;

			int x = cell->col + dx;
			int y = cell->row + dy;


			if (0 <= x && x < mWidth && 0 <= y && y < mHeight) {
				uint16_t i = y * mWidth + x;
				neighbour = &mBoard[i];

				revealCell(neighbour);
			}
		}
	}

	neighbour = nullptr;
}

void Saper::flagCell(Cell* cell)
{
	// TODO: introduce max cap on placed flags
	if (!cell->hidden)
		return;

	if (cell->flagged) {
		cell->flagged = false;
	}
	else {
		cell->flagged = true;
	}
}

uint32_t Saper::getCellColor(Cell* cell)
{
	if (cell->flagged)
		return COLOR_CELL_FLAGGED; 

	if (cell->hidden) {
		double ratio = cell->col / (double)(mWidth-1) / 2 + cell->row / (double)(mHeight - 1) / 2;
		return mixRGB(COLOR_CELL_HIDDEN1, COLOR_CELL_HIDDEN2, ratio);
	}

	switch (cell->type) {
	case CellType::CELL_EMPTY:
		return COLOR_CELL_EMPTY;

	case CellType::CELL_BOMB:
		return COLOR_CELL_BOMB;

	default:
		return COLOR_CELL_NEAR;
	}
}

void Saper::renderCell(Cell* cell)
{
	// TODO: render flag icons
	auto col = getCellColor(cell); 

	SDL_SetRenderDrawColor(mRenderer, RGBA(col));
	SDL_RenderFillRect(mRenderer, &cell->rect);
	
	renderCellLabel(cell);
}

void Saper::renderCellLabel(Cell* cell)
{
	if (!cell->hidden) {
		auto label = (uint8_t)cell->type;

		if (0 < label && label < 9) {
			int x = cell->rect.x + CELL_SIZE / 2;
			int y = cell->rect.y + CELL_SIZE / 2;
			PXL::Number digit{ label, x, y, TEXT_SIZE };

			SDL_SetRenderDrawColor(mRenderer, RGBA(COLOR_TEXT));
			PXL::renderCentered(mRenderer, &digit);
		}
	}
}

void Saper::renderBoard() 
{
	for (auto &cell : mBoard)
		renderCell(&cell);

	Cell* hovered = getHoveredCell();
	if (hovered != nullptr) {
		SDL_SetRenderDrawColor(mRenderer, RGBA(0xFFFFFFFF));
		SDL_RenderDrawRect(mRenderer, &hovered->rect);
	}
}

void Saper::shutdown() 
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	mWindow = nullptr;
	mRenderer = nullptr;
}