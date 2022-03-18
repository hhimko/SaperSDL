#pragma once
#include <iostream>
#include <unordered_set>
#include <vector>

using std::unordered_set;
using std::vector;

struct SaperCell {

	static enum class Type {
		CELL_EMPTY = 0,
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

	uint8_t col, row;
	Type type = Type::CELL_EMPTY;

	bool hidden = true;
	bool flagged = false;
};

class Saper {
public:
	Saper(const uint8_t w, const uint8_t h);
	Saper(const uint8_t s);
	~Saper();

public:
	uint8_t width() { return mWidth; }
	uint8_t height() { return mHeight; }
	uint16_t size() { return mWidth * mHeight; }
	bool inGame() { return mInGame; }


	SaperCell* getCell(uint16_t i);
	SaperCell* getCell(uint8_t x, uint8_t y);

	void revealCell(SaperCell* cell);
	void flagCell(SaperCell* cell);

private:
	void createBoard();
	void startGame(SaperCell* startingCell);
	void placeBombs(unsigned int seed, unordered_set<SaperCell*> excludedIndex);

	unordered_set<SaperCell*> getNeighbors(SaperCell* cell);
	void revealNeighbors(SaperCell* cell);

private:
	uint8_t mWidth, mHeight;
	float mBombRatio = 0.15;
	vector<SaperCell> mBoard;

	bool mInGame = false;
};