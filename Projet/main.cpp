/*
 * Auteurs : Léo Morin - Léo Pezard
 * Sujet : Jeu de gestion de centrales énergétiques en C - Polytech Marseille Mécanique énergétique
 * Date : 20 janvier 2025
 * Description : Ce fichier contient le main du projet, qui permet son exécution.
 */

#include"header.h"

// Initialisation de variables externes pour éviter des bugs
TTF_Font* font1 = NULL;
TTF_Font* font2 = NULL;
TTF_Font* font3 = NULL;
Image images[6];
SDL_Color white = { 255, 255, 255, 255 };
SDL_Color black = { 0, 0, 0, 255 };
Event chosenEvent;
float totalProduction = 0;
double generalSatisfaction = 0;
double generalCO2 = 0;
double generalCost = 0;
float totalDemand = 60000.0; // demande minimale (nuit, avant 6h)
double wind = 0;
double cost = 0;
int realTime = 3000; // Temps pour 1 heure
int heuremessage = 0;
int running = 1;
int hour = 0;


//--------------------------------------------------------------
//--------------------------------------------------------------
int main(int argc, char* argv[])
{
    srand(time(NULL)); // Initialise le générateur de nombres aléatoires avec l'heure système 
    // pour obtenir des résultats différents à chaque exécution du programme

    // Declarations fenetres rendu
    SDL_Window* fenetrePrincipale = NULL;
    SDL_Renderer* rendu = NULL;
    SDL_Texture* texture_fond = NULL;
    SDL_Texture* sun = NULL;
    SDL_Texture* moon = NULL;

    // Déclaration des rectangles pour les affichages 
    SDL_Rect destination = { 0,0,L_FENETRE,H_FENETRE };
    SDL_Rect sinusRect = { 900, 225, 250, 150 };
    SDL_Rect HourRect = { 795,10 };
    SDL_Rect info1 = { 945, 30 };
    SDL_Rect info2 = { 945, 50 };
    SDL_Rect info3 = { 950, 185 };
    SDL_Rect info4 = { 50, 280 };
    SDL_Rect sunRect = { 990,230,70,60 };
    SDL_Rect moonRect = { 1000,315,50,50 };

    // Initialisation des champs des boutons
    BUTTON button1 = { {952, 203, 75, 20}, FASTER };
    BUTTON button2 = { {1032, 203, 75, 20}, SLOWER };
    BUTTON buttonQuit = { {L_FENETRE / 2, H_FENETRE - 30, 70, 30}, QUIT };
    BUTTON appButtons[3] = { button1, button2, buttonQuit};

    // Initialisation des messages 
    char message1[256], message2[35], message3[256], message4[256], hour_text[256];
    snprintf(message1, sizeof(message1), "Event of the day :");
    snprintf(message4, sizeof(message4), "");

    // Initialisation de données du jeu (intervalle = durée de temps en ms d'actualisation)
    int temps = 0, tempsPrecedent = 0, intervalle = 20, direction = 1, eventTriggeredToday = 0, 
        offsetSin = 0, minutes = 0;

    int lastWindUpdateTime = 0, lastBatteryInterval = 0;  // Temps de la dernière mise à jour du vent
    int windUpdateInterval = 2000, batteryInterval = 3000; // Intervalle pour modifier la décharge de batterie
    int amplitude = sinusRect.h / 2 - 10; // Pour le dessin du sinus (courbe de l'ensoleillement)
    // Etats des images pour gérer les transparences selon les clics de souris
    bool clicked[6] = { false, false, false, false, false, false };

    // Création des centrales
    Energyplant plants[6] = {
    {"Gas Power Plant", FOSSIL, 300.0, 28.0,28.0, 4.0, 80.0, 60.0, 0.7, 1, 1, 0, 425, 200, 200,
        {
            {{10, 630, 30, 30}, POWER_PLUS},
            {{50, 630, 30, 30}, POWER_MINUS},
            {{10, 680, 30, 30}, STORAGE_PLUS},
            {{50, 680, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Solar Power Plant", SOLAR, 50.0, 18.24,18.24, 7.5, 10.0, 60.0, 0.7, 0, 1, 200, 425, 200, 200,
        {
            {{0, 0, 0, 0}, POWER_PLUS},
            {{0, 0, 0, 0}, POWER_MINUS},
            {{210, 680, 30, 30}, STORAGE_PLUS},
            {{250, 680, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Wind Power Plant", WIND, 60.0, 18.0,18.0, 7.5, 10.0, 60.0, 0.7, 0, 1, 400, 425, 200, 200,
        {
            {{0, 0, 0, 0}, POWER_PLUS},
            {{0, 0, 0, 0}, POWER_MINUS},
            {{410, 680, 30, 30}, STORAGE_PLUS},
            {{450, 680, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Nuclear Power Plant", NUCLEAR, 200.0, 120.0,120.0, 8.0, 80.0, 60.0, 0.7, 0, 1, 600, 425, 200, 200,
        {
            {{0, 0, 0, 0}, POWER_PLUS},
            {{0, 0, 0, 0}, POWER_MINUS},
            {{610, 680, 30, 30}, STORAGE_PLUS},
            {{650, 680, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Hydraulic Power Plant", HYDRO, 100.0, 48.0,48.0, 8.0, 10.0, 60.0, 0.7, 1, 1, 800, 425, 200, 200,
        {
            {{810, 630, 30, 30}, POWER_PLUS},
            {{850, 630, 30, 30}, POWER_MINUS},
            {{810, 680, 30, 30}, STORAGE_PLUS},
            {{850, 680, 30, 30}, STORAGE_MINUS}
        },
    },
    {"Battery Power Plant", BATTERY, 50.0, 0.0,5.0, 6.0, 0.0, 60.0, 0.7, 1, 0, 1000, 425, 200, 200,
        {
            {{1010, 630, 30, 30}, POWER_PLUS},
            {{1050, 630, 30, 30}, POWER_MINUS},
            {{1010, 680, 0, 0}, STORAGE_PLUS},
            {{1050, 680, 0, 0}, STORAGE_MINUS},
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
    // Initialisation des polices utilisées (a mettre après initialisation de la TTF sinon ne marche pas)
    font1 = TTF_OpenFont("arial.ttf", 30); // 30 est la taille de la police de caractère
    font2 = TTF_OpenFont("arial.ttf", 20);
    font3 = TTF_OpenFont("arial.ttf", 15);


    // Creation de la fenetre
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

    // Creation du rendu
    //--------------------
    rendu = SDL_CreateRenderer(fenetrePrincipale, -1, SDL_RENDERER_ACCELERATED);

    // Test immédiat
    if (rendu == NULL) {
        fprintf(stderr, "Impossible de charger le rendu : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // Création des évènements possibles
    Event events[5] = {
        {"Major sporting", INCREASE, 50, 20,22, IMG_LoadTexture(rendu, "./assets/match.png")},
        {"Cold wave", INCREASE, 60, 10,23, IMG_LoadTexture(rendu, "./assets/cold.png")},
        {"Holiday season", DECREASE, 70, 1,23, IMG_LoadTexture(rendu, "./assets/holidays.png")},
        {"Festival", INCREASE, 20, 10,23, IMG_LoadTexture(rendu, "./assets/musique.png")},
        {"Mass outdoor event", DECREASE, 35, 12,19,IMG_LoadTexture(rendu, "./assets/marathon.png")}
    };
    // Test de la librairie Image
    //-----------------------------
    // 
    // La fonction load_image renvoie -1 si l'image ne peux pas être chargée, sinon elle attribue au pointeur 
    // en paramètre la valeur de son chargement d'image (voir fonctions.cpp)

    // Chargement des tertures principales
    if (load_image(rendu, "./assets/background.png", &texture_fond) == -1 ||
        load_image(rendu, "./assets/sun.png", &sun) == -1 ||
        load_image(rendu, "./assets/moon.png", &moon) == -1) {
        printf("Erreur de chargement de l'image %s: \n", SDL_GetError());
        exit(EXIT_FAILURE); // Quitter si une image ne peut pas être chargée
    }
    // Vérification du chargement des images avant les modifications des centrales
    for (int i = 0; i < 6; i++) {
        char path[128];
        snprintf(path, sizeof(path), "./assets/%s", imageList[i]);
        if (load_image(rendu, path, &images[i].texture) != 0) {
            exit(EXIT_FAILURE); // Erreur de chargement
        }
    }
    // Attribuer la position des cantrales sur la fenetre
    position_energy_plant_widget(rendu, plants);
   
    // Boucle du jeu
    while (running) {
        int startTime = SDL_GetTicks();
        // Création d'un évènement par jour
        create_event(events, sizeof(events) / sizeof(events[0]),message2, message3, sizeof(message3));
        while (hour < 24 && running != 0) {
            // Modifier la valeur des minutes et des heures selon realTime qui est le temps réel pour 1h sur le jeu
            if (SDL_GetTicks() - startTime >= realTime / 60) {
                minutes += 1;
                startTime = SDL_GetTicks();
                if (minutes > 60) {
                    minutes = 0;
                    hour++;
                }
            }
            // Gestion des évènements, les fonctions de fonctions2.cpp sont appelées 
            // car liées spécialement aux évènements de souris et clavier
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_QUIT: // Fermeture de la fenêtre
                    running = 0;
                    break;

                case SDL_MOUSEMOTION: // Mouvement de souris
                    handleMouseMotion(event, images, clicked);
                    break;

                case SDL_MOUSEBUTTONDOWN: // Clic de souris
                    clickImageButtons(rendu, event, images, clicked, message4, sizeof(message4), white, plants);
                    clickButtonApp(rendu, event, appButtons, message4, sizeof(message4), white,realTime, running);
                    heuremessage = hour;
                    break;

                case SDL_KEYDOWN: // Evènement de clavier
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

            // Affichage de l'image de fond
            SDL_RenderCopy(rendu, texture_fond, NULL, &destination);
            // Modifie le fond selon l'heure de la journée
            update_background(texture_fond, hour);
            // Dessin des centrales énergétiques (jauge, légende puis image et boutons)
            // Jauges dessinées volontairement derrière les images des centrales
            legend_plant_production(rendu, plants, font2); // Texte au dessus de chaque centrale
            draw_energy_plant_production(rendu, plants); // Jauges
            draw_central_and_buttons(rendu, plants, clicked); // Images et boutons selon la condition clicked
            
            // Boutons de l'application et évènement du jour
            draw_events(rendu, chosenEvent);
            draw_button(rendu, button1);
            draw_button(rendu, button2);
            draw_button(rendu, buttonQuit);

            // Effacer le message d'information au bout de 2heures
            if (hour - heuremessage == 2) {
                snprintf(message4, sizeof(message4), "");
                
            }

            // Affichage des messages d'information
            render_text(rendu, font2, message1, black, info1);
            render_text(rendu, font2, message2, black, info2);
            render_text(rendu, font3, message3, white, info3);
            render_text(rendu, font1, message4, black, info4);
            render_text(rendu, font1, hour_text, black, HourRect);
            // Mise à jour de l'affichage de l'heure
            snprintf(hour_text, sizeof(hour_text), "%d : %d", hour, minutes);
            
            // Intervalle pour modifier la valeur du vent
            if (SDL_GetTicks() - lastWindUpdateTime >= windUpdateInterval) { 
                create_wind();  // Créer un nouveau vent
                lastWindUpdateTime = SDL_GetTicks();  // Mettre à jour le temps de la dernière mise à jour du vent
            }
            // Intervalle pour modifier la valeur de la batterie (décharge)
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
            // Dessin de la courbe de l'ensoleillement
            draw_sun(rendu, sinusRect, amplitude, hour, sun,moon, sunRect, moonRect);
            
            // Mettre à jour le décalage pour la courbe d'ensoleillement
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
    destroy_images();

    // Libération de la mémoire SDL
    SDL_Quit();
    TTF_Quit();

    return 0;
}

//--------------------------------------------------------------
