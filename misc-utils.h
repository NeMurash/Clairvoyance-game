#ifndef MISC_UTILS_H
#include <SDL3/SDL.h>
#define MISC_UTILS_H

// Map a number that falls between min0 and max0 to a number that falls between min1 and max1
float mapNumber(float n, float min0, float max0, float min1, float max1);

// Screen To World Coordinates Mapping ((-1, 1), (-1, 1)) -> ((0, 1280), (0, 720))
SDL_FPoint STWCoords(SDL_FPoint point, float screenWidth, float screenHeight);

// Linear interpolation of a vector (https://en.wikipedia.org/wiki/Linear_interpolation)
SDL_FPoint lerpV(SDL_FPoint v0, SDL_FPoint v1, float t);

// Check if a point is inside of a rectangular region
bool pointInRect(SDL_FPoint pt, SDL_FRect rect);

// A tool that will help us later
struct Card {
	SDL_Texture* texture;
	SDL_FPoint   position;
	SDL_FPoint   targetPosition;
	SDL_FRect    rect;
};

#endif
