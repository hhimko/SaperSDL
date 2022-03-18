#include "App.h"

static uint32_t mixRGB(uint32_t x, uint32_t y, double ratio)
{
	uint32_t a = (x & 0xFF) + (int)((y & 0xFF) - (x & 0xFF)) * ratio;
	uint32_t b = ((x >> 8) & 0xFF) + (int)(((y >> 8) & 0xFF) - ((x >> 8) & 0xFF)) * ratio;
	uint32_t g = ((x >> 16) & 0xFF) + (int)(((y >> 16) & 0xFF) - ((x >> 16) & 0xFF)) * ratio;
	uint32_t r = ((x >> 24) & 0xFF) + (int)(((y >> 24) & 0xFF) - ((x >> 24) & 0xFF)) * ratio;

	return a + b * 256 + g * 65536 + r * 16777216;
}

App::App() 
{

}

App::~App()
{
	shutdown();
}

int App::run()
{
	if (!initSDL())
		return 1;

	Saper saper{ 15 };
	mSaper = &saper;
	mainloop();

	shutdown();
	return 0;
}

bool App::initSDL()
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

void App::mainloop()
{
	bool running = true;

	SDL_Event event;
	while (running) {
		running = handleInput(&event);
		render();

		SDL_Delay(10); // TODO: introduce FPS capping system
	}
}

bool App::handleInput(SDL_Event* event)
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

void App::render()
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

void App::onMouseUp(SDL_MouseButtonEvent* event)
{
	// TODO: reveal cells on mouse up event, only if the same cell was clicked initially
}

void App::onMouseDown(SDL_MouseButtonEvent* event)
{
	// TODO: add GUI buttons
	// TODO: listen for game win event on all cells revealed and bombs flagged

	if (event->button == SDL_BUTTON_LEFT) {
		SaperCell* hovered = getHoveredCell();
		if (hovered != nullptr)
			mSaper->revealCell(hovered);
	}
	else if (event->button == SDL_BUTTON_RIGHT) {
		SaperCell* hovered = getHoveredCell();
		if (hovered != nullptr)
			mSaper->flagCell(hovered);
	}
}

uint32_t App::getCellColor(SaperCell* cell)
{
	if (cell->flagged)
		return COLOR_CELL_FLAGGED;

	if (cell->hidden) {
		double ratio = cell->col / (double)(mSaper->width() - 1) / 2 + cell->row / (double)(mSaper->height() - 1) / 2;
		return mixRGB(COLOR_CELL_HIDDEN1, COLOR_CELL_HIDDEN2, ratio);
	}

	switch (cell->type) {
	case SaperCell::Type::CELL_EMPTY:
		return COLOR_CELL_EMPTY;

	case SaperCell::Type::CELL_BOMB:
		return COLOR_CELL_BOMB;

	default:
		return COLOR_CELL_NEAR;
	}
}

SDL_Rect* App::getCellRect(SaperCell* cell)
{

	int x = cell->col * (CELL_SIZE + CELL_OFFSET);
	int y = cell->row * (CELL_SIZE + CELL_OFFSET);
	SDL_Rect rect{ x, y, CELL_SIZE, CELL_SIZE };

	return &rect;
}

SaperCell* App::getHoveredCell()
{
	SDL_Point cursor{0,0};
	SDL_GetMouseState(&cursor.x, &cursor.y);

	uint16_t i = (cursor.y / (CELL_SIZE + CELL_OFFSET)) * mSaper->width() + cursor.x / (CELL_SIZE + CELL_OFFSET);
	if (i >= mSaper->size())
		return nullptr;

	auto cell = mSaper->getCell(i);
	if (SDL_PointInRect(&cursor, getCellRect(cell)))
		return cell;

	return nullptr;
}

void App::renderCell(SaperCell* cell)
{
	// TODO: render flag icons
	auto col = getCellColor(cell);

	SDL_SetRenderDrawColor(mRenderer, RGBA(col));
	SDL_RenderFillRect(mRenderer, getCellRect(cell));

	renderCellLabel(cell);
}

void App::renderCellLabel(SaperCell* cell)
{
	if (!cell->hidden) {
		auto label = (uint8_t)cell->type;

		if (0 < label && label < 9) {
			auto rect = getCellRect(cell);
			int x = rect->x + CELL_SIZE / 2;
			int y = rect->y + CELL_SIZE / 2;
			PXL::Number digit{ label, x, y, TEXT_SIZE };

			SDL_SetRenderDrawColor(mRenderer, RGBA(COLOR_TEXT));
			PXL::renderCentered(mRenderer, &digit);
		}
	}
}

void App::renderBoard()
{
	for (uint16_t i = 0; i < mSaper->size(); ++i)
		renderCell(mSaper->getCell(i));

	SaperCell* hovered = getHoveredCell();
	if (hovered != nullptr) {
		SDL_SetRenderDrawColor(mRenderer, RGBA(0xFFFFFFFF));
		SDL_RenderDrawRect(mRenderer, getCellRect(hovered));
	}
}

void App::shutdown()
{
	mSaper = nullptr;

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	mWindow = nullptr;
	mRenderer = nullptr;
}