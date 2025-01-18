#include"header.h"


TTF_Font* font1 = NULL;
TTF_Font* font2 = NULL;
TTF_Font* font3 = NULL;
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
int heuremessage = 0;


//--------------------------------------------------------------
//--------------------------------------------------------------
int main(int argc, char* argv[])
{
    srand(time(NULL));
    // Declarations fenetres rendu
    SDL_Window* fenetrePrincipale = NULL;
    SDL_Renderer* rendu = NULL;
    SDL_Texture* texture_fond = NULL;
    SDL_Texture* sun = NULL;
    SDL_Texture* moon = NULL;


    SDL_Rect destination = { 0,0,L_FENETRE,H_FENETRE };
    SDL_Rect sinusRect = { 900, 225, 250, 150 };
    SDL_Rect HourRect = { 795,10 };
    SDL_Rect info1 = { 945, 30 };
    SDL_Rect info2 = { 945, 50 };
    SDL_Rect info3 = { 950, 185 };
    SDL_Rect info4 = { 50, 280 };
    SDL_Rect sunRect = { 990,230,70,60 };
    SDL_Rect moonRect = { 1000,315,50,50 };

    //SDL_Event evenement;


    int temps = 0, tempsPrecedent = 0, intervalle = 10, direction = 1, eventTriggeredToday = 0;
    // intervalle = durée de temps en ms d'actualisation

    Energyplant plants[6] = {
    {"Gas Power Plant", FOSSIL, 300.0, 28.0,28.0, 4.0, 80.0, 60.0, 0.7, 1, 1, 0, 425, 200, 200,
        {
            {{100, 630, 30, 30}, POWER_PLUS},
            {{350, 630, 30, 30}, POWER_MINUS},
            {{10, 655, 30, 30}, STORAGE_PLUS},
            {{250, 655, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Solar Power Plant", SOLAR, 50.0, 18.24,18.24, 7.5, 10.0, 60.0, 0.7, 0, 1, 200, 425, 200, 200,
        {
            {{0, 0, 0, 0}, POWER_PLUS},
            {{0, 0, 0, 0}, POWER_MINUS},
            {{410, 655, 30, 30}, STORAGE_PLUS},
            {{450, 655, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Wind Power Plant", WIND, 60.0, 18.0,18.0, 7.5, 10.0, 60.0, 0.7, 0, 1, 400, 425, 200, 200,
        {
            {{0, 0, 0, 0}, POWER_PLUS},
            {{0, 0, 0, 0}, POWER_MINUS},
            {{610, 655, 30, 30}, STORAGE_PLUS},
            {{650, 655, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Nuclear Power Plant", NUCLEAR, 200.0, 120.0,120.0, 8.0, 80.0, 60.0, 0.7, 0, 1, 600, 425, 200, 200,
        {
            {{0, 0, 0, 0}, POWER_PLUS},
            {{0, 0, 0, 0}, POWER_MINUS},
            {{810, 655, 30, 30}, STORAGE_PLUS},
            {{850, 655, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Hydraulic Power Plant", HYDRO, 100.0, 48.0,48.0, 8.0, 10.0, 60.0, 0.7, 1, 1, 800, 425, 200, 200,
        {
            {{10, 630, 30, 30}, POWER_PLUS},
            {{40, 630, 30, 30}, POWER_MINUS},
            {{1010, 655, 30, 30}, STORAGE_PLUS},
            {{1050, 655, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Battery Power Plant", BATTERY, 50.0, 0.0,5.0, 6.0, 0.0, 60.0, 0.7, 1, 0, 1000, 425, 200, 200,
        {
            {{10, 630, 30, 30}, POWER_PLUS},
            {{40, 630, 30, 30}, POWER_MINUS},
            {{1210, 0, 0, 0}, STORAGE_PLUS},
            {{1250, 0, 0, 0}, STORAGE_MINUS},
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

    font1 = TTF_OpenFont("arial.ttf", 30); // 20 est la taille de la police de caractère
    font2 = TTF_OpenFont("arial.ttf", 20);
    font3 = TTF_OpenFont("arial.ttf", 15);
    // 1) Creation de la fenetre
    //-------------------------
    fenetrePrincipale = SDL_CreateWindow("Projet Morin-Pezard", // titre
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
    texture_fond = IMG_LoadTexture(rendu, "./assets/background.png");
    sun = IMG_LoadTexture(rendu, "./assets/sun.png");
    moon = IMG_LoadTexture(rendu, "./assets/moon.png");
    if (texture_fond == NULL || sun == NULL || moon == NULL) {
        printf("Erreur de chargement de l'image %s: \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 6; i++) {
        char path[128];
        snprintf(path, sizeof(path), "./assets/%s", imageList[i]);
        if (load_image(rendu, path, &images[i].texture) != 0) {
            exit(EXIT_FAILURE); // Erreur de chargement
        }
    }
    draw_energy_plant_widget(rendu, plants);
    // 3) creation des variables

    // Initialisation des champs des boutons
    BUTTON button1 = { {952, 203, 75, 20}, FASTER };
    BUTTON button2 = { {1032, 203, 75, 20}, SLOWER };
    BUTTON buttonQuit = { {L_FENETRE / 2, H_FENETRE - 30, 70, 30}, QUIT };
    BUTTON appButtons[4] = { button1, button2, buttonQuit, 0 };
    

    char message1[256], message2[35], message3[256], message4[256],hour_text[256];
    snprintf(message1, sizeof(message1), "Event of the day :");
    snprintf(message4, sizeof(message4), "");

    bool clicked[6] = { false, false, false, false, false, false };
    int offsetSin = 0;

    int lastWindUpdateTime = 0, lastBatteryInterval=0;  // Temps de la dernière mise à jour du vent
    int windUpdateInterval = 2000, batteryInterval = 3000; // Intervalle pour modifier le vent

    int amplitude = sinusRect.h / 2 - 10;

    int minutes = 0;
    while (running) {
        int startTime = SDL_GetTicks();
        // Création d'un évènement par jour
        create_event(events, sizeof(events) / sizeof(events[0]), &totalDemand, hour, message2, message3, sizeof(message3));
        while (hour < 24 && running != 0) {
            
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
                    clickImageButtons(rendu, event, images, clicked, message4, sizeof(message4), white, plants);
                    clickButtonApp(rendu, event, appButtons, message4, sizeof(message4), white);
                    heuremessage = hour;

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

            SDL_RenderClear(rendu);
            // Affichage de l'image de fond et de la 
            SDL_RenderCopy(rendu, texture_fond, NULL, &destination);
            // Dessin des centrales énergétiques 
            //draw_energy_plant_widget(rendu, plants);
            for (int i = 0; i < 6; i++) {
                SDL_RenderCopy(rendu, images[i].texture, NULL, &images[i].rect);
            }

            // Leur production et leurs légendes
            draw_energy_plant_production(rendu, plants);
            legend_plant_production(rendu, plants, font2);
            // Dessin des boutons si une centrale est cliquée
            for (int i = 0; i < 6; i++) {
                if (clicked[i]) {
                    for (int j = 0; j < 4; j++) {
                        // Vérifie si le bouton est valide (ex dimensions non nulles) pour ne pas écrire les '+' et '-' dans le vide
                        // Si le bouton n'est pas valide (volontairement) on ne dessine pas 
                        // Exemple bouton d'augmentation manuelle de production du solaire
                        if (plants[i].buttons[j].rect.w > 0 && plants[i].buttons[j].rect.h > 0) {
                            draw_button(rendu, plants[i].buttons[j]);
                        }
                    }
                }
            }
            // Boutons de l'application
            draw_button(rendu, button1);
            draw_button(rendu, button2);
            draw_button(rendu, buttonQuit);

            // Effacer le message d'information au bout de 2heures
            if (hour - heuremessage == 2) {
                snprintf(message4, sizeof(message4), "");
                
            }

            // affichage du message
            draw_events(rendu, chosenEvent);
            render_text(rendu, font2, message1, black, info1);
            render_text(rendu, font2, message2, black, info2);
            render_text(rendu, font3, message3, white, info3);
            render_text(rendu, font1, message4, black, info4);
            render_text(rendu, font1, hour_text, black, HourRect);
            
            snprintf(hour_text, sizeof(hour_text), "%d : %d", hour, minutes);
            
            // Modifie le fond selon l'heure de la journée
            update_background(texture_fond, hour);
            
            if (SDL_GetTicks() - lastWindUpdateTime >= windUpdateInterval) {
                create_wind();  // Créer un nouveau vent
                lastWindUpdateTime = SDL_GetTicks();  // Mettre à jour le temps de la dernière mise à jour du vent
            }
            if (SDL_GetTicks() - lastBatteryInterval >= batteryInterval) {
                update_battery(&plants[5]);  // Changer la batterie (se décharge)
                lastBatteryInterval = SDL_GetTicks();
            }
            // Modifie tous les paramètres 
            update_current_params(plants, &plants[1], &plants[2]);
            // Affichage de la demande et de la production dans des jauges en haut à gauche de la fenêtre
            draw_demand_production(rendu, totalDemand, future_demand(hour, 2));
            // Affiche les données (en texte) sur la fenêtre
            display_datas(rendu);

            SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255); // Blanc
            SDL_RenderFillRect(rendu, &sinusRect);

            // Dessiner la courbe sinusoïdale dans le rectangle
            draw_sun(rendu, sinusRect, amplitude, hour);
            SDL_RenderCopy(rendu, sun, NULL, &sunRect);
            SDL_RenderCopy(rendu, moon, NULL, &moonRect);
            
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
