﻿#include"header.h"



TTF_Font* font1 = NULL;
TTF_Font* font2 = NULL;
Image images[6];
int running = 1;
int hour = 0;
SDL_Color white = { 255, 255, 255, 255 };
SDL_Color black = { 0, 0, 0, 255 };
Event chosenEvent;
float totalProduction = 0;
double generalSatisfaction = 0;
double generalCO2 = 0;
double generalCost = 0;
float totalDemand = 55000.0; // demande minimale (nuit, avant 6h)
double wind = 0;
double cost = 0;
int realTime = 3000; // Temps pour 1 heure
//--------------------------------------------------------------
//--------------------------------------------------------------
int main(int argc, char* argv[])
{
    srand(time(NULL));
    // Declarations fenetres rendu
    SDL_Window* fenetrePrincipale = NULL;
    SDL_Renderer* rendu = NULL;
    SDL_Texture* texture_fond = NULL;
    SDL_Texture* tv = NULL;
    SDL_Texture* sun = NULL;
    SDL_Texture* moon = NULL;


    SDL_Rect destination = { 0,0,L_FENETRE,H_FENETRE };
    SDL_Rect tvRect = { 850,-25,350,250 };
    SDL_Rect sinusRect = { 900, 225, 250, 150 };
    SDL_Rect HourRect = { 990,135 };
    SDL_Rect info1 = { 930, 70 };
    SDL_Rect info2 = { 930, 30 };
    SDL_Rect sunRect = { 990,230,70,60 };
    SDL_Rect moonRect = { 1000,315,50,50 };

    //SDL_Event evenement;


    int temps = 0, tempsPrecedent = 0, intervalle = 10, direction = 1, eventTriggeredToday = 0;
    // intervalle = durée de temps en ms d'actualisation

    Energyplant plants[6] = {
    {"Gas Power Plant", FOSSIL, 300.0, 28.0, 4.0, 80.0, 60.0, 0.7, 1, 1, 0, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Solar Power Plant", SOLAR, 50.0, 18.24, 7.5, 10.0, 60.0, 0.7, 1, 1, 200, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Wind Power Plant", WIND, 60.0, 18.0, 7.5, 10.0, 60.0, 0.7, 1, 1, 400, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Nuclear Power Plant", NUCLEAR, 500.0, 186.6, 8.0, 80.0, 60.0, 0.7, 1, 1, 600, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Hydraulic Power Plant", HYDRO, 100.0, 48.0, 8.0, 10.0, 60.0, 0.7, 1, 1, 800, 425, 200, 200,
        {
            {{10, 10, 20, 20}, POWER_PLUS},
            {{40, 10, 20, 20}, POWER_MINUS},
            {{10, 50, 20, 20}, STORAGE_PLUS},
            {{40, 50, 20, 20}, STORAGE_MINUS}
        },
    },
    {"Battery Power Plant", BATTERY, 50.0, 25.0, 6.0, 80.0, 60.0, 0.7, 1, 1, 1000, 425, 200, 200,
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

    Event events[5] = {
        {"Major sporting", INCREASE, 50, 20,22, IMG_LoadTexture(rendu, "./assets/match.png")},
        {"Cold wave", INCREASE, 60, 10,23, IMG_LoadTexture(rendu, "./assets/cold.png")},
        {"Holiday season", DECREASE, 70, 1,23, IMG_LoadTexture(rendu, "./assets/holidays.png")},
        {"Festival", INCREASE, 20, 10,23, IMG_LoadTexture(rendu, "./assets/musique.png")},
        {"Mass outdoor event", DECREASE, 35, 12,19,IMG_LoadTexture(rendu, "./assets/marathon.png")}
    };
    // 2) Test de la librairie Image
    //-----------------------------
    texture_fond = IMG_LoadTexture(rendu, "./assets/provence.jpg");
    tv = IMG_LoadTexture(rendu, "./assets/tv.png");
    sun = IMG_LoadTexture(rendu, "./assets/sun2.png");
    moon = IMG_LoadTexture(rendu, "./assets/moon.png");
    draw_energy_plant_widget(rendu, plants);
    // 3) creation des variables

    // Initialisation des champs des boutons
    BUTTON button1 = { {955, 190, 75, 25}, FASTER };
    BUTTON button2 = { {1030, 190, 75, 25}, SLOWER };
    BUTTON buttonQuit = { {L_FENETRE / 2, H_FENETRE - 30, 70, 30}, QUIT };
    BUTTON appButtons[4] = { button1, button2, buttonQuit, 0 };
    BUTTON buttons[4][6] = {
        // Initialisation des boutons POWER_PLUS sous chaque image
        {{ {0, 0, 80, 30}, POWER_PLUS }, { {0, 0, 80, 30}, POWER_PLUS }, { {0, 0, 80, 30}, POWER_PLUS }, { {0, 0, 80, 30}, POWER_PLUS }, { {0, 0, 80, 30}, POWER_PLUS },{ {0, 0, 80, 30}, POWER_PLUS }},
        {{ {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }, { {0, 0, 80, 30}, POWER_MINUS }},
        {{ {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }, { {0, 0, 80, 30}, STORAGE_PLUS }},
        {{ {0, 0, 80, 30}, STORAGE_MINUS }, { {0, 0, 80, 30}, STORAGE_MINUS }, { {0, 0, 80, 30}, STORAGE_MINUS }, { {0, 0, 80, 30}, STORAGE_MINUS }, { {0, 0, 80, 30}, STORAGE_MINUS },{ {0, 0, 80, 30}, STORAGE_MINUS }}

    };

    char message[256], message2[35];
    snprintf(message2, sizeof(message2), "Event of the day :");

    bool clicked[6] = { false, false, false, false, false, false };
    int offsetSin = 0;

    int lastWindUpdateTime = 0;  // Temps de la dernière mise à jour du vent
    int windUpdateInterval = 2000; // Intervalle pour modifier le vent

    int amplitude = sinusRect.h / 2 - 10;

    int minutes = 0;
    while (running) {
        int startTime = SDL_GetTicks();
        // Création d'un évènement par jour
        create_event(events, sizeof(events) / sizeof(events[0]), &totalDemand, hour, message, sizeof(message));
        while (hour < 24 && running != 0) {
            //if (SDL_GetTicks() - startTime >= realTime) { // 1 heure <-> 500 ms ici ( a modifier)
            //    hour += 1;
            //    startTime = SDL_GetTicks();
            //}
            if (SDL_GetTicks() - startTime >= realTime / 60) {
                minutes += 1;
                startTime = SDL_GetTicks();
                if (minutes > 60) {
                    minutes = 0;
                    hour++;
                }
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

            SDL_RenderCopy(rendu, texture_fond, NULL, &destination);
            SDL_RenderCopy(rendu, tv, NULL, &tvRect);

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
                else if (clicked[i] && (plants[i].type == WIND || plants[i].type == SOLAR ||
                    plants[i].type == NUCLEAR)) {  // Afficher les boutons uniquement si l'image correspondante est cliquée
                    for (int j = 2; j < 4; j++) {
                        draw_button(rendu, buttons[j][i]);  // Dessiner chaque bouton sous l'image
                    }
                }
                else if (clicked[i] && (plants[i].type == BATTERY)) {  // Afficher les boutons uniquement si l'image correspondante est cliquée
                    for (int j = 0; j < 2; j++) {
                        draw_button(rendu, buttons[j][i]);  // Dessiner chaque bouton sous l'image
                    }
                }
            }

            draw_button(rendu, button1);
            draw_button(rendu, button2);
            draw_button(rendu, buttonQuit);

            // affichage du message
            draw_events(rendu, chosenEvent);
            render_text(rendu, font1, message, black, info1);
            render_text(rendu, font1, message2, black, info2);
            char hour_text[256];

            snprintf(hour_text, sizeof(hour_text), "%d : %d", hour, minutes);
            render_text(rendu, font2, hour_text, black, HourRect);
            current_production(plants);
            current_satisfaction(plants);
            update_co2(plants);
            current_CO2(plants);
            current_demand(hour);
            draw_demand_indicator(rendu, totalDemand, future_demand(hour, 2));
            update_cost(plants);
            current_cost(plants);
            update_production_sun(&plants[1], hour);
            if (SDL_GetTicks() - lastWindUpdateTime >= windUpdateInterval) {
                create_wind();  // Créer un nouveau vent
                lastWindUpdateTime = SDL_GetTicks();  // Mettre à jour le temps de la dernière mise à jour du vent
            }
            update_production_wind(&plants[2], wind);
            
            SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255); // Blanc
            SDL_RenderFillRect(rendu, &sinusRect);

            // Dessiner la courbe sinusoïdale dans le rectangle
            draw_sun(rendu, sinusRect, amplitude, hour);
            SDL_RenderCopy(rendu, sun, NULL, &sunRect);
            SDL_RenderCopy(rendu, moon, NULL, &moonRect);
            draw_demand(rendu);

            // Mettre à jour le décalage
            offsetSin += 1;
            display_datas(rendu);


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