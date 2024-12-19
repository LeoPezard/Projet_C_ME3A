#ifndef HEADER_H
#define HEADER_H

#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <math.h>

#define L_FENETRE 1200
#define H_FENETRE 800
#define SIN_AMPLITUDE 100
#define SIN_FREQUENCY (2*M_PI/24.0)
#define SCROLL_SPEED 2




// Enums
enum Buttontype { POWER_PLUS, POWER_MINUS, STORAGE_PLUS, STORAGE_MINUS, QUIT};
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
    double storageRatio;
    double co2;
    double cost;
    double statisfaction;
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


// Prototypes des fonctions

// Valeurs générales calculées

float current_production(Energyplant plants[6]);
double current_satisfaction(Energyplant plants[6]);
double current_CO2(Energyplant plants[6]);
float current_demand(int hour);

void create_wind();
void update_production_sun(Energyplant* plant, int currentHour);
void update_production_wind(Energyplant* plant, int currentWind);

void draw_sun(SDL_Renderer* renderer, SDL_Rect sinusRect, int amplitude, int currentHour);
void display_datas(SDL_Renderer* renderer);
void destroyImages();
void drawRectangle(SDL_Renderer* renderer, int x, int y, int width, int height);
void draw_button(SDL_Renderer* renderer, BUTTON button);
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color,
    SDL_Rect position);
bool isRectClicked(int x, int y, SDL_Rect button);
void handleMouseMotion(SDL_Event& event, Image images[6], bool clicked[6]);
void clickImageButtons(SDL_Renderer* renderer, SDL_Event& event, Image images[6],
    bool clicked[6], BUTTON buttons[4][6], char message[], size_t messageSize, SDL_Color& white,
    Energyplant plants[6]);
void clickButtonApp(SDL_Renderer* renderer, SDL_Event& event, BUTTON appButtons[4],
    char message[], size_t messageSize, SDL_Color& white);
void handleKeyDown(SDL_Event& event, int continuer);
void draw_energy_plant_widget(SDL_Renderer* renderer, Energyplant plants[6]);
void draw_energy_plant_production(SDL_Renderer* renderer, Energyplant plants[6]);

// !!!!!! Une seule plant en paramètre
void update_production(Energyplant *plant, enum Buttontype buttontype); 
void legend_plant_production(SDL_Renderer* renderer, Energyplant plant[6], TTF_Font* font1);



extern TTF_Font* font1;
extern Image images[6];
extern const char* imageList[];
extern Energyplant plants[6];
extern int selectedImage;
extern int running;
extern int hour;
extern SDL_Color white;
extern SDL_Color black;
extern float totalProduction;
extern double generalSatisfaction;
extern double generalCO2;
extern float totalDemand;
extern SDL_Texture *sunTexture;
extern SDL_Texture *moonTexture;
extern double wind;
#endif
