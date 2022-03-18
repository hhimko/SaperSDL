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

	cell->hidden = false;

	if (cell->type == SaperCell::Type::CELL_EMPTY)
		revealNeighboringCells(cell);
}

void Saper::flagCell(SaperCell* cell)
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

void Saper::placeBombs(unsigned int seed)
{
	uint16_t bombCount = mBoard.size() * mBombRatio;
	std::unordered_set<uint16_t> bombCoords;

	srand(seed);
	while (bombCoords.size() < bombCount) {
		auto rolled = rand() % mBoard.size();
		bombCoords.insert(rolled);
	}

	SaperCell* cell = nullptr;
	SaperCell* neighbour = nullptr;
	for (const auto& coord : bombCoords) {
		cell = &mBoard[coord];
		cell->type = SaperCell::Type::CELL_BOMB;

		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0)
					continue;

				int x = cell->col + dx;
				int y = cell->row + dy;

				if (0 <= x && x < mWidth && 0 <= y && y < mHeight) {
					uint16_t i = y * mWidth + x;

					neighbour = &mBoard[i];
					if (neighbour->type != SaperCell::Type::CELL_BOMB)
						neighbour->type = (SaperCell::Type)((int)neighbour->type + 1);
				}
			}
		}
	}

}

void Saper::createBoard() 
{
	// TODO: initialize board only after first reveal so bombs dont appear under first pick
	mBoard = std::vector<SaperCell>(mWidth * mHeight);

	uint8_t col = 0;
	uint8_t row = 0;
	for (auto &cell : mBoard) {
		cell.col = col;
		cell.row = row;

		++col %= mWidth;
		row += col == 0;
	}

	placeBombs(time(0));
}

void Saper::revealNeighboringCells(SaperCell* cell)
{
	SaperCell* neighbour = nullptr;

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