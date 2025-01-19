#include "header.h"

const char* imageList[] = {
	"gaz.png",
	"solaire.png",
	"eolienne.png",
	"nucleaire.png",
	"hydro.png",
	"batterie.png",
	"charbon.png",
};
const struct {
	enum EnergyType type;
	double co2_factor;
	double cost_factor;
} PLANT_FACTORS[] = {
	{FOSSIL, 418.0, 40.0},
	{SOLAR, 43.9, 75.0},
	{WIND, 14.1, 60.0},
	{HYDRO, 6.0, 20.0},
	{NUCLEAR, 10.0, 70.0},
	{BATTERY, 0.0, 150.0}
};
SDL_Color getColorFromRatio(float ratio) {
	SDL_Color color;

	if (ratio >= 0.8) {
		color.r = 255; color.g = 0; color.b = 0; color.a = 255; // Rouge
	}
	else if (ratio >= 0.7) {
		color.r = 255; color.g = 165; color.b = 0; color.a = 255; // Orange
	}
	else if (ratio >= 0.6) {
		color.r = 255; color.g = 255; color.b = 0; color.a = 255; // Jaune
	}
	else if (ratio >= 0.3) {
		color.r = 173; color.g = 255; color.b = 47; color.a = 255; // Vert jaune
	}
	else {
		color.r = 0; color.g = 255; color.b = 0; color.a = 255; // Vert
	}

	return color;
}


void update_production(Energyplant* plant, enum Buttontype buttontype, Energyplant plants[6], SDL_Renderer* renderer) {
	switch (buttontype) {
	case POWER_PLUS:
		if (plant->currentProduction < plant->maximumProduction) {
			switch (plant->type) {
			case FOSSIL:
			case HYDRO:
				plant->currentProduction += (5.0 / 100) * plant->maximumProduction;
				animate_lightning(renderer, plant->x + plant->width / 2, plant->y, 600, 300, 100);
				break;
			case BATTERY:
				if (plant->storageRatio >= 5.0) {
					plant->currentProduction += (5.0 / 100) * plant->maximumProduction;
					plant->storageRatio -= (5.0 / 100) * plant->storageRatio;
					animate_lightning(renderer, plant->x + plant->width / 2, plant->y, 600, 300, 100);
				}
				break;
			}
		}
		break;

	case POWER_MINUS:
		plant->currentProduction -= (5.0 / 100) * plant->maximumProduction;
		break;

	case STORAGE_PLUS:
		if (plant->currentProduction >= (5.0 / 100) * plant->maximumProduction) {
			if (plants[5].storageRatio < 100.0) {
				plant->currentProduction -= (5.0 / 100) * plant->maximumProduction;
				plants[5].storageRatio += (5.0 / 100) * plant->maximumProduction;

				animate_lightning(renderer, plant->x + plant->width / 2, plant->y + plant->height,
					plant->x + plant->width / 2, plant->y + plant->height + 100, 50);
				animate_lightning(renderer, plant->x + plant->width / 2, plant->y + plant->height + 100,
					plants[5].x + plants[5].width / 2, plants[5].y + plants[5].height + 100, 100);

				if (plants[5].storageRatio >= 100.0) {
					plants[5].storageRatio = 100.0;
				}
			}
		}
		break;

	case STORAGE_MINUS:
		if (plant->currentProduction >= (5.0 / 100) * plant->maximumProduction) {
			if (plant->currentProduction < plant->initialProduction) {
				plant->currentProduction += (5.0 / 100) * plant->maximumProduction;
			}
		}
		break;
	}
	// Vérifications finales des limites
	plant->currentProduction = fmax(0.0, fmin(plant->currentProduction, plant->maximumProduction));
}

void update_co2_and_cost(Energyplant plants[6]) {
	for (int i = 0; i < 6; i++) {
		for (const auto& factor : PLANT_FACTORS) {
			if (plants[i].type == factor.type) {
				plants[i].co2 = factor.co2_factor * plants[i].currentProduction;
				plants[i].cost = factor.cost_factor * plants[i].currentProduction;
				break;
			}
		}
	}
}
void update_production_sun_and_wind(Energyplant* solarPlant,Energyplant* windPlant,  int currentHour) { // Elle marche
	solarPlant->currentProduction = 50.0f * fmax(0.0f, sin(currentHour * PI / 12.0f - PI / 2.0f));
	windPlant->currentProduction = 60.0f * (wind / 100);
}
void update_battery(Energyplant* plant) {
	if (plant->currentProduction != 0.0) { // Si la batterie produit de l'énergie
		plant->storageRatio = fmax(0.0, fmin(plant->storageRatio - 3.0, 100.0)); // Limite entre 0 et 100% de batterie
	}
	if (plant->storageRatio == 0.0) {
		plant->currentProduction = 0.0;
	}
}

float demand_at(int hour) {
	static const struct {
		int start_hour;
		int end_hour;
		float base_demand;
		float rate;
	} DEMAND_PERIODS[] = {
		{4, 12, 50000.0, 3125.0},   // Morning rise
		{12, 16, 75000.0, 0.0},     // Day plateau
		{16, 20, 75000.0, 2500.0},  // Evening peak
		{20, 23, 85000.0, -3000.0}, // Evening decline
		{23, 4, 60000.0, 0.0}       // Night stable
	};

	hour = hour % 24;
	float demande = 60000.0; // Default night value

	for (const auto& period : DEMAND_PERIODS) {
		if (hour > period.start_hour && hour <= period.end_hour) {
			demande = period.base_demand + (hour - period.start_hour) * period.rate;
			break;
		}
	}

	return (demande / 11.4) / 24.0;
}
float future_demand(int hour, int delta) {
	int futureHour = (hour + delta) % 24; // assure que l'heure reste entre 0 et 23
	return demand_at(futureHour); // demande future
}
float current_demand(int hour) {
	totalDemand = demand_at(hour); // demande actuelle
	return totalDemand;
}
double current_CO2(Energyplant plants[6]) {
	generalCO2 = 0;
	for (int i = 0; i < 6; i++) {
		generalCO2 += plants[i].co2;
	}
	return generalCO2;
}
double current_cost(Energyplant plants[6]) {
	cost = 0;
	for (int i = 0; i < 6; i++) {
		cost += plants[i].cost;
	}
	return cost;
}
float current_production(Energyplant plants[6]) {
	totalProduction = 0.0;
	for (int i = 0; i < 6; i++) {
		totalProduction += plants[i].currentProduction;
	}
	return totalProduction;
}
double current_satisfaction(Energyplant plants[6]) {
	generalSatisfaction = 0.0;


	// Vérification de la production totale pour éviter une division par zéro
	if (totalProduction <= 0.0) {
		return 0.0; // Satisfaction nulle si aucune production
	}

	for (int i = 0; i < 6; i++) {
		switch (plants[i].type) {
		case FOSSIL:
			plants[i].currentSatisfaction = (plants[i].currentProduction / plants[i].maximumProduction) * 0.6; // Réduction pour les fossiles

			break;
		case SOLAR:
		case WIND:
		case HYDRO:
			plants[i].currentSatisfaction = (plants[i].currentProduction / plants[i].maximumProduction) * 1.2; // Bonus pour les renouvelables

			break;
		case NUCLEAR:
			plants[i].currentSatisfaction = (plants[i].currentProduction / plants[i].maximumProduction) * 0.9; // Légère réduction pour le nucléaire

			break;
		}
		// Ajouter la satisfaction actuelle à la satisfaction globale
		generalSatisfaction += plants[i].currentSatisfaction;
	}
	if (totalDemand > totalProduction) {
		// Si la demande est supérieure à la production, pénalité proportionnelle accrue
		generalSatisfaction -= (totalDemand - totalProduction) / totalDemand * 15.0;
	}
	else {
		// Si la production dépasse la demande, appliquer des ajustements pour la surproduction
		double excessProductionRatio = (totalProduction - totalDemand) / totalDemand;
		if (excessProductionRatio > 0.3) {
			// Malus plus sévère pour une surproduction importante
			generalSatisfaction -= log10(1 + excessProductionRatio) * 20.0;
		}
		else {
			// Ajustement plus doux pour une surproduction légère
			generalSatisfaction += excessProductionRatio * 1.5; // bonus
		}
	}

	// S'assurer que la satisfaction reste dans les limites [0, 10]
	generalSatisfaction = (int)round(fmax(0.0, fmin(generalSatisfaction, 10.0)));

	return generalSatisfaction;
}


void update_current_params(Energyplant plants[6], Energyplant* solarPlant, Energyplant* windPlant) {
	update_co2_and_cost(plants); // modifie le co2 et cout de chaque centrale
	update_production_sun_and_wind(solarPlant, windPlant, hour);
	current_cost(plants); // calcule le cout total 
	current_CO2(plants); // calcule le co2 total
	current_demand(hour); // renvoie la demande à l'heure actuelle
	current_production(plants); // Renvoie la production totale (somme des centrales)
	current_satisfaction(plants); // Calcule la satisfaction avec des facteurs selon le type d'énergie
	// Pas le vent car on le crée à des intervalles réguliers + longs
}
void update_background(SDL_Texture* texture_fond, int hour) {
	if (hour >= 7 && hour < 19) {
		SDL_SetTextureAlphaMod(texture_fond, 255); // Opacité maximale (jour)
	}
	else {
		SDL_SetTextureAlphaMod(texture_fond, 100); // Opacité réduite (nuit)
	}
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
void create_event(Event events[], int event_count, float* totalDemand, int hour,
	char message[], char message3[], size_t messageSize) {
	// Choisir un événement aléatoire
	int eventIndex = rand() % event_count;
	chosenEvent = events[eventIndex];

	// Vérifier si l'événement est applicable à l'heure actuelle
	if (hour >= chosenEvent.startHour && hour <= chosenEvent.endHour) {
		// Appliquer l'augmentation/diminution à totalDemand
		if (chosenEvent.type == INCREASE) {
			*totalDemand += chosenEvent.value;
		}
		else if (chosenEvent.type == DECREASE) {
			*totalDemand -= chosenEvent.value;
		}
	}snprintf(message, messageSize, "%s", chosenEvent.name);
	const char* typeText = (chosenEvent.type == INCREASE) ? "Increase" : "Decrease";
	snprintf(message3, messageSize, "%s from %d to %d", typeText, chosenEvent.startHour, chosenEvent.endHour);
	
}

void draw_events(SDL_Renderer* renderer, Event chosenEvent) {
	SDL_Rect destRect = { 920, 25, 230, 160 }; // Position et taille de l'image
	if (chosenEvent.image == NULL) {
		printf("Erreur de chargement de l'image \n");
		exit(EXIT_FAILURE);
	}
	SDL_RenderCopy(renderer, chosenEvent.image, NULL, &destRect);
}
void draw_button(SDL_Renderer* renderer, BUTTON button) {
	// Définir les couleurs en fonction du type de bouton
	SDL_Color buttonColor = { 100, 100, 100, 255 }; // Couleur par défaut

	switch (button.type) {
	case POWER_PLUS: buttonColor.r = 255; buttonColor.g = 0; buttonColor.b = 0; buttonColor.a = 255; // Rouge
		break;
	case POWER_MINUS: buttonColor.r = 0; buttonColor.g = 255; buttonColor.b = 0; buttonColor.a = 255; // Vert
		break;
	case QUIT: buttonColor.r = 255; buttonColor.g = 0; buttonColor.b = 0; buttonColor.a = 255; // Rouge
		break;
	case FASTER: buttonColor.r = 255; buttonColor.g = 165; buttonColor.b = 0; buttonColor.a = 255; // Orange
		break;
	case SLOWER: buttonColor.r = 173; buttonColor.g = 216; buttonColor.b = 230; buttonColor.a = 255; // Bleu clair
		break;
	default:
		break;
	}

	// Dessiner le rectangle du bouton
	SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
	SDL_RenderFillRect(renderer, &button.rect);
	draw_border(renderer, button.rect.x, button.rect.y, button.rect.w, button.rect.h);

	// Définir le texte du bouton
	const char* label = "";
	switch (button.type) {
	case POWER_PLUS:
	case STORAGE_PLUS:  label = "+"; break;
	case POWER_MINUS:
	case STORAGE_MINUS: label = "-"; break;
	case QUIT:          label = "QUIT"; break;
	case FASTER:        label = "Faster"; break;
	case SLOWER:        label = "Slower"; break;
	default: break;
	}

	// Afficher le texte au centre du bouton
	SDL_Rect textRect;
	textRect.x = button.rect.x + button.rect.w / 2 - 4;  // Centrage horizontal
	textRect.y = button.rect.y + button.rect.h / 2 - 12;  // Centrage vertical
	// Ajuster la position pour les textes longs
	if (button.type == QUIT || button.type == FASTER || button.type == SLOWER) {
		textRect.x = button.rect.x + 13;
		textRect.y = button.rect.y + (button.rect.h / 2 -12);
	}
	render_text(renderer, font2, label, black, textRect);
}
void draw_sun(SDL_Renderer* renderer, SDL_Rect sinusRect, int amplitude, int currentHour, SDL_Texture* sunTexture, SDL_Texture* moonTexture, SDL_Rect sunRect, SDL_Rect moonRect) {
	// Définir les couleurs pour les fonds
	draw_gauge(renderer, sinusRect.x, sinusRect.y, sinusRect.w, sinusRect.h,1,white);
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
	SDL_RenderDrawLine(renderer, sinusRect.x, sinusRect.y + sinusRect.h / 2, sinusRect.x + sinusRect.w, sinusRect.y + sinusRect.h / 2); // Axe des abscisses
	draw_border(renderer, sinusRect.x, sinusRect.y, sinusRect.w, sinusRect.h); // Bordure du contour

	// Dessiner la courbe sinusoïdale
	double displayPeriod = 12.0; // Afficher les 12 prochaines heures
	double hoursPerPixel = displayPeriod / sinusRect.w; // Conversion des pixels en heures (par rapport à la période affichée)
	for (int x = 0; x < sinusRect.w; x++) {
		double time = currentHour + x * hoursPerPixel; // Heure correspondant au pixel x
		double value = sin(SIN_FREQUENCY * (time - 7)); // Décalage pour commencer à 7h
		int y = (int)(amplitude * value);
		y = sinusRect.y + sinusRect.h / 2 - y; // Ajuster l'origine au centre du rectangle
		SDL_RenderDrawPoint(renderer, sinusRect.x + x, y);
	}

	// Dessiner les textures (soleil et lune)
	SDL_RenderCopy(renderer, sunTexture, NULL, &sunRect);
	SDL_RenderCopy(renderer, moonTexture, NULL, &moonRect);
}


void draw_central_and_buttons(SDL_Renderer* renderer, Energyplant plants[], bool clicked[]) {
	for (int i = 0; i < 6; i++) {
		SDL_RenderCopy(renderer, images[i].texture, NULL, &images[i].rect);
		if (clicked[i]) {
			for (int j = 0; j < 4; j++) {
				// Vérifie si le bouton est valide avant de le dessiner
				if (plants[i].buttons[j].rect.w > 0 && plants[i].buttons[j].rect.h > 0) {
					draw_button(renderer, plants[i].buttons[j]);
				}
			}
		}
	}
}


//Fonction qui dessine un rectangle et change sa couleur selon des paramètres
// Utilisation dans les fonctions pour tracer la production, la capacité de la batterie et les jauges de 
// demande et production (en haut à gauche)
void draw_gauge(SDL_Renderer* renderer, int x, int y, int width, int height, float ratio, SDL_Color fillColor) {
	// Dessiner le rectangle intérieur
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
	SDL_Rect fillRect = { x, y, (int)(width * ratio), height };
	SDL_RenderFillRect(renderer, &fillRect);	
}
void draw_border(SDL_Renderer* renderer, int x, int y, int w, int h) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect border = { x, y, w, h };
	SDL_RenderDrawRect(renderer, &border);
}
void draw_demand_production(SDL_Renderer* renderer, float currentDemand, float futureDemand) {
	draw_arrow(renderer, 450, 30, currentDemand <= futureDemand);
	float ratioDemand = totalDemand / 700.0f;
	float ratioProduction = totalProduction / 700.0f;

	// Jauge pour la demande
	SDL_Color demandColor = getColorFromRatio(ratioDemand);
	draw_gauge(renderer, 10, 10, 400, 25, ratioDemand, demandColor);
	draw_border(renderer, 10, 10, 400, 25);
	
	// Ajouter le texte "Demand"
	char demandText[50];
	snprintf(demandText, sizeof(demandText), "Demand %.2f MW", totalDemand);
	SDL_Surface* surfaceDemand = TTF_RenderText_Solid(font2, demandText, black);
	SDL_Texture* textureDemand = SDL_CreateTextureFromSurface(renderer, surfaceDemand);
	SDL_Rect textRectDemand = { 15, 13, surfaceDemand->w, surfaceDemand->h };
	SDL_RenderCopy(renderer, textureDemand, NULL, &textRectDemand);
	SDL_FreeSurface(surfaceDemand);
	SDL_DestroyTexture(textureDemand);

	// Jauge pour la production
	SDL_Color productionColor = getColorFromRatio(ratioProduction);
	draw_gauge(renderer, 10, 40, 400, 25, ratioProduction, productionColor);
	draw_border(renderer, 10, 40, 400, 25);
	
	// Ajouter le texte "Production"
	char productionText[50];
	snprintf(productionText, sizeof(productionText), "Production %.2f MW", totalProduction);
	SDL_Surface* surfaceProduction = TTF_RenderText_Solid(font2, productionText, black);
	SDL_Texture* textureProduction = SDL_CreateTextureFromSurface(renderer, surfaceProduction);
	SDL_Rect textRectProduction = { 15, 43, surfaceProduction->w, surfaceProduction->h };
	SDL_RenderCopy(renderer, textureProduction, NULL, &textRectProduction);
	SDL_FreeSurface(surfaceProduction);
	SDL_DestroyTexture(textureProduction);
}
void draw_energy_plant_production(SDL_Renderer* renderer, Energyplant plants[6]) {
	for (int i = 0; i < 6; i++) {
		float productionRatio = plants[i].currentProduction / plants[i].maximumProduction;
		float batteryRatio = plants[i].storageRatio / 100.0f;

		// Jauge principale
		SDL_Color productionColor = getColorFromRatio(productionRatio);

		// Calcul de la hauteur ajustée et de l'offset vertical
		int adjustedHeight = (int)(plants[i].height * productionRatio);
		int yOffset = plants[i].y +100 + plants[i].height - adjustedHeight; // La jauge commence en bas et remplit vers le haut
		int xOffset = (plants[i].type == BATTERY) ? 30 : 100;

		// Dessiner la jauge principale (production)
		draw_gauge(renderer, plants[i].x + xOffset, yOffset, 75, adjustedHeight, 1.0f, productionColor);
		draw_border(renderer, plants[i].x + xOffset,  plants[i].y + 100,  75, plants[i].height);
		
		// Si c'est une batterie, ajouter une jauge de capacité
		if (plants[i].type == BATTERY) {
			SDL_Color batteryColor = getColorFromRatio(batteryRatio);

			// Calcul de la hauteur ajustée pour la batterie
			int capacityHeight = (int)(plants[i].height * batteryRatio);
			int capacityY = plants[i].y + 100 + plants[i].height - capacityHeight; // La jauge de batterie commence aussi en bas

			// Dessiner la jauge de capacité
			draw_gauge(renderer, plants[i].x + 120, capacityY, 30, capacityHeight, 1.0f, batteryColor);
			draw_border(renderer,plants[i].x + 120,plants[i].y + 100,30,plants[i].height);
			
		}
	}
}
void draw_energy_plant_widget(SDL_Renderer* renderer, Energyplant plant[6]) {
	for (int i = 0; i < 6; i++) {
		// Définir la position et la taille des images
		images[i].rect.x = plant[i].x;      // Position X
		images[i].rect.y = plant[i].y;      // Position Y
		images[i].rect.w = plant[i].width;  // Largeur
		images[i].rect.h = plant[i].height; // Hauteur

		// Définir la transparence
		images[i].alpha = 100;  // Semi-transparent au début
		SDL_SetTextureAlphaMod(images[i].texture, images[i].alpha);		
	}
}


void draw_arrow(SDL_Renderer* renderer, int x, int y, bool up) {
	SDL_Rect arrowRect = { 440,10, 50,50 };
	SDL_Texture* arrowUp = IMG_LoadTexture(renderer, "./assets/arrowUp.png");
	SDL_Texture* arrowDown = IMG_LoadTexture(renderer, "./assets/arrowDown.png");
	if (up) {
		SDL_RenderCopy(renderer, arrowUp, NULL, &arrowRect);
	}
	else {
		SDL_RenderCopy(renderer, arrowDown, NULL, &arrowRect);
	}
}

void animate_lightning(SDL_Renderer* renderer, int startX, int startY, int endX, int endY, int durationMs) {
	// Charger l'image de l'éclair
	SDL_Texture* lightningTexture = NULL;
	if (load_image(renderer, "./assets/eclair.png", &lightningTexture) != 0) {
		return; // Arrêter si le chargement échoue
	}

	// Calcul de la vitesse en pixels par milliseconde
	float deltaX = endX - startX;
	float deltaY = endY - startY;
	int steps = durationMs / 16; // Nombre d'itérations pour ~60 FPS

	float stepX = deltaX / steps;
	float stepY = deltaY / steps;

	float currentX = startX;
	float currentY = startY;

	// Taille du rectangle de rendu
	int textureWidth = 30, textureHeight = 50;

	for (int i = 0; i <= steps; ++i) {
		// Calcul de la position pour l'affichage de l'éclair
		SDL_Rect destRect = {
			(int)currentX - textureWidth / 2,  // Centrer l'image horizontalement
			(int)currentY - textureHeight / 2, // Centrer l'image verticalement
			textureWidth,
			textureHeight
		};

		// Afficher la texture
		SDL_RenderCopy(renderer, lightningTexture, NULL, &destRect);
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
		currentX += stepX;
		currentY += stepY;
	}

	// Nettoyer la texture
	SDL_DestroyTexture(lightningTexture);
}

void display_datas(SDL_Renderer* renderer) {
	// Définir les rectangles pour chaque ligne
	SDL_Rect rect_data1 = { 62, 90, L_FENETRE, 25 };  // Ligne 1
	SDL_Rect rect_data2 = { 62, 145, L_FENETRE, 25 }; // Ligne 4
	SDL_Rect rect_data3 = { 62, 195, L_FENETRE, 25 }; // Ligne 5
	SDL_Rect rect_data4 = { 62, 250, L_FENETRE, 25 }; // Ligne 6

	// Texte pour chaque ligne
	char description1[150];
	char description2[150], description3[150], description4[150];

	// Remplir chaque chaîne avec un seul terme
	snprintf(description1, sizeof(description1), "Current CO2 emissions : %.2f Kg", generalCO2);
	snprintf(description2, sizeof(description2), "Actual wind : %.2f Km/h", wind);
	snprintf(description3, sizeof(description3), "Current cost : %.2f euros", cost);
	snprintf(description4, sizeof(description4), "Current satisfaction : %.2f/10", generalSatisfaction);

	// Afficher chaque ligne de texte séparément
	render_text(renderer, font2, description1, black, rect_data1); // Ligne 1
	render_text(renderer, font2, description2, black, rect_data2); // Ligne 4
	render_text(renderer, font2, description3, black, rect_data3); // Ligne 5
	render_text(renderer, font2, description4, black, rect_data4); // Ligne 6
}
void legend_plant_production(SDL_Renderer* renderer, Energyplant plants[6], TTF_Font* font) {
	for (int i = 0; i < 6; i++) {
		SDL_Rect rect_description = { plants[i].x + 10, plants[i].y - 15 , plants[i].width, 50 };
		char description[128];
		snprintf(description, sizeof(description), "Power : %.2f MW", plants[i].currentProduction);
		render_text(renderer, font, description, white, rect_description);
		if (plants[i].type == BATTERY) {
			SDL_Rect rect_description = { plants[i].x + 10, plants[i].y + 10 , plants[i].width, 50 };
			char description[128];
			snprintf(description, sizeof(description), "Capacity : %.2f %%", plants[i].storageRatio);
			render_text(renderer, font, description, white, rect_description);
		}
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


int load_image(SDL_Renderer* renderer, const char* imagePath, SDL_Texture** texture) {
	*texture = IMG_LoadTexture(renderer, imagePath);
	if (*texture == NULL) {
		printf("Erreur de chargement de l'image %s: %s\n", imagePath, SDL_GetError());
		return -1;
	}
	return 0;
}
void destroy_images() {
	for (int i = 0; i < 5; i++) {
		SDL_DestroyTexture(images[i].texture);
	}
}
