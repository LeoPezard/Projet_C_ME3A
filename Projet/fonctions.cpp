#include "header.h"

const char* imageList[] = {
	"gaz.png",
	"solaire.png",
	"eolienne.png",
	"nucleaire.png",
	"hydro.png",
	"batterie.png",
	"charbon.png",
	"tv.png"
};


void update_production(Energyplant* plant, enum Buttontype buttontype) {
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
			plant->currentProduction -= (5.0 / 100) * plant->maximumProduction;
			
		}
	}
	if (plant->currentProduction > plant->maximumProduction) {
		plant->currentProduction = plant->maximumProduction;
	}
	else if (plant->currentProduction < 0) {
		plant->currentProduction = 0;
	}
}
void update_co2(Energyplant plants[6]) {
	for (int i = 0; i < 5;i++) {
		if (plants[i].type == FOSSIL) {
			plants[i].co2 = 418 * plants[i].currentProduction; // 0.418 kg/kwh
		}
		else if (plants[i].type == SOLAR) {
			plants[i].co2 = 43.9 * plants[i].currentProduction; // 0.0439 kg/kwh
		}
		else if (plants[i].type == WIND) {
			plants[i].co2 = 14.1 * plants[i].currentProduction; // 0.0141 kg/kwh
		}
		else if (plants[i].type == HYDRO) {
			plants[i].co2 = 6 * plants[i].currentProduction; // 0.006 kg/kwh
		}
		else if (plants[i].type == NUCLEAR) {
			plants[i].co2 = 10 * plants[i].currentProduction; // 0.01 kg/kwh
		}
	}
}
void update_cost(Energyplant plants[6]) {
	for (int i = 0; i < 6; i++) {
		if (plants[i].type == FOSSIL) {
			plants[i].cost = plants[i].currentProduction * 40;
		}
		else if (plants[i].type == SOLAR) {
			plants[i].cost = plants[i].currentProduction * 75;
		}
		else if (plants[i].type == WIND) {
			plants[i].cost = plants[i].currentProduction * 60;
		}
		else if (plants[i].type == NUCLEAR) {
			plants[i].cost = plants[i].currentProduction * 70;
		}
		else if (plants[i].type == HYDRO) {
			plants[i].cost = plants[i].currentProduction * 20;
		}
		else if (plants[i].type == BATTERY) {
			plants[i].cost = plants[i].currentProduction * 150;
		}
	}
}
void update_production_sun(Energyplant* plant, int currentHour) { // Elle marche
	plant->currentProduction = 6.36f * max(0.0f, sin(currentHour * PI / 12.0f - PI / 2.0f));

}
void update_production_wind(Energyplant* plant, int currentWind) { // Elle marche
	plant->currentProduction = 6.36f * (wind / 100);
}

double current_cost(Energyplant plants[6]) {
	cost = 0;
	for (int i = 0; i < 6; i++) {
		cost += plants[i].cost;
	}
	return cost;
}
float current_demand(int hour) { // en MW (données de la france)
	if (hour >1 && hour <= 10) { // augmentation progressive jusqu'à midi
		totalDemand += (hour - 1) * 4000; // chaque heure augmente de 4000 MW
	}
	else if (hour > 10 && hour <= 16) { // stabilisation dans la journée
		totalDemand = 71000.0; // plateau autour de 71000 MW
	}
	else if (hour > 16 && hour <= 20) { // pic en soirée
		totalDemand = 75000.0; // pic maximal vers 18-20h
	}
	else if (hour > 20 || hour <= 1) { // baisse progressive la nuit
		totalDemand = 35000.0; // stabilisation la nuit
	}
	totalDemand = (totalDemand/11.4)/24.0; // convertie en donné PACA puis en MWh
	return totalDemand;
}
double current_CO2(Energyplant plants[6]) {
	generalCO2 = 0;
	for (int i = 0; i < 6; i++) {
		generalCO2 += plants[i].co2;
	}
	return generalCO2;
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
	return generalSatisfaction / 6;
}

void create_wind() { // Elle marche
	wind = (double)rand() / RAND_MAX;
	if (wind <= 0.6) {
		wind = (double)rand() / RAND_MAX * 60;
	}
	else {
		wind = 0.5 + (double)rand() / RAND_MAX * 40;
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
	else if (button.type == FASTER)
	{
		SDL_SetRenderDrawColor(renderer, 255,165, 0, 255);
	}
	else if (button.type == SLOWER)
	{
		SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);
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
	case FASTER: label = "Faster"; break;
	case SLOWER: label = "Slower"; break;
	}
	SDL_Color black = { 0, 0, 0, 255 };
	SDL_Rect textRect;
	textRect.x = button.rect.x + button.rect.w / 2 - 4; textRect.y = button.rect.y + button.rect.h / 2 - 12;
	textRect.w = 0; textRect.h = 0;
	if (label == "QUIT" || label == "Faster" || label == "Slower") {
		textRect.x = button.rect.x + 10; textRect.y = button.rect.y;
	}
	render_text(renderer, font1, label, black, textRect);
}
void draw_sun(SDL_Renderer* renderer, SDL_Rect sinusRect, int amplitude, int currentHour) {
	// Définir les couleurs pour les fonds
	if (currentHour > 7 && currentHour < 19) {
		SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Jaune clair
		SDL_Rect topRect = { sinusRect.x, sinusRect.y, sinusRect.w, sinusRect.h / 2 };
		SDL_RenderFillRect(renderer, &topRect); // Colorier la moitié supérieure
	}
	else {
		SDL_SetRenderDrawColor(renderer, 25, 25, 112, 255); // Bleu nuit
		SDL_Rect bottomRect = { sinusRect.x, sinusRect.y + sinusRect.h / 2, sinusRect.w, sinusRect.h / 2 };
		SDL_RenderFillRect(renderer, &bottomRect); // Colorier la moitié inférieure
	}

	// Dessiner les bordures du rectangle
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Noir
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y + sinusRect.h / 2,
		sinusRect.x + sinusRect.w, sinusRect.y + sinusRect.h / 2); // Ligne des abscisses
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y,
		sinusRect.x + sinusRect.w, sinusRect.y); // Bordure supérieure
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y,
		sinusRect.x, sinusRect.y + sinusRect.h); // Bordure gauche
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y + sinusRect.h,
		sinusRect.x + sinusRect.w, sinusRect.y + sinusRect.h); // Bordure inférieure
	SDL_RenderDrawLine(renderer, sinusRect.x + sinusRect.w, sinusRect.y,
		sinusRect.x + sinusRect.w, sinusRect.y + sinusRect.h); // Bordure droite

	// Dessiner la sinusoïde
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Couleur orangée (sinusoïde)
	double displayPeriod = 12.0; // afficher que les 12 prochaines heures
	double hoursPerPixel = displayPeriod / sinusRect.w; // Conversion des pixels en heures (par rapport à la période affichée)
	for (int x = 0; x < sinusRect.w; x++) {
		double time = currentHour + x * hoursPerPixel; // Heure correspondant au pixel x
		double value = sin(SIN_FREQUENCY * (time - 7)); // Décalage pour commencer à 7h
		int y = (int)(amplitude * value);
		y = sinusRect.y + sinusRect.h / 2 - y; // Ajuster l'origine au centre du rectangle
		SDL_RenderDrawPoint(renderer, sinusRect.x + x, y);
	}
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
void drawRectangle(SDL_Renderer* renderer, int x, int y, int width, int height) {
	SDL_Rect rect = { x, y, width, height };
	SDL_RenderFillRect(renderer, &rect);
}



void display_datas(SDL_Renderer*renderer){
	SDL_Rect rect_datas1 = { 50,175 , L_FENETRE, 50 };
	SDL_Rect rect_datas2 = { 50,210 , L_FENETRE, 50 };
	SDL_Rect rect_datas3 = { 50,245 , L_FENETRE, 50 };

	char description[250], description2[150], description3[150];
	snprintf(description, sizeof(description), 
		"Current CO2 emissions : %.2f Kg  "
		"Current production : %2.f MWh", 
		generalCO2, totalProduction);
	snprintf(description2, sizeof(description2),
		"Current demand : %.2f MWh  " 
		"Current satisfaction : %2.f/10",
		totalDemand, generalSatisfaction);
	snprintf(description3, sizeof(description3),
		"Actual wind %.2f Km/h  "
		" Current cost : %2.f euros",
		wind, cost);
	render_text(renderer, font1, description, black, rect_datas1);
	render_text(renderer, font1, description2, black, rect_datas2);
	render_text(renderer, font1, description3, black, rect_datas3);
	//render_text(renderer, font1, description, black, rect_datas1);

}
void legend_plant_production(SDL_Renderer* renderer, Energyplant plants[6], TTF_Font*font1) {
	for (int i = 0; i < 6; i++) {
		SDL_Rect rect_description = { plants[i].x +10, plants[i].y-15 , plants[i].width, 50};
		char description[128];
		snprintf(description, sizeof(description), "Power : %.2f MWh", plants[i].currentProduction);
		render_text(renderer, font1, description, white, rect_description);
	}

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


