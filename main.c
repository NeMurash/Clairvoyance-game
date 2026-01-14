#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#include "misc-utils.h"

// Pixels
#define SCREEN_W 1280
#define SCREEN_H 720
#define CARD_W   168
#define CARD_H   240

// -1 -> 1
#define PCARD_OFF_X  0.35
#define PCARD_REG_Y  0.50
#define PCARD_HOV_Y  0.45
#define NULLC_REG_Y -0.50

// Milliseconds
#define START_DELAY  1000
#define PCARDS_DELAY 1000
#define NULLC_DELAY  500

// Go figure
#define TARGET_FPS 180

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

	SDL_Surface* bgSurface = SDL_LoadPNG("textures/bg.png");
	SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
	if (!bgSurface || !bgTexture) return -1;
	SDL_DestroySurface(bgSurface);

	SDL_Surface* nullCardSurface   = SDL_LoadPNG("textures/5.png");
	SDL_Texture* nullCardTexture   = SDL_CreateTextureFromSurface(renderer, nullCardSurface  );
	if (!nullCardSurface   || !nullCardTexture  ) return -1;
	SDL_DestroySurface(nullCardSurface  );

	struct Card nullCard = {
		nullCardTexture,
		STWCoords((SDL_FPoint){0, -2.0}),
		STWCoords((SDL_FPoint){0, -2.0}),
		(SDL_FRect){0, 0, CARD_W, CARD_H}
	};
	nullCard.rect.x = nullCard.position.x - CARD_W / 2;
	nullCard.rect.y = nullCard.position.y - CARD_H / 2;
	
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
		playerCards[i].targetPosition = playerCards[i].position;
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
					if (event.button.button == 1 && gameState == STATE_CHOOSING) {
						for (int i=0; i<5; i++) {
							bool hoveringOver = pointInRect(
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
				Uint64 ticks = SDL_GetTicks();
				if (ticks > PCARDS_DELAY)
					for (int i=0; i<5; i++)
						playerCards[i].targetPosition = STWCoords((SDL_FPoint){(i - 2) * PCARD_OFF_X, PCARD_REG_Y});
				if (ticks > NULLC_DELAY) nullCard.targetPosition = STWCoords((SDL_FPoint){0, NULLC_REG_Y});
				if (ticks > START_DELAY) gameState = STATE_CHOOSING;
				break;
			case STATE_CHOOSING:
				// Card hover effect
				for (int i=0; i<5; i++) {
					bool hoveringOver = pointInRect(
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

		// Update card positions
		for (int i=0; i<5; i++) {
			playerCards[i].position = lerpV(playerCards[i].position, playerCards[i].targetPosition, 0.2);
			playerCards[i].rect.x = playerCards[i].position.x - CARD_W / 2;
			playerCards[i].rect.y = playerCards[i].position.y - CARD_H / 2;
		}
		nullCard.position = lerpV(nullCard.position, nullCard.targetPosition, 0.2);
		nullCard.rect.x = nullCard.position.x - CARD_W / 2;
		nullCard.rect.y = nullCard.position.y - CARD_H / 2;

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

		// Background
		SDL_RenderTexture(renderer, bgTexture, NULL, NULL);

		// Render the cards
		for (int i=0; i<5; i++) SDL_RenderTexture(renderer, playerCards[i].texture, NULL, &playerCards[i].rect);
		SDL_RenderTexture(renderer, nullCard.texture, NULL, &nullCard.rect);

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
	SDL_DestroyTexture(nullCardTexture  );

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}
