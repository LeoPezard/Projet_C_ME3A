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
			images[i].alpha = 100;
		}
		SDL_SetTextureAlphaMod(images[i].texture, images[i].alpha);
	}
}

bool isRectClicked(int x, int y, SDL_Rect rect) {
	return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

void clickButtonApp(SDL_Renderer* renderer, SDL_Event& event, BUTTON appButtons[3],
	char message[], size_t messageSize, SDL_Color& white, int &realTime, int &running) {
	if (event.button.button == SDL_BUTTON_LEFT) {
		int x = event.button.x;
		int y = event.button.y;

		// Vérification des clics sur les boutons de l'app
		for (int i = 0; i < 3; i++) {
			if (isRectClicked(x, y, appButtons[i].rect)) {
				if (appButtons[i].type == QUIT) {
					running = 0;
				}
				if (appButtons[i].type == FASTER) {
					if (realTime > 300) {
						realTime -= 300;
						if (realTime < 300) {
							realTime = 100;
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


void clickImageButtons(SDL_Renderer* renderer,SDL_Event event,Image images[],bool clicked[],char message[],
	size_t messageSize,SDL_Color color,Energyplant plants[]) {
	if (event.button.button == SDL_BUTTON_LEFT) {
		int x = event.button.x;
		int y = event.button.y;

		// Vérification des clics sur les boutons des images actives en priorité
		for (int i = 0; i < 6; i++) {
			if (clicked[i]) {  // Vérifier uniquement les boutons des images actives
				for (int j = 0; j < 4; j++) {
					if (isRectClicked(x, y, plants[i].buttons[j].rect)) {
						// Effectuer l'action du bouton
						if (plants[i].buttons[j].type == POWER_PLUS) {
							snprintf(message, messageSize, "Augmentation de la production");
						}
						else if (plants[i].buttons[j].type == POWER_MINUS) {
							snprintf(message, messageSize, "Diminution de la production");
						}
						else if (plants[i].buttons[j].type == STORAGE_PLUS) {
							snprintf(message, messageSize, "Stockage de l'energie");
						}
						else if (plants[i].buttons[j].type == STORAGE_MINUS) {
							snprintf(message, messageSize, "Reduction du stockage");
						}
						update_production(&plants[i], plants[i].buttons[j].type, plants, renderer);
						return; // Sortir de la fonction après avoir traité le clic sur un bouton
					}
				}
			}
		}

		// Vérification des clics sur les images
		for (int i = 0; i < 6; i++) {
			if (isRectClicked(x, y, images[i].rect)) {
				// Si l'image est déjà cliquée, la rendre transparente
				if (clicked[i]) {
					images[i].alpha = 100;
					clicked[i] = false;
				}
				else {
					images[i].alpha = 255;
					clicked[i] = true;

					// Positionner les boutons sous l'image cliquée
					for (int j = 0; j < 2; j++) {
						plants[i].buttons[j].rect.x = images[i].rect.x + j * (plants[i].buttons[j].rect.w + 10);
						plants[i].buttons[j].rect.y = images[i].rect.y + images[i].rect.h + 10;
					}
					for (int j = 2; j < 4; j++) {
						plants[i].buttons[j].rect.x = images[i].rect.x + (j - 2) * (plants[i].buttons[j].rect.w + 10);
						plants[i].buttons[j].rect.y = images[i].rect.y + images[i].rect.h + 50;
					}
				}
			}
			else {
				images[i].alpha = 100;
				clicked[i] = false;
			}
			SDL_SetTextureAlphaMod(images[i].texture, images[i].alpha);
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


