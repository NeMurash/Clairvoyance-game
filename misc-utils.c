#include "misc-utils.h"
#include <SDL3/SDL.h>

#ifndef SCREEN_W
#define SCREEN_W 1280
#endif
#ifndef SCREEN_H
#define SCREEN_H 720
#endif

SDL_FPoint STWCoords(SDL_FPoint point) {
	return (SDL_FPoint){
		(point.x - (-1)) / (1 - (-1)) * SCREEN_W,
		(point.y - (-1)) / (1 - (-1)) * SCREEN_H,
	};
}
SDL_FPoint WTSCoords(SDL_FPoint point) {
	return (SDL_FPoint){
		(point.x) / SCREEN_W * (1 + 1) - 1,
		(point.y) / SCREEN_H * (1 + 1) - 1,
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
