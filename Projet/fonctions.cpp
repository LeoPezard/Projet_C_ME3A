#include "header.h"

const char* imageList[] = {
	"charbon.png",
	"solaire.png",
	"eolienne.png",
	"nucleaire.png",
	"hydro.png",
	"batterie.png",
};


void draw_sun(SDL_Renderer*renderer,SDL_Rect sinusRect,int amplitude, int currentHour) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Noir
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y + sinusRect.h / 2,
		sinusRect.x + sinusRect.w, sinusRect.y + sinusRect.h / 2);// Ligne des abscisses
	
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y ,
		sinusRect.x + sinusRect.w, sinusRect.y);// Bordure supérieure
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y,
		sinusRect.x , sinusRect.y+ sinusRect.h);//Bordure gauche
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y+sinusRect.h,
		sinusRect.x + sinusRect.w, sinusRect.y + sinusRect.h);//Bordure inférieure
	SDL_RenderDrawLine(renderer, sinusRect.x+ sinusRect.w, sinusRect.y,
		sinusRect.x + sinusRect.w, sinusRect.y + sinusRect.h);// Bordure droite
	SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Couleur soleil

	double displayPeriod = 12.0; // afficher que les 12 prochaines heures
	double hoursPerPixel = displayPeriod / sinusRect.w; // Conversion des pixels en heures (par rapport à la période affichée)
	
	// Dessiner la sinusoïde
	SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Couleur orangée (sinusoïde)
	for (int x = 0; x < sinusRect.w; x++) {
		double time = currentHour + x * hoursPerPixel; // Heure correspondant au pixel x
		double value = sin(SIN_FREQUENCY * (time - 7)); // Décalage pour commencer à 7h
		int y = (int)(amplitude * value);
		y = sinusRect.y + sinusRect.h / 2 - y; // Ajuster l'origine au centre du rectangle
		SDL_RenderDrawPoint(renderer, sinusRect.x + x, y);
	}
}

float current_demand(int hour) {
	totalDemand = 100.0;
	if (hour >6 && hour <=10) { // matinée
		totalDemand += 20.0;
	}
	else if (hour >10 && hour <=16) { // pause du midi
		totalDemand += 50.0;
	}
	else if (hour > 16 && hour < 20) { // heure de pointe
		totalDemand += 100.0;
	}
	else if(hour > 20 || hour < 6){ //nuit
		totalDemand -= 30.0;
	}
	return totalDemand;
}

void draw_energy_plant_widget(SDL_Renderer* renderer, Energyplant plant[6]) {
	for (int i = 0; i < 6; i++) {
		char path[128];
		snprintf(path, sizeof(path), "./assets/%s", imageList[i]);

		// Charger la texture
		images[i].texture = IMG_LoadTexture(renderer, path);
		if (images[i].texture == NULL) {
			printf("Erreur de chargement de l'image %s: %s\n", path, SDL_GetError());
			exit(EXIT_FAILURE);
		}

		// Définir la position et la taille des images
		images[i].rect.x = plant[i].x;  // Position X
		images[i].rect.y = plant[i].y;            // Position Y
		images[i].rect.w = plant[i].width;            // Largeur
		images[i].rect.h = plant[i].height;            // Hauteur

		// Définir la transparence
		images[i].alpha = 180;  // Semi-transparent au début
		SDL_SetTextureAlphaMod(images[i].texture, images[i].alpha);
	}

}

void draw_energy_plant_production(SDL_Renderer* renderer, Energyplant plants[6]) {
	for (int i = 0; i < 6; i++) {
		float ratio = plants[i].currentProduction / plants[i].maximumProduction;

		// Définir la couleur en fonction des paliers du ratio
		if (ratio >= 0.8) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge
		}
		else if (ratio >= 0.7) {
			SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange
		}
		else if (ratio >= 0.6) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune
		}
		else if (ratio >= 0.3) {
			SDL_SetRenderDrawColor(renderer, 173, 255, 47, 255); // Vert jaune
		}
		else {
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Vert
		}

		// Dessiner le rectangle
		int adjustedHeight = (int)(plants[i].height * ratio); // Hauteur ajustée
		int yOffset = plants[i].y + 100 + (plants[i].height - adjustedHeight); // Ajuster la position verticale

		// Dessiner le rectangle
		drawRectangle(renderer, plants[i].x + 100, yOffset, 75, adjustedHeight);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_Rect border = {
			plants[i].x + 100,        // Position x
			plants[i].y + 100,   
			75,// Position y (en haut de la bordure)
			//plants[i].width/2,          // Largeur de la bordure
			plants[i].height          // Hauteur totale (pleine bordure)
		};
		SDL_RenderDrawRect(renderer, &border);
	}
}

float current_production(Energyplant plants[6]) {
	totalProduction = 0;
	for (int i = 0; i < 6; i++) {
		totalProduction += plants[i].currentProduction;
	}
	return totalProduction;
}

double current_satisfaction(Energyplant plants[6]) {
	generalSatisfaction = 0;
	for (int i = 0; i < 6; i++) {
		generalSatisfaction += plants[i].statisfaction;
	}
	return generalSatisfaction/6;
}

double current_CO2(Energyplant plants[6]) {
	generalCO2 = 0;
	for (int i = 0; i < 6; i++) {
		generalCO2 += plants[i].co2;
	}
	return generalCO2;
}

void display_datas(SDL_Renderer*renderer){
	SDL_Rect rect_datas1 = { 50,175 , L_FENETRE, 50 };
	SDL_Rect rect_datas2 = { 50,210 , L_FENETRE, 50 };
	char description[250], description2[150];
	snprintf(description, sizeof(description), 
		"Current CO2 emissions : %.2f Kg  "
		"Current production : %2.f MWh", 
		generalCO2, totalProduction);
	snprintf(description2, sizeof(description2),
		"Current demand : %.2f MWh  " 
		"Current satisfaction : %2.f/10",
		totalDemand, wind);
	render_text(renderer, font1, description, black, rect_datas1);
	render_text(renderer, font1, description2, black, rect_datas2);
	
	//render_text(renderer, font1, description, black, rect_datas1);

}

void legend_plant_production(SDL_Renderer* renderer, Energyplant plants[6], TTF_Font*font1) {
	for (int i = 0; i < 6; i++) {
		SDL_Rect rect_description = { plants[i].x + 10, plants[i].y , plants[i].width, 50 };
		char description[128];
		snprintf(description, sizeof(description), "Power : %.2f KWh", plants[i].currentProduction);
		render_text(renderer, font1, description, white, rect_description);
	}

}

void update_production_sun(Energyplant *plant, int currentHour){
	if (currentHour<7 || currentHour>19) {
		plant->currentProduction = 0.0;
	}
	else {
		plant->currentProduction = 2 * currentHour;
	}
}
void update_production_wind(Energyplant* plant, int currentWind) {
	// 0<currentWind<1 donc *100 pour la production
	plant->currentProduction = currentWind*1000;// Supposons une production linéaire en fonction du vent
	
}

void create_wind() {
	wind = (double)rand() / RAND_MAX;
	if (wind <= 0.6) {
		wind = (double)rand() / RAND_MAX * 0.5;  // 60% de probabilité d'être entre 0 et 0.5
	}
	else {
		wind = 0.5 + (double)rand() / RAND_MAX * 0.5; // 40% de probabilité d'être entre 0.5 et 1
	}
}

void update_production(Energyplant *plant, enum Buttontype buttontype) {

	if (buttontype == POWER_PLUS) {
			if (plant->type == FOSSIL || plant->type == HYDRO ||
				plant->type == BATTERY) {
				plant->currentProduction += (5.0 / 100) * plant->maximumProduction;
				plant->co2 = (5.0 / 100) * plant->currentProduction; //  TODO : trouver relation
			}
	}
	else if (buttontype == POWER_MINUS) {
			if (plant->type == FOSSIL || plant->type == HYDRO ||
				plant->type == BATTERY) {
				plant->currentProduction -= (5.0/ 100) * plant->maximumProduction;
			}
		}
	if (plant->currentProduction > plant->maximumProduction) {
		plant->currentProduction = plant->maximumProduction;
	}
	else if (plant->currentProduction < 0) {
		plant->currentProduction = 0;
	}
}

void draw_button(SDL_Renderer* renderer, BUTTON button)
{
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	if (button.type == POWER_PLUS)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}
	else if (button.type == POWER_MINUS)
	{
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	}
	else if (button.type == QUIT)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}
	SDL_RenderFillRect(renderer, &button.rect);

	//affiche le text du bouton en fonction du type de bouton
	const char* label = "";
	switch (button.type)
	{
	case POWER_PLUS: label = "+"; break;
	case POWER_MINUS: label = "-"; break;
	case STORAGE_PLUS: label = "+"; break;
	case STORAGE_MINUS: label = "-"; break;
	case QUIT: label = "QUIT"; break;
	}
	SDL_Color black = { 0, 0, 0, 255 };
	SDL_Rect textRect;
	textRect.x = button.rect.x + button.rect.w / 2 - 4; textRect.y = button.rect.y + button.rect.h / 2 - 12;
	textRect.w = 0; textRect.h = 0;
	if (label == "QUIT") {
		textRect.x = button.rect.x +10; textRect.y = button.rect.y;

	}
	render_text(renderer, font1, label, black, textRect);
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, SDL_Rect position)
{
	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!surface) {
		printf("Erreur lors du rendu du texte : %s\n", TTF_GetError());
		return;
	}
	if (!texture) {
		printf("Erreur lors de la création de la texture du texte : %s\n", SDL_GetError());
		SDL_FreeSurface(surface);
		return;
	}

	//la ligne suivante est a commenter si on veux que le texte occupe le rectangle initial (stretch dans le rectangle)
	SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);

	SDL_RenderCopy(renderer, texture, NULL, &position);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void destroyImages() {
	for (int i = 0; i < 5; i++) {
		SDL_DestroyTexture(images[i].texture);
	}
}


void drawRectangle(SDL_Renderer* renderer, int x, int y, int width, int height) {
	SDL_Rect rect = { x, y, width, height };
	SDL_RenderFillRect(renderer, &rect);
}
