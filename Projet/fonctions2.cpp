/*
 * Auteurs : L�o Morin - L�o Pezard
 * Sujet : Jeu de gestion de centrales �nerg�tiques en C - Polytech Marseille M�canique �nerg�tique
 * Date : 20 janvier 2025
 * Description : Ce fichier contient les fonctions li�es aux �v�nements : Souris et clavier
 */


#include"header.h"


// Fonction qui va servir � v�rifier si la position de la souris est dans un rectangle
bool mouse_in_rect(int x, int y, SDL_Rect rect) {
	return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

// Fonction qui g�re les mouvements de la souris (pas les clics)
// Va permettre de changer la transparence des images des centrales selon la position de la souris
void handleMouseMotion(SDL_Event& event, Image images[6], bool clicked[6]) {
	// Position de la souris
	int x = event.motion.x;
	int y = event.motion.y;

	// images[i].alpha est le niveau de transparence
	for (int i = 0; i < 6; i++) {
		// Si l'image � d�j� �t� cliqu�e, la laisser opaque (alpha = 255)
		if (clicked[i]) {
			images[i].alpha = 255;
		}
		// Si les coordon�es de la souris sont dans un rectangle d'une image, la rendre opaque
		else if( mouse_in_rect(x,y,images[i].rect)) {
			images[i].alpha = 255;
		}
		else { // La rendre transparente
			images[i].alpha = 100;
		}
		// Application du niveau de transparence
		SDL_SetTextureAlphaMod(images[i].texture, images[i].alpha);
	}
}

// Fonction qui g�re les clics de la souris sur des boutons de l'application (faster, slower et quit)
void clickButtonApp(SDL_Renderer* renderer, SDL_Event& event, BUTTON appButtons[3],
	char message[], size_t messageSize, SDL_Color& white, int &realTime, int &running) {
	if (event.button.button == SDL_BUTTON_LEFT) {
		int x = event.button.x;
		int y = event.button.y;

		// V�rification des clics sur les boutons de l'app
		for (int i = 0; i < 3; i++) {
			if (mouse_in_rect(x, y, appButtons[i].rect)) {
				if (appButtons[i].type == QUIT) {
					// Running est la variable pour la boucle infinie utilis�e pour le jeu --> arr�ter le jeu
					running = 0;
				}
				// Changer le temps r�el pour 1 heure dans le jeu
				if (appButtons[i].type == FASTER) {
					realTime -= 300;	
					snprintf(message, messageSize, "Faster");
				}
				if (appButtons[i].type == SLOWER) {
					realTime += 300;
					snprintf(message, messageSize, "Slower");
				}
				// Limiter la valeur 
				realTime = fmax(300, fmin(realTime, 5000));

			}
		}
	}
}

// Fonction qui g�re les clics de la souris pour les clics sur les images des centrales
void clickImageButtons(SDL_Renderer* renderer,SDL_Event event,Image images[],bool clicked[],char message[],
	size_t messageSize,SDL_Color color,Energyplant plants[]) {
	if (event.button.button == SDL_BUTTON_LEFT) {
		int x = event.button.x;
		int y = event.button.y;

		// V�rification des clics sur les boutons des images actives en priorit�
		for (int i = 0; i < 6; i++) {
			if (clicked[i]) {  // V�rifier uniquement les boutons des images actives
				for (int j = 0; j < 4; j++) {
					if (mouse_in_rect(x, y, plants[i].buttons[j].rect)) {
						// Effectuer l'action du bouton (fonction update_production plus bas)
						// et afficher des messages d'information
						if (plants[i].buttons[j].type == POWER_PLUS) {
							snprintf(message, messageSize, "Increase in production");
						}
						else if (plants[i].buttons[j].type == POWER_MINUS) {
							snprintf(message, messageSize, "Decrease in production");
						}
						else if (plants[i].buttons[j].type == STORAGE_PLUS) {
							snprintf(message, messageSize, "Increased energy storage");
						}
						else if (plants[i].buttons[j].type == STORAGE_MINUS) {
							snprintf(message, messageSize, "Decreased energy storage");
						}
						// Fonctions dans un autre fichier qui modifie la production de chaque centrale et g�re le stockage
						// selon le type de bouton cliqu�
						update_production(&plants[i], plants[i].buttons[j].type, plants, renderer);
						return; // Sortir de la fonction apr�s avoir trait� le clic sur un bouton
					}
				}
			}
		}

		// V�rification des clics sur les images
		for (int i = 0; i < 6; i++) {
			if (mouse_in_rect(x, y, images[i].rect)) {
				// Si l'image est d�j� cliqu�e, la rendre transparente
				if (clicked[i]) {
					images[i].alpha = 100;
					clicked[i] = false;
				}
				else { // Sinon la rendre opaque
					images[i].alpha = 255;
					clicked[i] = true;
				}
			}
			else {
				images[i].alpha = 100;
				clicked[i] = false;
			}
			// Mise � jour de la transparence de l'image 
			SDL_SetTextureAlphaMod(images[i].texture, images[i].alpha);
		}
	}
}

// G�re les �v�nements de clavier
void handleKeyDown(SDL_Event& event, int &running) {
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE: // Touche �chap cliqu�e
		running = 0; // Quitter la boucle
		break;
	default:
		break;
	}
}


