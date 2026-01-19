#ifndef MY_UTILS_H
#include <SDL3/SDL.h>
#define MY_UTILS_H

// DECLARATION

// Map a number that falls between min0 and max0 to a number that falls between min1 and max1
float mapNumber(float n, float min0, float max0, float min1, float max1);
// Screen To World Coordinates Mapping ((-1, 1), (-1, 1)) -> ((0, minmax.x), (0, minmax.y))
SDL_FPoint STWCoords(SDL_FPoint point, SDL_FPoint minmax);
// Linear interpolation of a vector (https://en.wikipedia.org/wiki/Linear_interpolation)
SDL_FPoint lerpV(SDL_FPoint v0, SDL_FPoint v1, float t);
// Check if a point is inside of a rectangular region
bool pointInRect(SDL_FPoint pt, SDL_FRect rect);

// Structs
struct Card {
	SDL_Texture* texture;
	SDL_FPoint   position;
	SDL_FPoint   targetPosition;
	SDL_FRect    rect;
};

struct IntListNode {
	int val;
	struct IntListNode *prev;
	struct IntListNode *next;
};

struct IntListNode *addIntListNode(struct IntListNode *lastNode, int val) {
	lastNode->next = (struct IntListNode*) malloc(sizeof(struct IntListNode));
	lastNode->next->val  = val;
	lastNode->next->prev = lastNode;
	lastNode->next->next = NULL;

	return lastNode->next;
}

struct CardListNode {
	struct Card card;
	float rotation;
	struct CardListNode *prev;
	struct CardListNode *next;
};

// Enums
enum CardType {
	CARD_CIRCLE,
	CARD_CROSS,
	CARD_WAVE,
	CARD_SQUARE,
	CARD_STAR
};

// IMPLEMENTATION

float mapNumber(float n, float min0, float max0, float min1, float max1) {
	return (n - min0) / (max0 - min0) * (max1 - min1) + min1;
}

SDL_FPoint STWCoords(SDL_FPoint point, SDL_FPoint minmax) {
	return (SDL_FPoint){
		(point.x + 1) / 2 * minmax.x,
		(point.y + 1) / 2 * minmax.y
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

#endif
