#include "misc-utils.h"
#include <SDL3/SDL.h>

float mapNumber(float n, float min0, float max0, float min1, float max1) {
	return (n - min0) / (max0 - min0) * (max1 - min1) + min1;
}

SDL_FPoint STWCoords(SDL_FPoint point, float screenWidth, float screenHeight) {
	return (SDL_FPoint){
		(point.x + 1) / 2 * screenWidth,
		(point.y + 1) / 2 * screenHeight
	};
}

SDL_FPoint lerpV(SDL_FPoint v0, SDL_FPoint v1, float t) {
	return (SDL_FPoint) {
		(1 - t) * v0.x + t * v1.x,
		(1 - t) * v0.y + t * v1.y
	};
}

bool pointInRect(SDL_FPoint pt, SDL_FRect rect) {
	if ((pt.x > rect.x && pt.x < rect.x + rect.w) &&
		(pt.y > rect.y && pt.y < rect.y + rect.h))
		return true;
	return false;
}
