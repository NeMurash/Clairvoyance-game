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
#define NUMBER_W   32
#define NUMBER_H   48

// -1 -> 1
#define PCARD_OFF_X       0.35
#define PCARD_REG_Y       0.50
#define PCARD_HOV_Y       0.40
#define ANSWC_REG_Y      -0.50
#define ANSWC_RAND_OFF_X  0.15

// Degrees
#define MAX_CARD_ROT 15

// Amount of cards
#define MAX_ANS_CARDS 15

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
		STATE_ANSWERED,
		STATE_ANSWER,
	} gameState = STATE_STARTING;

	SDL_Surface* bgSurface = SDL_LoadPNG("textures/bg.png");
	SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
	if (!bgSurface || !bgTexture) return -1;
	SDL_DestroySurface(bgSurface);

	SDL_Surface *numberSurfaces[10];
	SDL_Texture *numberTextures[10];
	numberSurfaces[0] = SDL_LoadPNG("textures/numbers/0.PNG");
	numberSurfaces[1] = SDL_LoadPNG("textures/numbers/1.PNG");
	numberSurfaces[2] = SDL_LoadPNG("textures/numbers/2.PNG");
	numberSurfaces[3] = SDL_LoadPNG("textures/numbers/3.PNG");
	numberSurfaces[4] = SDL_LoadPNG("textures/numbers/4.PNG");
	numberSurfaces[5] = SDL_LoadPNG("textures/numbers/5.PNG");
	numberSurfaces[6] = SDL_LoadPNG("textures/numbers/6.PNG");
	numberSurfaces[7] = SDL_LoadPNG("textures/numbers/7.PNG");
	numberSurfaces[8] = SDL_LoadPNG("textures/numbers/8.PNG");
	numberSurfaces[9] = SDL_LoadPNG("textures/numbers/9.PNG");
	for (int i=0; i<10; i++) {
		if (!numberSurfaces[i]) return -1;
		numberTextures[i] = SDL_CreateTextureFromSurface(renderer, numberSurfaces[i]);
		SDL_DestroySurface(numberSurfaces[i]);
		if (!numberTextures[i]) return -1;
	}

	SDL_Surface* nullCardSurface   = SDL_LoadPNG("textures/5.png");
	SDL_Texture* nullCardTexture   = SDL_CreateTextureFromSurface(renderer, nullCardSurface);
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

	struct CardListNode *cardFirst = (struct CardListNode*) malloc(sizeof(struct CardListNode));
	cardFirst->card = (struct Card) {
		nullCardTexture,
		STWCoords((SDL_FPoint){0, -2}, SCREEN_RES),
		STWCoords((SDL_FPoint){0, -2}, SCREEN_RES),
		(SDL_FRect) {0, 0, CARD_W, CARD_H}
	};
	cardFirst->card.targetPosition.x = STWCoords((SDL_FPoint){(float)(rand() % 100 + 1 - 50) / 100.0 * ANSWC_RAND_OFF_X, 0.0}, SCREEN_RES).x;
	cardFirst->rotation = (rand() % ((MAX_CARD_ROT * 2) + 1)) - MAX_CARD_ROT;
	cardFirst->prev = NULL;
	cardFirst->next = NULL;
	struct CardListNode *cardLast = cardFirst;

	int ansCardCount = 1;

	struct CardListNode *curr;

    int lastTicks = SDL_GetTicks();
    float deltaTime __attribute__((unused)) = 0;
	SDL_FPoint mousePosition = {0, 0};
	SDL_FRect numberRendRect = {0, 0, NUMBER_W, NUMBER_H};

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
							// Clicked a card
							if (hoveringOver) {
								playerCards[i].position.y = STWCoords((SDL_FPoint){0.0, PCARD_REG_Y}, SCREEN_RES).y;
								gameState = STATE_ANSWERED;
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
			case STATE_ANSWERED:
				cardLast->card.texture = cardTextures[rand() % 5];
				cardLast->card.targetPosition.y = STWCoords((SDL_FPoint){0, ANSWC_REG_Y}, SCREEN_RES).y;

				curr = cardLast;
				cardLast->next = (struct CardListNode*) malloc(sizeof(struct CardListNode));
				cardLast->next->card = (struct Card) {
					nullCardTexture,
					STWCoords((SDL_FPoint){0, -2}, SCREEN_RES),
					STWCoords((SDL_FPoint){0, -2}, SCREEN_RES),
					(SDL_FRect) {0, 0, CARD_W, CARD_H}
				};
				cardLast->next->card.targetPosition.x = STWCoords((SDL_FPoint){(float)(rand() % 100 + 1 - 50) / 100.0 * ANSWC_RAND_OFF_X, 0.0}, SCREEN_RES).x;
				cardLast->next->rotation = (rand() % ((MAX_CARD_ROT * 2) + 1)) - MAX_CARD_ROT;
				cardLast->next->prev = cardLast;
				cardLast->next->next = NULL;

				cardLast = cardLast->next;

				ansCardCount++;

				if (ansCardCount > MAX_ANS_CARDS) {
					curr = cardFirst->next;
					curr->prev = NULL;
					free(cardFirst);
					cardFirst = curr;
					ansCardCount--;
				}

				gameState = STATE_ANSWER;
				break;
			case STATE_ANSWER:
				gameState = STATE_CHOOSING;
				break;
			default: break;
		}

		// Update card positions
		for (int i=0; i<5; i++) {
			playerCards[i].position = lerpV(playerCards[i].position, playerCards[i].targetPosition, 0.2);
			playerCards[i].rect.x = playerCards[i].position.x - CARD_W / 2;
			playerCards[i].rect.y = playerCards[i].position.y - CARD_H / 2;
		}
		curr = cardFirst;
		while (curr) {
			curr->card.position = lerpV(curr->card.position, curr->card.targetPosition, 0.2);
			curr->card.rect.x = curr->card.position.x - CARD_W / 2;
			curr->card.rect.y = curr->card.position.y - CARD_H / 2;
			curr = curr->next;
		}

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

		// Background
		SDL_RenderTexture(renderer, bgTexture, NULL, NULL);

		// Render the cards
		for (int i=0; i<5; i++) SDL_RenderTexture(renderer, playerCards[i].texture, NULL, &playerCards[i].rect);

		// Render answer cards
		curr = cardFirst;
		while (curr) {
			SDL_RenderTextureRotated(renderer, curr->card.texture, NULL, &curr->card.rect, curr->rotation, NULL, SDL_FLIP_NONE);
			curr = curr->next;
		}

		for (int i=1; i<=10; i++) {
			numberRendRect.x = i * (NUMBER_W + 10);
		}

        // Render everything
        SDL_RenderPresent(renderer);
    }

    // Everything to the trash bin
	SDL_DestroyTexture(bgTexture);

	SDL_DestroyTexture(nullCardTexture);

	for (int i=0; i<5; i++) SDL_DestroyTexture(cardTextures[i]);

	for (int i=0; i<10; i++) SDL_DestroyTexture(numberTextures[i]);

	curr = cardFirst;
	struct CardListNode *next = cardFirst->next;
	while (curr) {
		free(curr);
		if (next) {
			curr = next;
			next = curr->next;
		}
		else {
			curr = NULL;
		}
	}

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}
