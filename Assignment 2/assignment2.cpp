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
static float CURSOR_VELOCITY = 150; // velocity is in pixels/second

class A2Canvas {
public:
	static const int CANVAS_SIZE_X = 800;
	static const int CANVAS_SIZE_Y = 600;
	static const int CURSOR_RADIUS = 15;

	A2Canvas() {
		ball_position.x = CANVAS_SIZE_X/2;
		ball_position.y = CANVAS_SIZE_Y/2;
		ball_direction.x = cos(30*(M_PI/180.0));
		ball_direction.y = sin(30*(M_PI/180.0));
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

		} else if (key == SDLK_DOWN) {

		} else if (key == SDLK_LEFT) {

		} else if (key == SDLK_RIGHT) {

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

		Vector2d new_position = ball_position + position_delta*ball_direction;

		//The ball collides with the edge of the screen if the new position is less than CURSOR_RADIUS
		//pixels away from any edge.

		if (new_position.x <= CURSOR_RADIUS) {
			//Collide with left edge
			
			//Determine how far past the collision point the new position is.
			float offset_x = CURSOR_RADIUS-new_position.x;
			//Mirror the direction around the y axis (since the ball bounces)
			ball_direction.x = -ball_direction.x;
			new_position.x += 2*offset_x;
		} else if (new_position.x >= CANVAS_SIZE_X - CURSOR_RADIUS) {
			//Collide with right edge

			//Determine how far past the collision point the new position is.
			float offset_x = new_position.x - (CANVAS_SIZE_X - CURSOR_RADIUS);
			//Mirror the direction around the y axis (since the ball bounces)
			ball_direction.x = -ball_direction.x;
			new_position.x -= 2*offset_x;
		} else if(new_position.y <= CURSOR_RADIUS) {
			//Collide with top

			//Determine how far past the collision point the new position is.
			float offset_y = CURSOR_RADIUS-new_position.y;
			//Mirror the direction around the x axis (since the ball bounces)
			ball_direction.y = -ball_direction.y;
			new_position.y += 2*offset_y;
		} else if(new_position.y >= CANVAS_SIZE_Y - CURSOR_RADIUS) {
			//Collide with bottom

			//Determine how far past the collision point the new position is.
			float offset_y = new_position.y - (CANVAS_SIZE_Y - CURSOR_RADIUS);
			//Mirror the direction around the x axis (since the ball bounces)
			ball_direction.y = -ball_direction.y;
			new_position.y -= 2*offset_y;
		}
		ball_position = new_position;

		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
		SDL_RenderClear(renderer);
	
		const ColourRGB& ball_colour = ColourRGB(0,0,0);
		filledCircleRGBA(renderer, ball_position.x, ball_position.y, CURSOR_RADIUS, ball_colour.r, ball_colour.g, ball_colour.b, 255);

		SDL_RenderPresent(renderer);
	}
	Vector2d ball_position, ball_direction;
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
