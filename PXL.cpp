#include "PXL.h"

static uint32_t DIGITS[10] = {
	0b111111000111111,
	0b1111100001,
	0b101111010111101,
	0b111111010110001,
	0b111110010000111,
	0b111011010110111,
	0b111001010011111,
	0b111110000100001,
	0b111111010111111,
	0b111110010100111
};

static uint8_t getStreamWidth(uint32_t stream)
{
	return floor(log(stream) / log(32)) + 1;
}

static void renderStream(SDL_Renderer* renderer, uint32_t stream, uint8_t width, int xs, int ys, int size) 
{
	SDL_Rect pixel = {xs, ys, size, size};

	for (uint32_t x = 0; x < width; ++x) {
		for (uint32_t y = 0; y < 5; ++y) {

			if (stream&1)
				SDL_RenderFillRect(renderer, &pixel);

			stream >>= 1;
			pixel.y += size;
		}
		pixel.y = ys;
		pixel.x += size;
	}
}

int PXL::getLabelWidth(PXL::Number* label)
{
	int w = 0;

	int digitsCount = floor(log10(label->value)) + 1;
	for (int i = digitsCount - 1; i >= 0; --i) {
		auto digit = (int)floor(label->value / pow(10, i)) % 10;
		uint32_t stream = DIGITS[digit];

		w += getStreamWidth(stream);
	}

	return w * label->size;
}

int PXL::getLabelWidth(PXL::Text* label)
{
	return 0;
}

int PXL::getLabelHeight(PXL::Number* label)
{
	return 5 * label->size;
}

int PXL::getLabelHeight(PXL::Text* label)
{
	return 0;
}

void PXL::render(SDL_Renderer* renderer, PXL::Number* label)
{
	int x = label->x;

	int digitsCount = floor(log10(label->value)) + 1;
	for (int i = digitsCount - 1; i >= 0; --i) {
		auto digit = (int)floor(label->value / pow(10, i)) % 10;
		uint32_t stream = DIGITS[digit];

		auto w = getStreamWidth(stream);

		renderStream(renderer, stream, w, x, label->y, label->size);

		x += (w + 1) * label->size;
	}
}

void PXL::render(SDL_Renderer* renderer, PXL::Text* label)
{

}

void PXL::renderCentered(SDL_Renderer* renderer, PXL::Number* label)
{
	int x = label->x - getLabelWidth(label) / 2;
	int y = label->y - getLabelHeight(label) / 2;

	int digitsCount = floor(log10(label->value)) + 1;
	for (int i = digitsCount - 1; i >= 0; --i) {
		auto digit = (int)floor(label->value / pow(10, i)) % 10;
		uint32_t stream = DIGITS[digit];

		auto w = getStreamWidth(stream);

		renderStream(renderer, stream, w, x, y, label->size);

		x += (w + 1) * label->size;
	}
}

void PXL::renderCentered(SDL_Renderer* renderer, PXL::Text* label)
{

}