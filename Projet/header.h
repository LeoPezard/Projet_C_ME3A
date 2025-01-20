#ifndef HEADER_H
#define HEADER_H

/*
 * Auteurs : Léo Morin - Léo Pezard
 * Sujet : Jeu de gestion de centrales énergétiques en C - Polytech Marseille Mécanique énergétique
 * Date : 20 janvier 2025
 * Description : Ce fichier contient :
 *  - les bibliothèques nécessaires au jeu
 *  - les constantes définies (DEFINE)
 *  - les énumérations (Enum)
 *  - les structures
 *  - les prototypages des fonctions regroupées par catégories
 *  - les variables externes
 */

#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <math.h>
#include<time.h>

#define L_FENETRE 1200
#define H_FENETRE 800
#define PI 3.14
#define SIN_AMPLITUDE 100
#define SIN_FREQUENCY (2*M_PI/24.0)
#define SCROLL_SPEED 2
#define INCREASE 1
#define DECREASE -1


// Enums
enum Buttontype { POWER_PLUS, POWER_MINUS, STORAGE_PLUS, STORAGE_MINUS, QUIT, FASTER, SLOWER };
enum EnergyType { WIND, FOSSIL, SOLAR, HYDRO, BATTERY, NUCLEAR };

// Structures
typedef struct {
    SDL_Rect rect;
    enum Buttontype type;
} BUTTON;

typedef struct {
    SDL_Texture* texture;
    SDL_Rect rect;
    Uint8 alpha; // Niveau de transparence (0 = transparent, 255 = opaque)
} Image;

typedef struct
{
    char name[256];
    enum EnergyType  type;
    double maximumProduction;
    double currentProduction;
    double initialProduction;
    double currentSatisfaction;
    double storageRatio;
    double co2;
    double cost;

    int adjustable;
    int stockable;
    // Direct position and dimension fields
    int x;
    // X position of the widget
    int y;
    // Y position of the widget
    int width;  // Width of the widget
    int height; // Height of the widget
    // Buttons with relative positioning
    BUTTON buttons[4];
} Energyplant;

typedef struct
{
    char name[256];   // Nom de l'événement
    int type;   // Type ("increase" ou "decrease")
    int value;    // Valeur associée
    int startHour; // Heure de début pour cet événement
    int endHour;
    SDL_Texture* image;
} Event;

// Prototypes des fonctions

// Fonctions liées au 'dessins' d'évènements, de rectangles etc...
void draw_gauge(SDL_Renderer* renderer, int x, int y, int width, int height, float ratio, SDL_Color fillColor);
void draw_border(SDL_Renderer* renderer, int x, int y, int w, int h);
void draw_button(SDL_Renderer* renderer, BUTTON button);
void draw_central_and_buttons(SDL_Renderer* renderer, Energyplant plants[], bool clicked[]);
void draw_demand_production(SDL_Renderer* renderer, float currentDemand, float futureDemand);
void position_energy_plant_widget(SDL_Renderer* renderer, Energyplant plants[6]);
void draw_energy_plant_production(SDL_Renderer* renderer, Energyplant plants[6]);
void draw_sun(SDL_Renderer* renderer, SDL_Rect sinusRect, int amplitude, int currentHour, SDL_Texture* sunTexture,
    SDL_Texture* moonTexture, SDL_Rect sunRect, SDL_Rect moonRect);
//void draw_sun(SDL_Renderer* renderer, SDL_Rect sinusRect, int amplitude, int currentHour);
void draw_events(SDL_Renderer* renderer, Event chosenEvent);
void draw_arrow(SDL_Renderer* renderer, int x, int y, bool up);
void animate_lightning(SDL_Renderer* renderer, int startX, int startY, int endX, int endY, int durationMs);

// Fonction calculant les données générales du jeu
float current_production(Energyplant plants[6]);
double current_satisfaction(Energyplant plants[6]);
double current_CO2(Energyplant plants[6]);
float demand_at(int hour);
float current_demand(int hour);
double current_cost(Energyplant plants[6]);
float future_demand(int hour, int delta);
void demand_with_event(Event event);

// Fonctions générant des données de manière aléatoire
void create_wind();
void create_event( Event events[], int event_count,char message[], char message3[], size_t messageSize);

// Fonctions liées aux modifications de certains paramètres 
void update_production(Energyplant* plant, enum Buttontype buttontype, Energyplant plants[6], SDL_Renderer* renderer);
void update_production_sun_and_wind(Energyplant* solarPlant, Energyplant* windPlant);
void update_battery(Energyplant* plant);
void update_background(SDL_Texture* texture_fond, int hour);
void update_current_params(Energyplant plants[6], Energyplant* solarPlant, Energyplant* windPlant);
void update_co2_and_cost(Energyplant plants[6]);

// Foncitons de rendu sous forme de texte
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color,
    SDL_Rect position);
void display_datas(SDL_Renderer* renderer);
void legend_plant_production(SDL_Renderer* renderer, Energyplant plant[6], TTF_Font* font);

// Fonctions liées au évènements (clic de souris et mouvement de souris)
bool mouse_in_rect(int x, int y, SDL_Rect button);
void handleMouseMotion(SDL_Event& event, Image images[6], bool clicked[6]);
void clickImageButtons(SDL_Renderer* renderer, SDL_Event event, Image images[], bool clicked[], char message[],
    size_t messageSize, SDL_Color color, Energyplant plants[]);
void clickButtonApp(SDL_Renderer* renderer, SDL_Event& event, BUTTON appButtons[3],
    char message[], size_t messageSize, SDL_Color& white, int &realTime, int &running);
void handleKeyDown(SDL_Event& event, int &running);

// Fonctions liées aux chargements d'image et destruction
int load_image(SDL_Renderer* renderer, const char* imagePath, SDL_Texture** texture);
void destroy_images();


extern int realTime;
extern TTF_Font* font1;
extern TTF_Font* font2;
extern Image images[6];
extern const char* imageList[];
extern Energyplant plants[6];
extern Event events[5];
extern Event chosenEvent;
extern int selectedImage;
extern int running;
extern int hour;
extern SDL_Color white;
extern SDL_Color black;
extern float totalProduction;
extern double generalSatisfaction;
extern double generalCO2;
extern float totalDemand;
extern double cost;
extern SDL_Texture* sunTexture;
extern SDL_Texture* moonTexture;
extern double wind;
extern int heuremessage;


#endif
