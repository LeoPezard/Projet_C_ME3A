//#include "header.h"
//
//#define H_FENETRE 800
//#define L_FENETRE 1200
//int temps, tempsPrecedent;
//
//int main(int argc, char** argv)
//{
//    /* Déclaration des variables */
//
//    // Déclarations fenetres rendu 
//    SDL_Window* fenetrePrincipale = NULL, * fenetre2 = NULL;
//    SDL_Renderer* rendu = NULL, * rendu2 = NULL;
//    SDL_Texture* texture = NULL;
//    SDL_Rect destination = { 0,0,100,100 };
//
//    const SDL_Rect rect1 = { 200, 200, 100, 100 };
//
//    int continuer = 1;
//    SDL_Event evenement;
//
//    // Création de la fenêtre
//    fenetrePrincipale = SDL_CreateWindow("superProjet", 100, 100, 1000, 500, SDL_WINDOW_SHOWN); // Fenetre principale
//    fenetre2 = SDL_CreateWindow("FENETRE 2", 1000, 300, 200, 200, SDL_WINDOW_SHOWN);
//
//    rendu = SDL_CreateRenderer(fenetrePrincipale, -1, SDL_RENDERER_ACCELERATED);
//    rendu2 = SDL_CreateRenderer(fenetre2, -1, SDL_RENDERER_ACCELERATED);
//
//    if (fenetrePrincipale == NULL || fenetre2 == NULL) {
//        fprintf(stderr, "Erreur sur creation fenetre: %s\n", SDL_GetError());
//        exit(EXIT_FAILURE);
//    }
//    if (rendu == NULL || rendu2 == NULL) {
//        fprintf(stderr, "Impossible de charger le rendu : %s\n", SDL_GetError());
//        exit(EXIT_FAILURE);
//    }
//
//    /* Initialisation de la SDL avec sortie du programme si erreur */
//    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
//        fprintf(stderr, "Erreur sur initialisation de la SDL (%s)\n", SDL_GetError());
//        exit(EXIT_FAILURE);
//    }
//
//    // Position et taille du tronc du sapin
//    SDL_Rect troncSapin = { L_FENETRE / 2 - 25, 400, 50, 100 };
//
//    // Rectangle à afficher en haut de la fenêtre après clic
//    SDL_Rect rectangleHaut = { L_FENETRE / 2 - 50, 50, 100, 50 }; // Nouveau rectangle en haut de la fenêtre
//    int starX = L_FENETRE / 2;
//    int starY = 50;
//
//
//    /* Corps du jeu */
//    while (continuer) {  // boucle dont on sort seulement si continuer est mise à 0
//        SDL_PollEvent(&evenement); // récupération événement depuis la pompe 
//        switch (evenement.type) { // traitement selon événement 
//        case SDL_QUIT:
//            continuer = 0;
//            break;
//        case SDL_KEYDOWN:
//            switch (evenement.key.keysym.sym) {
//            case SDLK_ESCAPE: // touche Echap 
//                continuer = 0;
//                break;
//            case SDLK_g:      // touche g 
//                // Autres instructions à faire si appui touche ‘g’ 
//                break;
//            }
//            break; // Ajout du break pour fermer le case SDL_KEYDOWN
//        case SDL_MOUSEBUTTONDOWN:
//            if (evenement.button.button == SDL_BUTTON_LEFT) {
//                // Vérifier si le clic est à l'intérieur du tronc du sapin
//                if (evenement.button.x >= troncSapin.x && evenement.button.x <= troncSapin.x + troncSapin.w &&
//                    evenement.button.y >= troncSapin.y && evenement.button.y <= troncSapin.y + troncSapin.h) {
//                    // Action si le clic est à l'intérieur du tronc
//                    printf("Clic sur le pied du sapin !\n");
//
//                    // Dessiner le nouveau rectangle en haut de la fenêtre
//                    SDL_SetRenderDrawColor(rendu, 255, 255, 0, 255); // Couleur du rectangle (rouge)
//                    //SDL_RenderFillRect(rendu, &rectangleHaut); // Dessiner le rectangle
//                    drawStar(rendu, starX, starY, 30, 5, 5);
//                    drawCoalPlant(rendu, 100, 100, 50);
//                    /*SDL_SetRenderDrawColor(rendu, 200, 25, 100, 255);*/
//                    SDL_RenderPresent(rendu);
//                }
//            }
//            if (evenement.button.button == SDL_BUTTON_RIGHT) {
//                // Vérifier si le clic est à l'intérieur du tronc du sapin
//                continuer = 0;
//            }
//            break; // Ajout du break pour fermer le case SDL_MOUSEBUTTONDOWN
//        }
//
//        /*** Zone de création ***/
//
//        // Dessin du sapin
//        SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255); // Fond noir
//        //SDL_RenderClear(rendu);
//
//        // Branches du sapin (triangles)
//        SDL_SetRenderDrawColor(rendu, 0, 128, 0, 255); // Vert sapin
//        drawTriangle(rendu, L_FENETRE / 2, 100, L_FENETRE / 2 - 100, 200, L_FENETRE / 2 + 100, 200);
//        drawTriangle(rendu, L_FENETRE / 2, 200, L_FENETRE / 2 - 150, 300, L_FENETRE / 2 + 150, 300);
//        drawTriangle(rendu, L_FENETRE / 2, 300, L_FENETRE / 2 - 200, 400, L_FENETRE / 2 + 200, 400);
//
//        // Tronc du sapin (rectangle)
//        SDL_SetRenderDrawColor(rendu, 139, 69, 19, 255); // Marron
//        SDL_RenderFillRect(rendu, &troncSapin);
//
//        SDL_RenderPresent(rendu);
//    }
//
//    // Destruction
//    SDL_DestroyRenderer(rendu);
//    SDL_DestroyWindow(fenetrePrincipale);
//    SDL_DestroyTexture(texture);
//
//    /* Libération de la mémoire SDL */
//    SDL_Quit();
//  
//}
