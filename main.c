#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

#include "myUtils.h"

// Pixels
#define SCREEN_W   1280
#define SCREEN_H   720
#define SCREEN_RES (SDL_FPoint){SCREEN_W, SCREEN_H}
#define CARD_W     178
#define CARD_H     250

// -1 -> 1
#define PCARD_OFF_X  0.35
#define PCARD_REG_Y  0.50
#define PCARD_HOV_Y  0.40
#define ANSWC_REG_Y -0.50

// Degrees
#define MAX_CARD_ROT 45

// Milliseconds
#define START_DELAY  500
#define PCARDS_DELAY 500

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

	srand(time(NULL));

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
	SDL_DestroySurface(nullCardSurface);

	// #based
	SDL_Surface* circleCardSurface = SDL_LoadPNG("textures/0.png");
	SDL_Surface* crossCardSurface  = SDL_LoadPNG("textures/1.png");
	SDL_Surface* waveCardSurface   = SDL_LoadPNG("textures/2.png");
	SDL_Surface* squareCardSurface = SDL_LoadPNG("textures/3.png");
	SDL_Surface* starCardSurface   = SDL_LoadPNG("textures/4.png");
	if (!circleCardSurface) return -1;
	if (!crossCardSurface ) return -1;
	if (!waveCardSurface  ) return -1;
	if (!squareCardSurface) return -1;
	if (!starCardSurface  ) return -1;

	SDL_Texture* cardTextures[5];
	cardTextures[CARD_CIRCLE] = SDL_CreateTextureFromSurface(renderer, circleCardSurface);
	cardTextures[CARD_CROSS ] = SDL_CreateTextureFromSurface(renderer, crossCardSurface );
	cardTextures[CARD_WAVE  ] = SDL_CreateTextureFromSurface(renderer, waveCardSurface  );
	cardTextures[CARD_SQUARE] = SDL_CreateTextureFromSurface(renderer, squareCardSurface);
	cardTextures[CARD_STAR  ] = SDL_CreateTextureFromSurface(renderer, starCardSurface  );
	for (int i=0; i<5; i++) if (!cardTextures[i]) return -1;

	SDL_DestroySurface(circleCardSurface);
	SDL_DestroySurface(crossCardSurface );
	SDL_DestroySurface(waveCardSurface  );
	SDL_DestroySurface(squareCardSurface);
	SDL_DestroySurface(starCardSurface  );

	struct Card playerCards[5];
	for (int i=0; i<5; i++) {
		playerCards[i].texture = cardTextures[i];
		playerCards[i].position = STWCoords((SDL_FPoint){0, 2}, SCREEN_RES);
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
								    STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}, SCREEN_RES).y - CARD_H / 2,
								    CARD_W,
								    CARD_H
								}
							);
							if (hoveringOver) {
								playerCards[i].targetPosition.y = STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}, SCREEN_RES).y;
								// gameState = STATE_ANSWER;
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
						playerCards[i].targetPosition = STWCoords((SDL_FPoint){(i - 2) * PCARD_OFF_X, PCARD_REG_Y}, SCREEN_RES);
				if (ticks > START_DELAY) gameState = STATE_CHOOSING;
				break;
			case STATE_CHOOSING:
				// Card hover effect
				for (int i=0; i<5; i++) {
					bool hoveringOver = pointInRect(
						mousePosition,
						(SDL_FRect) {
							playerCards[i].rect.x,
							STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}, SCREEN_RES).y - CARD_H / 2,
							CARD_W,
							CARD_H
						}
					);
					if (hoveringOver) playerCards[i].targetPosition.y = STWCoords((SDL_FPoint){0.0, PCARD_HOV_Y}, SCREEN_RES).y;
					else playerCards[i].targetPosition.y = STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}, SCREEN_RES).y;
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

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

		// Background
		SDL_RenderTexture(renderer, bgTexture, NULL, NULL);

		// Render the cards
		for (int i=0; i<5; i++) SDL_RenderTexture(renderer, playerCards[i].texture, NULL, &playerCards[i].rect);

        // Render everything
        SDL_RenderPresent(renderer);
    }

    // Everything to the trash bin
	SDL_DestroyTexture(bgTexture);

	SDL_DestroyTexture(nullCardTexture);

	for (int i=0; i<5; i++) SDL_DestroyTexture(cardTextures[i]);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}
