#include"header.h"

void handleMouseMotion(SDL_Event& event, Image images[6], bool clicked[6]) {
	int x = event.motion.x;
	int y = event.motion.y;

	for (int i = 0; i < 6; i++) {
		if (clicked[i]) {
			images[i].alpha = 255;
		}
		else if (x >= images[i].rect.x && x <= images[i].rect.x + images[i].rect.w &&
			y >= images[i].rect.y && y <= images[i].rect.y + images[i].rect.h) {
			images[i].alpha = 255;
		}
		else {
			images[i].alpha = 180;
		}
		SDL_SetTextureAlphaMod(images[i].texture, images[i].alpha);
	}
}

bool isRectClicked(int x, int y, SDL_Rect rect) {
	return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

void clickButtonApp(SDL_Renderer* renderer, SDL_Event& event, BUTTON appButtons[4],
	char message[], size_t messageSize, SDL_Color& white) {
	if (event.button.button == SDL_BUTTON_LEFT) {
		int x = event.button.x;
		int y = event.button.y;

		// Vérification des clics sur les boutons de l'app
		for (int i = 0; i < 5; i++) {
			if (isRectClicked(x, y, appButtons[i].rect)) {
				if (appButtons[i].type == QUIT) {
					/*snprintf(message, messageSize, "QUIT");*/
					running = 0;
				}
				if (appButtons[i].type == FASTER) {
					if (realTime > 300) {
						realTime -= 300;
						if (realTime < 300) {
							realTime = 300;
						}
					}
					snprintf(message, messageSize, "Faster");
				}

				if (appButtons[i].type == SLOWER) {
					if (realTime < 5000) {
						realTime += 300;
						if (realTime > 5000) {
							realTime = 5000;
						}
					}
					snprintf(message, messageSize, "Slower");
				}

			}
		}
	}
}



void clickImageButtons(SDL_Renderer* renderer, SDL_Event& event, Image images[6],
	bool clicked[6], BUTTON buttons[4][6], char message[], size_t messageSize, SDL_Color& white,
	Energyplant plants[6]) {
	if (event.button.button == SDL_BUTTON_LEFT) {
		int x = event.button.x;
		int y = event.button.y;

		bool imageClicked = false;

		// Vérification des clics sur les images
		for (int i = 0; i < 6; i++) {
			if (isRectClicked(x, y, images[i].rect)) {
				// Si l'image est déjà cliquée, la rendre transparente
				if (clicked[i]) {
					images[i].alpha = 180;
					clicked[i] = false;
				}
				else {
					images[i].alpha = 255;
					clicked[i] = true;
				}
				imageClicked = true;

				// Positionner les boutons sous l'image cliquée
				for (int i = 0; i < 6; i++) {
					if (isRectClicked(x, y, images[i].rect)) {
						for (int j = 0; j < 2; j++) {
							buttons[j][i].rect.x = images[i].rect.x + j * (buttons[j][i].rect.w + 10);
							buttons[j][i].rect.y = images[i].rect.y + images[i].rect.h + 10;
						}
						for (int j = 2; j < 4; j++) {
							buttons[j][i].rect.x = images[i].rect.x + (j - 2) * (buttons[j][i].rect.w + 10);
							buttons[j][i].rect.y = images[i].rect.y + images[i].rect.h + 50;
						}
					}
				}
			}
			else {
				images[i].alpha = 180;
				clicked[i] = false;
			}
			SDL_SetTextureAlphaMod(images[i].texture, images[i].alpha);
		}

		// Vérification des clics sur les boutons des images
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 4; j++) {
				if (isRectClicked(x, y, buttons[j][i].rect)) {
					if (buttons[j][i].type == POWER_PLUS) {
						snprintf(message, messageSize, "Augmentation de la production");
					}
					else if (buttons[j][i].type == POWER_MINUS) {
						snprintf(message, messageSize, "Diminution de la production");
					}
					else if (buttons[j][i].type == STORAGE_PLUS) {
						snprintf(message, messageSize, "Stockage de l'energie");
					}
					else if (buttons[j][i].type == STORAGE_MINUS) {
						snprintf(message, messageSize, "Reduction du stockage");
					}
					update_production(&plants[i], buttons[j][i].type, plants);
				}
			}
		}

		// Affichage des boutons sous l'image cliquée
		if (imageClicked) {
			for (int i = 0; i < 5; i++) {
				if (clicked[i]) {
					for (int j = 0; j < 4; j++) {
						draw_button(renderer, buttons[j][i]);
					}
				}
			}
		}
	}
}

void handleKeyDown(SDL_Event& event, int continuer) {
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
		continuer = 0; // Quitter la boucle
		break;
	case SDLK_g:
		printf("Touche 'g' détectée\n");
		// Ajouter ici la logique pour la touche 'g'
		break;
	default:
		printf("Touche non gérée : %d\n", event.key.keysym.sym);
		break;
	}
}


