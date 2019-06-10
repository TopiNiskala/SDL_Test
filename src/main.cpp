//------------------------------------------------------------------------------------------
//HEADER

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

//------------------------------------------------------------------------------------------
//DECLARATIONS

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Key press surfaces constants
enum KeyPressSurfaces {
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_SHIFT,
	KEY_PRESS_SURFACE_TOTAL
};

bool init(); 						//Start SDL and create window
bool loadMedia();					//Load media
void close();						//Free media and shut down SDL
SDL_Surface* loadSurface(std::string path);		//Load individual image
SDL_Window* gWindow = NULL;				//The rendering window
SDL_Surface* gScreenSurface = NULL;			//The surface contained by the window
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];//Images rgar correspond to a keypress
SDL_Surface* gCurrentSurface = NULL;			//Current displayed image

//------------------------------------------------------------------------------------------
//INIT()

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		//Create window
		gWindow = SDL_CreateWindow("PELI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return success;
}

//------------------------------------------------------------------------------------------
//LOADMEDIA()

bool loadMedia() {
	//Loading success flag
	bool success = true;

	//Load default surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("img/front.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL) {
		printf("Failed to load default image!\n");
		success = false;
	}

	gKeyPressSurfaces[KEY_PRESS_SURFACE_SHIFT] = loadSurface("img/back.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_SHIFT] == NULL) {
		printf("Failed to load shift image!\n");
		success = false;
	}

	return success;
}

//------------------------------------------------------------------------------------------
//LOADSURFACE()

SDL_Surface* loadSurface(std::string path) {
	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}

	return loadedSurface;
}

//------------------------------------------------------------------------------------------
//CLOSE()

void close() {
	//Deallocate surface
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i) {
		SDL_FreeSurface(gKeyPressSurfaces[i]);
		gKeyPressSurfaces[i] = NULL;
	}

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

//------------------------------------------------------------------------------------------
//MAIN()

int main(int argc, char* args[]) {
	//Start up SDL and create window
	if (!init()) {
		printf("Failed to initialize!\n");
	} else {
		//Load media
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		} else {
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Set default current surface
			gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

			//While application is running
			while (!quit) {
				//Handle eventd on queue
				while (SDL_PollEvent(&e) != 0) {
					//User requests quit
					if (e.type == SDL_QUIT) {
						quit = true;
					} else if (e.type == SDL_KEYDOWN) { //User presses a key
						//Select surfaces based on key press
						switch (e.key.keysym.sym) {
							case SDLK_RSHIFT:
							if (gCurrentSurface == gKeyPressSurfaces[KEY_PRESS_SURFACE_SHIFT]) {
								gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
							} else {
								gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_SHIFT];
							}
							break;
						}
					}
				}
				//Apply the image
				SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);

				//Update the surface
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}

//------------------------------------------------------------------------------------------
