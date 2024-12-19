#include "header.h"

const char* imageList[] = {
	"charbon.png",
	"solaire.png",
	"eolienne.png",
	"nucleaire.png",
	"hydro.png",
	"batterie.png",
};

float current_demand(int hour) {
	totalDemand = 100.0;
	if (hour <= 20 && hour > 16) { // heure de pointe
		totalDemand += 100.0;
	}
	else if (hour <= 10 && hour > 6) { // matinée
		totalDemand += 20.0;
	}
	else if (hour <= 16 && hour > 10) { // pause du midi
		totalDemand += 50.0;
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
		"Current demand : %.2f MWh" 
		"Current satisfaction : %2.f/10",
		totalDemand, generalSatisfaction);
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


void update_production(SDL_Renderer* renderer, Energyplant *plant, enum Buttontype buttontype) {

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


void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
	for (int w = 0; w < radius * 2; w++) {
		for (int h = 0; h < radius * 2; h++) {
			int dx = radius - w; // distance horizontale par rapport au centre
			int dy = radius - h; // distance verticale par rapport au centre
			if ((dx * dx + dy * dy) <= (radius * radius)) {
				SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
			}
		}
	}
}

void drawRectangle(SDL_Renderer* renderer, int x, int y, int width, int height) {
	SDL_Rect rect = { x, y, width, height };
	SDL_RenderFillRect(renderer, &rect);
}

void drawStar(SDL_Renderer* renderer, int x, int y, int radius, int numPoints, int size) {
	double angle = 2 * M_PI / numPoints;
	for (int i = 0; i < numPoints; i++) {
		// Calcul des coordonnées des deux points de chaque bras de l'étoile
		int x1 = x + cos(i * angle) * radius;
		int y1 = y - sin(i * angle) * radius;

		int x2 = x + cos((i + 1) * angle) * (radius - size);
		int y2 = y - sin((i + 1) * angle) * (radius - size);

		// Dessiner la ligne entre les deux points
		SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	}
}

void drawTriangle(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int x3, int y3) {
	// Trier les sommets par ordre croissant de y (y1 <= y2 <= y3)
	if (y1 > y2) { int tmpX = x1, tmpY = y1; x1 = x2; y1 = y2; x2 = tmpX; y2 = tmpY; }
	if (y2 > y3) { int tmpX = x2, tmpY = y2; x2 = x3; y2 = y3; x3 = tmpX; y3 = tmpY; }
	if (y1 > y2) { int tmpX = x1, tmpY = y1; x1 = x2; y1 = y2; x2 = tmpX; y2 = tmpY; }

	// Dessiner la partie supérieure du triangle (de y1 à y2)
	float slope1 = (float)(x2 - x1) / (y2 - y1); // Pente de la ligne gauche
	float slope2 = (float)(x3 - x1) / (y3 - y1); // Pente de la ligne droite

	float curX1 = x1;
	float curX2 = x1;

	for (int y = y1; y <= y2; y++) {
		SDL_RenderDrawLine(renderer, (int)curX1, y, (int)curX2, y);
		curX1 += slope1;
		curX2 += slope2;
	}

	// Dessiner la partie inférieure du triangle (de y2 à y3)
	slope1 = (float)(x3 - x2) / (y3 - y2);

	curX1 = x2;

	for (int y = y2; y <= y3; y++) {
		SDL_RenderDrawLine(renderer, (int)curX1, y, (int)curX2, y);
		curX1 += slope1;
		curX2 += slope2;
	}
}

