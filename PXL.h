#pragma once
#include <SDL.h>
#include <string>

namespace PXL 
{
	struct Label {
		int x, y;
		int size = 10;
	};

	struct Number : Label {
		int value;

		Number(int val, int x, int y, int s = 10)
			: value(val), Label{ x, y, s }{}
	};

	struct Text : Label {	
		std::string value;

		Text(std::string val, int x, int y, int s = 10)
			: value(val), Label{ x, y, s }{}
	};

	int getLabelWidth(Number* label);
	int getLabelWidth(Text* label);
	int getLabelHeight(Number* label);
	int getLabelHeight(Text* label);


	void render(SDL_Renderer* renderer, Number* label);
	void render(SDL_Renderer* renderer, Text* label);
	void renderCentered(SDL_Renderer* renderer, Number* label);
	void renderCentered(SDL_Renderer* renderer, Text* label);
}
