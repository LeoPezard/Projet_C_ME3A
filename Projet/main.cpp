#include"header.h"



TTF_Font* font1 = NULL;
TTF_Font* font2 = NULL;
Image images[6];
int running = 1;
int hour = 0;
SDL_Color white = { 255, 255, 255, 255 };
SDL_Color black = { 0, 0, 0, 255 };
float totalProduction = 0;
double generalSatisfaction = 0;
double generalCO2 = 0;
double generalCost = 0;
float totalDemand = 0.0;
double wind = 0;
//--------------------------------------------------------------
//--------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Declarations fenetres rendu
    SDL_Window* fenetrePrincipale = NULL;
    SDL_Renderer* rendu = NULL;
    SDL_Texture* texture_fond = NULL;


    SDL_Rect destination;
    //SDL_Event evenement;

    
    int temps = 0, tempsPrecedent = 0;
    int intervalle = 10; // intervalle de temps en ms
    int direction = 1;


    Energyplant plants[6] = {
    {"Coal Power Plant", FOSSIL, 100.0, 50.0, 0.0, 80.0, 60.0, 0.7, 1, 1, 0, 425, 400, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Solar Power Plant", SOLAR, 100.0, 75.0, 0.0, 10.0, 60.0, 0.7, 1, 1, 200, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Wind Power Plant", WIND, 100.0, 90.0, 0.0, 10.0, 60.0, 0.7, 1, 1, 400, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Nuclear Power Plant", NUCLEAR, 100.0, 20.0, 0.0, 80.0, 60.0, 0.7, 1, 1, 600, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Hydraulic Power Plant", HYDRO, 100.0, 60.0, 0.0, 10.0, 60.0, 0.7, 1, 1, 800, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Battery Power Plant", BATTERY, 100.0, 95.0, 0.0, 80.0, 60.0, 0.7, 1, 1, 1000, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    }
    };

    // Initialisation de la SDL avec sortie du programme si erreur
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur sur initialisation de la SDL (%s)\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Initialisation de la TTF
    if (TTF_Init() != 0) {
        fprintf(stdout, "Erreur sur initialisation de la TTF (%s)\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    font1 = TTF_OpenFont("arial.ttf", 20); // 20 est la taille de la police de caractère
    font2 = TTF_OpenFont("arial.ttf", 30);

    // 1) Creation de la fenetre
    //-------------------------
    fenetrePrincipale = SDL_CreateWindow("superProjet", // titre
        SDL_WINDOWPOS_UNDEFINED,    // position initiale en X
        SDL_WINDOWPOS_UNDEFINED,    // position initiale en Y
        L_FENETRE,                  // Largeur en pixel
        H_FENETRE,                  // hauteur en pixel
        SDL_WINDOW_SHOWN);          // Fenetre visible

    // Test immédiat
    if (fenetrePrincipale == NULL) {
        fprintf(stderr, "Erreur sur creation fenetre: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // 2) Creation du rendu
    //--------------------
    rendu = SDL_CreateRenderer(fenetrePrincipale, -1, SDL_RENDERER_ACCELERATED);

    // Test immédiat
    if (rendu == NULL) {
        fprintf(stderr, "Impossible de charger le rendu : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // 2) Test de la librairie Image
    //-----------------------------
    texture_fond = IMG_LoadTexture(rendu, "./assets/provence.jpg");
    draw_energy_plant_widget(rendu, plants);
    // 3) creation des variables

    // Initialisation des champs des boutons
    BUTTON button1 = { {50, 100, 30, 20}, POWER_PLUS };
    BUTTON button2 = { {100, 100, 30, 20}, POWER_MINUS };
    BUTTON buttonQuit = { {L_FENETRE/2, H_FENETRE -30, 70, 30}, QUIT };
    BUTTON appButtons[4] = { button1, button2, buttonQuit, 0};
    BUTTON buttons[4][6] = {
        // Initialisation des boutons POWER_PLUS sous chaque image
        {{ {0, 0, 80, 30}, POWER_PLUS }, { {0, 0, 80, 30}, POWER_PLUS }, { {0, 0, 80, 30}, POWER_PLUS }, { {0, 0, 80, 30}, POWER_PLUS }, { {0, 0, 80, 30}, POWER_PLUS },{ {0, 0, 80, 30}, POWER_PLUS }},
        {{ {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }},
        {{ {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }},
        {{ {0, 0, 80, 30}, STORAGE_MINUS }, { {0, 0, 80, 30}, STORAGE_MINUS }, { {0, 0, 80, 30}, STORAGE_MINUS }, { {0, 0, 80, 30}, STORAGE_MINUS }, { {0, 0, 80, 30}, STORAGE_MINUS },{ {0, 0, 80, 30}, STORAGE_MINUS }}

    };
    
    char message[256];
    snprintf(message, sizeof(message), "nothing to say");
    
    bool clicked[6] = { false, false, false, false, false, false };
    int offsetSin = 0;

    int lastWindUpdateTime = 0;  // Temps de la dernière mise à jour du vent
    int windUpdateInterval = 1000;


    SDL_Rect sinusRect = { 850, 150, 300, 200 };
    int amplitude = sinusRect.h / 2 - 10;

    while (running) {
        int startTime = SDL_GetTicks();
        while (hour < 24 && running!=0) {
            if (SDL_GetTicks() - startTime >= 500) { // 1 heure <-> 500 ms ici ( a modifier)
                hour += 1;
                startTime = SDL_GetTicks();
            }
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_MOUSEMOTION:
                    handleMouseMotion(event, images, clicked);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    clickImageButtons(rendu, event, images, clicked, buttons, message, sizeof(message), white, plants);
                    clickButtonApp(rendu, event, appButtons, message, sizeof(message), white);

                    break;

                case SDL_KEYDOWN:
                    handleKeyDown(event, running);
                    break;

                default:
                    break;
                }
            }


            // Mise à jour du temps
            temps = SDL_GetTicks();
            if (temps - tempsPrecedent > intervalle) {
                tempsPrecedent = temps; // mise à jour tempsPrecedent
            }
            else {
                // Mise en pause pour laisser le CPU travailler sur d'autres tâches
                SDL_Delay(intervalle - (temps - tempsPrecedent));
            }
            
            
            // Mise à jour de l'affichage avec les fonctions liées au rendu à mettre ici
            // début de la zone d'affichage
            SDL_RenderClear(rendu);
            

            destination.x = 0;
            destination.y = 0;
            destination.h = H_FENETRE;
            destination.w = L_FENETRE;
            SDL_RenderCopy(rendu, texture_fond, NULL, &destination);

            draw_energy_plant_production(rendu, plants);
            legend_plant_production(rendu, plants, font1);
            for (int i = 0; i < 6; i++) {
                SDL_RenderCopy(rendu, images[i].texture, NULL, &images[i].rect);
            }
            for (int i = 0; i < 6; i++) {
                if (clicked[i] && (plants[i].type == FOSSIL || plants[i].type == HYDRO)) {  // Afficher les boutons uniquement si l'image correspondante est cliquée
                    for (int j = 0; j < 4; j++) {
                        draw_button(rendu, buttons[j][i]);  // Dessiner chaque bouton sous l'image
                    }
                }
                else if (clicked[i] && (plants[i].type == WIND || plants[i].type == SOLAR || plants[i].type == NUCLEAR)) {  // Afficher les boutons uniquement si l'image correspondante est cliquée
                    for (int j = 2; j < 4; j++) {
                        draw_button(rendu, buttons[j][i]);  // Dessiner chaque bouton sous l'image
                    }
                }
                else if (clicked[i] && (plants[i].type == BATTERY )) {  // Afficher les boutons uniquement si l'image correspondante est cliquée
                    for (int j = 0; j < 2; j++) {
                        draw_button(rendu, buttons[j][i]);  // Dessiner chaque bouton sous l'image
                    }
                }
            }
            
            draw_button(rendu, button1);
            draw_button(rendu, button2);
            draw_button(rendu, buttonQuit);

            // affichage du message
            SDL_Rect rect1;
            rect1.x = 10;
            rect1.y = 18;
            render_text(rendu, font1, message, white, rect1);

            char hour_text[256];
            SDL_Rect HourRect;
            HourRect.x = 300;
            HourRect.y = 20;
            snprintf(hour_text, sizeof(hour_text), "Hour : %d", hour);
            render_text(rendu, font2, hour_text, black, HourRect);
            current_production(plants);
            current_satisfaction(plants);
            current_CO2(plants);
            current_demand(hour);
            update_production_sun(&plants[1], hour);

            if (SDL_GetTicks() - lastWindUpdateTime >= windUpdateInterval) {
                create_wind();  // Créer un nouveau vent
                lastWindUpdateTime = SDL_GetTicks();  // Mettre à jour le temps de la dernière mise à jour du vent
            }
            update_production_wind(&plants[2], wind);


            display_datas(rendu);

            SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255); // Blanc
            SDL_RenderFillRect(rendu, &sinusRect);

            // Dessiner la courbe sinusoïdale dans le rectangle
            draw_sun(rendu, sinusRect, amplitude, hour);

            // Mettre à jour le décalage
            offsetSin += 1;

            SDL_RenderPresent(rendu); // fin de la zone d'affichage
            SDL_Delay(16);
        }
        hour = 0;
    }


    // Destructions
    SDL_DestroyWindow(fenetrePrincipale);
    SDL_DestroyRenderer(rendu);
    SDL_DestroyTexture(texture_fond);
    destroyImages();

    // Libération de la mémoire SDL
    SDL_Quit();
    TTF_Quit();

    return 0;
}

//--------------------------------------------------------------