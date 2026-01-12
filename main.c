#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#define SCREEN_W 1280
#define SCREEN_H 720
#define CARD_W 168
#define CARD_H 240
#define PCARD_OFF_X 0.35
#define PCARD_REG_Y 0.5
#define PCARD_HOV_Y 0.45
#define START_DELAY 500 // milliseconds
#define TARGET_FPS 180

// Screen To World Coordinates Mapping ((-1, 1), (-1, 1)) -> ((0, 1280), (0, 720))
SDL_FPoint STWCoords(SDL_FPoint point) {
	return (SDL_FPoint){
		(point.x - (-1)) / (1 - (-1)) * SCREEN_W,
		(point.y - (-1)) / (1 - (-1)) * SCREEN_H,
	};
}
// Screen To World Coordinates Mapping ((0, 1280), (0, 720)) -> ((-1, 1), (-1, 1))
SDL_FPoint WTSCoords(SDL_FPoint point) {
	return (SDL_FPoint){
		(point.x) / SCREEN_W * (1 + 1) - 1,
		(point.y) / SCREEN_H * (1 + 1) - 1,
	};
}
// Linear interpolation of a vector (https://en.wikipedia.org/wiki/Linear_interpolation)
SDL_FPoint lerpV(SDL_FPoint v0, SDL_FPoint v1, float t) {
	return (SDL_FPoint) {
		(1 - t) * v0.x + t * v1.x,
		(1 - t) * v0.y + t * v1.y
	};
}
// Check if the mouse is over a rectangular region
bool mouseOverRect(SDL_FPoint mousePos, SDL_FRect rect) {
	if ((mousePos.x > rect.x && mousePos.x < rect.x + rect.w) &&
		(mousePos.y > rect.y && mousePos.y < rect.y + rect.h))
		return true;
	return false;
}

struct Card {
	SDL_Texture* texture;
	SDL_FPoint position;
	SDL_FPoint targetPosition;
	SDL_FRect rect;
};

int main() {
	// Boilerplate
    bool windowShouldClose = false;

    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Hello hi",
        SCREEN_W,
        SCREEN_H,
        SDL_WINDOW_OPENGL
    );
    renderer = SDL_CreateRenderer(
        window,
        NULL
    );

    if (window == NULL || renderer == NULL) return 1;

	// Start of the actual thing 👀
	enum {
		STATE_STARTING,
		STATE_CHOOSING,
		STATE_ANSWER
	} gameState = STATE_STARTING;

	SDL_Surface* bgSurface = SDL_LoadBMP("textures/bg.bmp");
	SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
	if (!bgSurface || !bgTexture) return -1;
	SDL_DestroySurface(bgSurface);

	// #based
	SDL_Surface* circleCardSurface = SDL_LoadPNG("textures/0.png");
	SDL_Surface* crossCardSurface  = SDL_LoadPNG("textures/1.png");
	SDL_Surface* waveCardSurface   = SDL_LoadPNG("textures/2.png");
	SDL_Surface* squareCardSurface = SDL_LoadPNG("textures/3.png");
	SDL_Surface* starCardSurface   = SDL_LoadPNG("textures/4.png");
	SDL_Texture* circleCardTexture = SDL_CreateTextureFromSurface(renderer, circleCardSurface);
	SDL_Texture* crossCardTexture  = SDL_CreateTextureFromSurface(renderer, crossCardSurface );
	SDL_Texture* waveCardTexture   = SDL_CreateTextureFromSurface(renderer, waveCardSurface  );
	SDL_Texture* squareCardTexture = SDL_CreateTextureFromSurface(renderer, squareCardSurface);
	SDL_Texture* starCardTexture   = SDL_CreateTextureFromSurface(renderer, starCardSurface  );
	if (!circleCardSurface || !circleCardTexture) return -1;
	if (!crossCardSurface  || !crossCardTexture ) return -1;
	if (!waveCardSurface   || !waveCardTexture  ) return -1;
	if (!squareCardSurface || !squareCardTexture) return -1;
	if (!starCardSurface   || !starCardTexture  ) return -1;
	SDL_DestroySurface(circleCardSurface);
	SDL_DestroySurface(crossCardSurface );
	SDL_DestroySurface(waveCardSurface  );
	SDL_DestroySurface(squareCardSurface);
	SDL_DestroySurface(starCardSurface  );

	struct Card playerCards[5];
	playerCards[0].texture = circleCardTexture;
	playerCards[1].texture = crossCardTexture ;
	playerCards[2].texture = waveCardTexture  ;
	playerCards[3].texture = squareCardTexture;
	playerCards[4].texture = starCardTexture  ;
	for (int i=0; i<5; i++) {
		playerCards[i].position = STWCoords((SDL_FPoint){0, 2});
		playerCards[i].targetPosition = STWCoords((SDL_FPoint){(i - 2) * PCARD_OFF_X, PCARD_REG_Y});
		playerCards[i].rect.w = CARD_W;
		playerCards[i].rect.h = CARD_H;
		playerCards[i].rect.x = playerCards[i].position.x - CARD_W / 2;
		playerCards[i].rect.y = playerCards[i].position.y - CARD_H / 2;
	}
	
    int lastTicks = SDL_GetTicks();
    float deltaTime __attribute__((unused)) = 0;
	SDL_FPoint mousePosition = {0, 0};
    while (!windowShouldClose) {
        // Limit the FPS I think
		if (SDL_GetTicks() - lastTicks < 1000 / TARGET_FPS) continue;
		deltaTime = (SDL_GetTicks() - lastTicks) / 1000.0;
		lastTicks = SDL_GetTicks();

        // Handle the events
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					windowShouldClose = true;
					break;
				case SDL_EVENT_KEY_DOWN:
					switch (event.key.scancode) {
						case SDL_SCANCODE_ESCAPE:
							windowShouldClose = true;
							break;
						default: break;
					}
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					// Selecting a card
					if (event.button.button == 1) {
						for (int i=0; i<5; i++) {
							bool hoveringOver = mouseOverRect(
								mousePosition,
							    (SDL_FRect) {
								    playerCards[i].rect.x,
								    STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}).y - CARD_H / 2,
								    CARD_W,
								    CARD_H
								}
							);
							if (hoveringOver) {
								gameState = STATE_ANSWER;
								playerCards[i].targetPosition.y = STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}).y;
							}
						}
					}
					break;
				default: break;
			}
        }

		SDL_MouseButtonFlags mouseFlags __attribute__((unused)) = SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

		// Game state handling
		switch (gameState) {
			case STATE_STARTING:
				if (SDL_GetTicks() > START_DELAY) gameState = STATE_CHOOSING;
				break;
			case STATE_CHOOSING:
				// Card hover effect
				for (int i=0; i<5; i++) {
					bool hoveringOver = mouseOverRect(
						mousePosition,
						(SDL_FRect) {
							playerCards[i].rect.x,
							STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}).y - CARD_H / 2,
							CARD_W,
							CARD_H
						}
					);
					if (hoveringOver) playerCards[i].targetPosition.y = STWCoords((SDL_FPoint){0.0, PCARD_HOV_Y}).y;
					else playerCards[i].targetPosition.y = STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}).y;
				}
				break;
			case STATE_ANSWER:
				break;
			default: break;
		}

		if (SDL_GetTicks() > START_DELAY) {
			for (int i=0; i<5; i++) {
				playerCards[i].position = lerpV(playerCards[i].position, playerCards[i].targetPosition, 0.2);
				playerCards[i].rect.x = playerCards[i].position.x - CARD_W / 2;
				playerCards[i].rect.y = playerCards[i].position.y - CARD_H / 2;
			}
		}

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

		// Background
		SDL_RenderTexture(renderer, bgTexture, NULL, NULL);

		// Render the playing cards
		for (int i=0; i<5; i++) SDL_RenderTexture(renderer, playerCards[i].texture, NULL, &playerCards[i].rect);

        // Render everything
        SDL_RenderPresent(renderer);
    }

    // Everything to the trash bin
	SDL_DestroyTexture(bgTexture);

	SDL_DestroyTexture(circleCardTexture);
	SDL_DestroyTexture(crossCardTexture );
	SDL_DestroyTexture(waveCardTexture  );
	SDL_DestroyTexture(squareCardTexture);
	SDL_DestroyTexture(starCardTexture  );

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}
