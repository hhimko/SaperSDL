#include "Saper.h"

#include <math.h> 

// TODO: unhardcode board dimentions
Saper::Saper(const uint8_t w, const uint8_t h)
	: mWidth(w), mHeight(h)
{
	createBoard();
}

Saper::Saper(const uint8_t s)
	: Saper(s,s)
{

}

Saper::~Saper()
{

}

SaperCell* Saper::getCell(uint16_t i)
{
	return &mBoard[i];
}

SaperCell* Saper::getCell(uint8_t x, uint8_t y)
{
	return &mBoard[y * mWidth + x];
}

void Saper::revealCell(SaperCell* cell)
{
	// TODO: end the game upon revealing a bomb
	if (!cell->hidden || cell->flagged)
		return;

	if (!mInGame)
		startGame(cell);

	cell->hidden = false;

	if (cell->type == SaperCell::Type::CELL_EMPTY)
		revealNeighbors(cell);
}

void Saper::flagCell(SaperCell* cell)
{
	// TODO: introduce max cap on placed flags
	if (!cell->hidden)
		return;

	cell->flagged = !cell->flagged;
}

void Saper::createBoard()
{
	// TODO: initialize board only after first reveal so bombs dont appear under first pick
	mBoard = vector<SaperCell>(mWidth * mHeight);

	uint8_t col = 0;
	uint8_t row = 0;
	for (auto& cell : mBoard) {
		cell.col = col;
		cell.row = row;

		++col %= mWidth;
		row += col == 0;
	}
}

void Saper::startGame(SaperCell* startingCell)
{
	unordered_set<SaperCell*> excludedCells = getNeighbors(startingCell);
	excludedCells.insert(startingCell);

	placeBombs(time(0), excludedCells);

	mInGame = true;
}

void Saper::placeBombs(unsigned int seed, unordered_set<SaperCell*> excluded)
{
	uint16_t bombCount = mBoard.size() * mBombRatio + excluded.size();
	unordered_set<SaperCell*> bombCells = excluded;

	srand(seed);
	while (bombCells.size() < bombCount) {
		auto rolled = rand() % mBoard.size();
		bombCells.insert(&mBoard[rolled]);
	}

	for (const auto ex : excluded)
		bombCells.erase(ex);

	for (const auto cell : bombCells) {
		cell->type = SaperCell::Type::CELL_BOMB;

		for (const auto neighbor : getNeighbors(cell)) {
			if (neighbor->type != SaperCell::Type::CELL_BOMB)
				neighbor->type = (SaperCell::Type)((int)neighbor->type + 1);
		}
	}

}

unordered_set<SaperCell*> Saper::getNeighbors(SaperCell* cell) 
{
	unordered_set<SaperCell*> neighbors;

	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			if (dx == 0 && dy == 0)
				continue;

			int x = cell->col + dx;
			int y = cell->row + dy;

			if (0 <= x && x < mWidth && 0 <= y && y < mHeight) {
				uint16_t i = y * mWidth + x;
				neighbors.insert(&mBoard[i]);
			}
		}
	}

	return neighbors;
}

void Saper::revealNeighbors(SaperCell* cell)
{
	for (auto neighbour : getNeighbors(cell))
		revealCell(neighbour);
}