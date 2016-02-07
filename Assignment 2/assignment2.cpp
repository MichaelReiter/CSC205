/* sample_ball.cpp

   A sample program consisting of an animated ball which bounces off of the edges of the screen.


   B. Bird - 01/17/2016
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
static const float BALL_VELOCITY = 150; //Pixels/second

static const ColourRGB BALL_COLOURS[] = {
	ColourRGB(0,0,0),
	ColourRGB(255,0,0),
	ColourRGB(0,255,0),
	ColourRGB(0,0,255),
	ColourRGB(0,255,255),
	ColourRGB(255,0,255),
	ColourRGB(255,255,0)
};

class A2Canvas{
public:
	static const int CANVAS_SIZE_X = 800;
	static const int CANVAS_SIZE_Y = 600;
	static const int BALL_RADIUS = 15;

	static const int NUM_COLOURS = 7;
	
	A2Canvas(){
		ball_position.x = CANVAS_SIZE_X/2;
		ball_position.y = CANVAS_SIZE_Y/2;
		ball_direction.x = cos(30*(M_PI/180.0));
		ball_direction.y = sin(30*(M_PI/180.0));
		ball_colour_idx = 0;
	}

	
	void frame_loop(SDL_Renderer* r){
		unsigned int last_frame = SDL_GetTicks();
		unsigned int frame_number = 0;
		while(1){
			//cout << "Frame " << frame_number << endl;
			unsigned int current_frame = SDL_GetTicks();
			unsigned int delta_ms = current_frame - last_frame;
			
			SDL_Event e;
			//Handle all queued events
			while(SDL_PollEvent(&e)){
				switch(e.type){
					case SDL_QUIT:
						//Exit immediately
						return;
					case SDL_KEYDOWN:
						//e.key stores the key pressed
						handle_key_down(e.key.keysym.sym);
						break;
					case SDL_MOUSEMOTION:
						//e.motion contains the relevant mouse position information
						handle_mouse_moved(e.motion.x,e.motion.y);
						break;
					case SDL_MOUSEBUTTONDOWN:
						//e.button contains the relevant mouse position and button information
						handle_mouse_down(e.button.x,e.button.y,e.button.button);
						break;
					case SDL_MOUSEBUTTONUP:		
						handle_mouse_up(e.button.x,e.button.y,e.button.button);
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

	void handle_key_down(SDL_Keycode key){
		if (key == SDLK_UP){
			ball_colour_idx++;
			if (ball_colour_idx >= NUM_COLOURS)
				ball_colour_idx = 0;
		}else if (key == SDLK_DOWN){	
			ball_colour_idx--;
			if (ball_colour_idx < 0)
				ball_colour_idx = NUM_COLOURS-1;
		}else if (key == SDLK_r){
			Vector2d rotation( cos(30*(M_PI/180)), sin(30*(M_PI/180)) );
			Vector2d new_direction(
					rotation.x*ball_direction.x - rotation.y*ball_direction.y,
					rotation.x*ball_direction.y + rotation.y*ball_direction.x);
			ball_direction = new_direction;
		}
	}
	void handle_mouse_down(int x, int y, int button){
	}
	void handle_mouse_up(int x, int y, int button){
	}
	void handle_mouse_moved(int x, int y){
	}

	void draw(SDL_Renderer *renderer, float frame_delta_ms){
	

		float frame_delta_seconds = frame_delta_ms/1000.0;

		float position_delta = frame_delta_seconds*BALL_VELOCITY;

		Vector2d new_position = ball_position + position_delta*ball_direction;

		//The ball collides with the edge of the screen if the new position is less than BALL_RADIUS
		//pixels away from any edge.

		
		if (new_position.x <= BALL_RADIUS){
			//Collide with left edge
			
			//Determine how far past the collision point the new position is.
			float offset_x = BALL_RADIUS-new_position.x;
			//Mirror the direction around the y axis (since the ball bounces)
			ball_direction.x = -ball_direction.x;
			new_position.x += 2*offset_x;
		}else if(new_position.x >= CANVAS_SIZE_X - BALL_RADIUS){
			//Collide with right edge

			//Determine how far past the collision point the new position is.
			float offset_x = new_position.x - (CANVAS_SIZE_X-BALL_RADIUS);
			//Mirror the direction around the y axis (since the ball bounces)
			ball_direction.x = -ball_direction.x;
			new_position.x -= 2*offset_x;
		}else if(new_position.y <= BALL_RADIUS){
			//Collide with top

			//Determine how far past the collision point the new position is.
			float offset_y = BALL_RADIUS-new_position.y;
			//Mirror the direction around the x axis (since the ball bounces)
			ball_direction.y = -ball_direction.y;
			new_position.y += 2*offset_y;
		}else if(new_position.y >= CANVAS_SIZE_Y - BALL_RADIUS){
			//Collide with bottom

			//Determine how far past the collision point the new position is.
			float offset_y = new_position.y - (CANVAS_SIZE_Y-BALL_RADIUS);
			//Mirror the direction around the x axis (since the ball bounces)
			ball_direction.y = -ball_direction.y;
			new_position.y -= 2*offset_y;
		}
		ball_position = new_position;


		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
		SDL_RenderClear(renderer);
	
		const ColourRGB& ball_colour = BALL_COLOURS[ball_colour_idx];
		filledCircleRGBA(renderer,ball_position.x,ball_position.y,BALL_RADIUS,ball_colour.r,ball_colour.g,ball_colour.b,255);

		SDL_RenderPresent(renderer);
	}
	Vector2d ball_position,ball_direction;	
	int ball_colour_idx;
};

int main(){

	SDL_Window* window = SDL_CreateWindow("CSC 205 A2",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_SIZE_X, WINDOW_SIZE_Y, 
							  SDL_WINDOW_SHOWN);
							  
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0/*SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED*/);

	//Initialize the canvas to solid green
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	A2Canvas canvas;

	canvas.frame_loop(renderer);
	
	return 0;
}
