/* assignment2.cpp

  A simple game using SDL2 to demonstrate the use of a 2D graphics API.

  Michael Reiter 2016
*/
#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "vector2d.h"
#include "colourRGB.h"

using namespace std;

static const int WINDOW_SIZE_X = 800;
static const int WINDOW_SIZE_Y = 600;
static float CURSOR_VELOCITY = 300; // velocity is in pixels/second
static const ColourRGB& CURSOR_COLOUR = ColourRGB(26, 188, 156);

class A2Canvas {
public:
	static const int CANVAS_SIZE_X = 800;
	static const int CANVAS_SIZE_Y = 600;
	static const int CURSOR_RADIUS = 15;

	A2Canvas() {
		cursor_position.x = CANVAS_SIZE_X/2;
		cursor_position.y = CANVAS_SIZE_Y/2;
		cursor_direction = Vector2d(0, 0);
	}
	
	void frame_loop(SDL_Renderer* r) {
		unsigned int last_frame = SDL_GetTicks();
		unsigned int frame_number = 0;
		while(1) {
			//cout << "Frame " << frame_number << endl;
			unsigned int current_frame = SDL_GetTicks();
			unsigned int delta_ms = current_frame - last_frame;
			
			SDL_Event e;
			//Handle all queued events
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
						//Exit immediately
						return;
					case SDL_KEYDOWN:
						//e.key stores the key pressed
						handle_key_down(e.key.keysym.sym);
						break;
					case SDL_KEYUP:
						//e.key stores the key pressed
						handle_key_up(e.key.keysym.sym);
						break;
					case SDL_MOUSEMOTION:
						//e.motion contains the relevant mouse position information
						handle_mouse_moved(e.motion.x, e.motion.y);
						break;
					case SDL_MOUSEBUTTONDOWN:
						//e.button contains the relevant mouse position and button information
						handle_mouse_down(e.button.x, e.button.y, e.button.button);
						break;
					case SDL_MOUSEBUTTONUP:		
						handle_mouse_up(e.button.x, e.button.y, e.button.button);
						break;
					default:
						break;
				}
			}
			
			draw(r,delta_ms);
			
			last_frame = current_frame;
			frame_number++;
		}
	}
private:
	void handle_key_down(SDL_Keycode key) {
		if (key == SDLK_UP) {
			cursor_direction.y = -1;
		} else if (key == SDLK_DOWN) {
			cursor_direction.y = 1;
		} else if (key == SDLK_LEFT) {
			cursor_direction.x = -1;
		} else if (key == SDLK_RIGHT) {
			cursor_direction.x = 1;
		}
	}

	void handle_key_up(SDL_Keycode key) {
		if (key == SDLK_UP) {
			cursor_direction.y = 0;
		} else if (key == SDLK_DOWN) {
			cursor_direction.y = 0;
		} else if (key == SDLK_LEFT) {
			cursor_direction.x = 0;
		} else if (key == SDLK_RIGHT) {
			cursor_direction.x = 0;
		}
	}

	void handle_mouse_down(int x, int y, int button) {
		CURSOR_VELOCITY *= 2;
	}

	void handle_mouse_up(int x, int y, int button) {
	}

	void handle_mouse_moved(int x, int y) {
	}

	void draw(SDL_Renderer *renderer, float frame_delta_ms) {

		float frame_delta_seconds = frame_delta_ms/1000.0;
		float position_delta = frame_delta_seconds * CURSOR_VELOCITY;

		Vector2d new_position = cursor_position + position_delta*cursor_direction;

		cursor_position = new_position;

		// Fill background colour
		SDL_SetRenderDrawColor(renderer, 52, 73, 94, 255);
		SDL_RenderClear(renderer);
	
		const ColourRGB& CURSOR_COLOUR = ColourRGB(26, 188, 156);
		filledCircleRGBA(renderer, cursor_position.x, cursor_position.y, CURSOR_RADIUS, CURSOR_COLOUR.r, CURSOR_COLOUR.g, CURSOR_COLOUR.b, 255);

		SDL_RenderPresent(renderer);
	}

	Vector2d cursor_position, cursor_direction;
};

int main() {
	SDL_Window* window = SDL_CreateWindow("CSC 205 Assignment 2",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    WINDOW_SIZE_X, WINDOW_SIZE_Y, 
	  SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	// Initialize the canvas to solid black
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	A2Canvas canvas;
	canvas.frame_loop(renderer);
	
	return 0;
}
