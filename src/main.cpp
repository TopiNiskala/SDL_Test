//------------------------------------------------------------------------------------------
//HEADER

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

//------------------------------------------------------------------------------------------
//DECLARATIONS

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture {
	public:
		LTexture();				//Initializes variables
		~LTexture();				//Deallocates memory
		bool loadFromFile(std::string path);	//Loads image at specified path
		void free();				//Deallocates texture
		void render(int x, int y);		//Renders texture at given point
		int getWidth();				//Get image width
		int getHeight();			//Get image height
	private:
		SDL_Texture* mTexture;			//The actual hardware texture
		int mWidth;				//Image width
		int mHeight;				//Image height
};

bool init(); 					//Start SDL and create window
bool loadMedia();				//Load media
void close();					//Free media and shut down SDL
SDL_Window* gWindow = NULL;			//The rendering window
SDL_Renderer* gRenderer = NULL;			//The window renderer
LTexture gCursorTexture;			//Cursor texture
LTexture gBackgroundTexture;			//Background texture

LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {
	//Deallocate
	free();
}

//------------------------------------------------------------------------------------------
//LOADFROMFILE()

bool LTexture::loadFromFile(std::string path) {
	//Get rid of pre-existing texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_Image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	//Return success
	mTexture = newTexture;
	return mTexture != NULL;

}

//------------------------------------------------------------------------------------------
//FREE()

void  LTexture::free() {
	//Free texture if exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

//------------------------------------------------------------------------------------------
//RENDER()

void LTexture::render(int x, int y) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};
	SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad);
}

//------------------------------------------------------------------------------------------
//GETWIDTH() and GETHEIGHT()

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

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
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}
		//Create window
		gWindow = SDL_CreateWindow("PELI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			} else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

//------------------------------------------------------------------------------------------
//LOADMEDIA()

bool loadMedia() {
	//Loading success flag
	bool success = true;

	//Load cursor texture
	if (!gCursorTexture.loadFromFile("img/cursor.png")) {
		printf("Failed to load cursor texture image!\n");
		success = false;
	}

	//Load background texture
	if (!gBackgroundTexture.loadFromFile("img/front.png")) {
		printf("Failed to load background texture image!\n");
		success = false;
	}

	return success;
}

//------------------------------------------------------------------------------------------
//CLOSE()

void close() {
	//Free loaded images
	gCursorTexture.free();
	gBackgroundTexture.free();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
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

			//While application is running
			while (!quit) {
				//Handle eventd on queue
				while (SDL_PollEvent(&e) != 0) {
					//User requests quit
					if (e.type == SDL_QUIT) {
						quit = true;
					}
				}
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render background texture to screen
				gBackgroundTexture.render(0, 0);

				//Render cursor texture to screen
				gCursorTexture.render(240, 190);

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}

//------------------------------------------------------------------------------------------
