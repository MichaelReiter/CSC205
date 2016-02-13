/* assignment2.cpp

  A simple game similar to Missile Command built using SDL
  to demonstrate the use of a low level 2D graphics API.

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

static const int WINDOW_SIZE_X = 1024;
static const int WINDOW_SIZE_Y = 768;
static const int CANNON_THICKNESS = 15;
static const int CANNON_LENGTH = 20;
static const float CURSOR_VELOCITY = 300; 		// velocity is in pixels/second
static const float SHOT_VELOCITY = 300;

static const ColourRGB& MISSILE_COLOUR = ColourRGB(255, 255, 255);	// White
static const ColourRGB& CURSOR_COLOUR = ColourRGB(149, 165, 166);		// Light Grey
static const ColourRGB& SHOT_COLOUR = ColourRGB(255, 255, 255);			// White
static const ColourRGB& BACKGROUND_COLOUR = ColourRGB(30, 30, 30);	// Charcoal
static const ColourRGB& GROUND_COLOUR = ColourRGB(52,73,94);				// Navy
static const ColourRGB& BASE_COLOUR = ColourRGB(39, 174, 96);				// Green
static const ColourRGB& CANNON_COLOUR = ColourRGB(127, 140, 141);		// Dark Grey
static const ColourRGB EXPLOSION_COLOUR[] = {
	ColourRGB(192, 57, 43),	// Red
	ColourRGB(241,196,15),	// Yellow
};
static const int EXPLOSION_COLOURS_LENGTH = sizeof(EXPLOSION_COLOUR) / sizeof(EXPLOSION_COLOUR[0]);
static const unsigned int EXPLOSION_RADIUS = 40;

static const Vector2d CANNON_BASE = Vector2d(WINDOW_SIZE_X/2, WINDOW_SIZE_Y-43);
static Vector2d BASE_LOCATION[] = {
	Vector2d(WINDOW_SIZE_X/10 + 50, WINDOW_SIZE_Y-51),
	Vector2d(3*WINDOW_SIZE_X/10 + 50, WINDOW_SIZE_Y-51),
	Vector2d(6*WINDOW_SIZE_X/10 + 50, WINDOW_SIZE_Y-51),
	Vector2d(8*WINDOW_SIZE_X/10 + 50, WINDOW_SIZE_Y-51)
};

class A2Canvas {
public:
	static const int CANVAS_SIZE_X = WINDOW_SIZE_X;
	static const int CANVAS_SIZE_Y = WINDOW_SIZE_Y;
	static const int CURSOR_RADIUS = 3;
	static const int SHOT_RADIUS = 1;

	A2Canvas() {
		cursor_position.x = CANVAS_SIZE_X/2;
		cursor_position.y = CANVAS_SIZE_Y/2;
		cursor_direction = Vector2d(0, 0);
		can_shoot = true;
		boom = false;
		explosion_time = EXPLOSION_RADIUS;
		explosion_frame = 0;
		targeted_base = 0;
		missile_velocity = 50;
		missile_spawnpoint = Vector2d(WINDOW_SIZE_X/8, 0);
		missile_endpoint = missile_spawnpoint;
		missile_target = BASE_LOCATION[targeted_base];
		missile_on_screen = true;
		base_alive[0] = true;
		base_alive[1] = true;
		base_alive[2] = true;
		base_alive[3] = true;
		gameOver = false;
	}
	
	void frame_loop(SDL_Renderer* r) {
		unsigned int last_frame = SDL_GetTicks();
		unsigned int frame_number = 0;
		while(1) {
			unsigned int current_frame = SDL_GetTicks();
			unsigned int delta_ms = current_frame - last_frame;
			
			SDL_Event e;
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
						//Exit immediately
						return;
					case SDL_KEYDOWN:
						handle_key_down(e.key.keysym.sym);
						break;
					case SDL_KEYUP:
						handle_key_up(e.key.keysym.sym);
						break;
					case SDL_MOUSEMOTION:
						handle_mouse_moved(e.motion.x, e.motion.y);
						break;
					case SDL_MOUSEBUTTONDOWN:
						handle_mouse_down(e.button.x, e.button.y, e.button.button);
						break;
					default:
						break;
				}
			}
			
			draw(r,delta_ms);
			
			if (gameOver) {
				return;
			}

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
		} else if (key == SDLK_SPACE && can_shoot) {
			fire_shot(cursor_position);
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

	// Destroy bases if they are clicked
	void handle_mouse_down(int x, int y, int button) {
		if (base_alive[0] && x > 1*(CANVAS_SIZE_X/10) && x < 1*(CANVAS_SIZE_X/10) + 100
			&& y > CANVAS_SIZE_Y - 50 && y < CANVAS_SIZE_Y - 26) {
			destroyBase(0);
		} else if (base_alive[1] && x > 3*(CANVAS_SIZE_X/10) && x < 3*(CANVAS_SIZE_X/10) + 100
			&& y > CANVAS_SIZE_Y - 50 && y < CANVAS_SIZE_Y - 26) {
			destroyBase(1);
		} else if (base_alive[2] && x > 6*(CANVAS_SIZE_X/10) && x < 6*(CANVAS_SIZE_X/10) + 100
			&& y > CANVAS_SIZE_Y - 50 && y < CANVAS_SIZE_Y - 26) {
			destroyBase(2);
		} else if (base_alive[3] && x > 8*(CANVAS_SIZE_X/10) && x < 8*(CANVAS_SIZE_X/10) + 100
			&& y > CANVAS_SIZE_Y - 50 && y < CANVAS_SIZE_Y - 26) {
			destroyBase(3);
		}

		// target remaining bases
		if (base_alive[0]) {
			targetBase(0);
		} else if (base_alive[1]) {
			targetBase(1);
		} else if (base_alive[2]) {
			targetBase(2);
		} else if (base_alive[3]) {
			targetBase(3);
		}
	}

	void handle_mouse_moved(int x, int y) {
	}

	void fire_shot(Vector2d target) {
		// Shot starts at cannon and fires toward cursor
		shot_position = Vector2d(CANVAS_SIZE_X/2, CANVAS_SIZE_Y - 50);
		target_position = cursor_position;
		shot_direction = target_position - shot_position;
		can_shoot = false;
	}

	int explosion_size(int x) {
		return EXPLOSION_RADIUS - x;
	}

	void draw_explosion(Vector2d position, SDL_Renderer* renderer) {
		if (explosion_time > 0) {
			if (explosion_time % 2 == 0) {
				explosion_frame = (explosion_frame + 1) % EXPLOSION_COLOURS_LENGTH;
			}

			filledCircleRGBA(renderer,
				position.x, position.y,
				explosion_size(explosion_time),
				EXPLOSION_COLOUR[explosion_frame].r,
				EXPLOSION_COLOUR[explosion_frame].g,
				EXPLOSION_COLOUR[explosion_frame].b,
				255);
			explosion_time -= 5;
		} else if (explosion_time == 0) {
			boom = false;
			explosion_time = EXPLOSION_RADIUS;
		}
	}

	void drawMissile(SDL_Renderer* renderer, Vector2d startpoint, Vector2d endpoint, float frame_delta_ms) {
		// Update missile location
		float frame_delta_seconds = frame_delta_ms/1000.0;
		float missile_position_delta = frame_delta_seconds * missile_velocity;
		Vector2d missile_direction = endpoint - startpoint;
		missile_endpoint = missile_endpoint + missile_position_delta*missile_direction.normalize();

		lineRGBA(renderer,
			startpoint.x, startpoint.y,
			missile_endpoint.x, missile_endpoint.y,
			MISSILE_COLOUR.r, MISSILE_COLOUR.g, MISSILE_COLOUR.b, 255);
	}

	void destroyBase(int base) {
		boom = true;
		explosion_position = BASE_LOCATION[base];
		base_alive[base] = false;
		missile_on_screen = false;
	}

	void targetBase(int base) {
		targeted_base = base;
		missile_target = BASE_LOCATION[targeted_base];
	}

	void draw(SDL_Renderer *renderer, float frame_delta_ms) {
		float frame_delta_seconds = frame_delta_ms/1000.0;

		// Fill background colour
		SDL_SetRenderDrawColor(renderer,
			BACKGROUND_COLOUR.r, BACKGROUND_COLOUR.g, BACKGROUND_COLOUR.b, 255);
		SDL_RenderClear(renderer);

		// Draw missile
		if (missile_on_screen == true) {
			// Reset missile if collides with base
			if (abs(missile_endpoint.x - missile_target.x) < 3 && abs(missile_endpoint.y - missile_target.y) < 3) {
				destroyBase(targeted_base);
				targetBase(targeted_base+1);
			}
			drawMissile(renderer, missile_spawnpoint, missile_target, frame_delta_ms);
		} else {
			// reset and fire again
			missile_endpoint = missile_spawnpoint;
			missile_on_screen = true;
		}

		// Collide missile with cannon shot
		int explosion_bounds = explosion_size(explosion_time);
		if (boom
			&& abs(missile_endpoint.x - explosion_position.x) < explosion_bounds
			&& abs(missile_endpoint.y - explosion_position.y) < explosion_bounds) {
			missile_on_screen = false;
			missile_velocity += 25;
		}

		// Draw ground
		boxRGBA(renderer,
			CANVAS_SIZE_X, CANVAS_SIZE_Y - 25,
			0, CANVAS_SIZE_Y,
			GROUND_COLOUR.r, GROUND_COLOUR.g, GROUND_COLOUR.b, 255);

		// Draw bases
		if (base_alive[0]) {
			boxRGBA(renderer,
			1*(CANVAS_SIZE_X/10) + 100, CANVAS_SIZE_Y - 50,
			1*(CANVAS_SIZE_X/10), CANVAS_SIZE_Y - 26,
			BASE_COLOUR.r, BASE_COLOUR.g, BASE_COLOUR.b, 255);
		}
		if (base_alive[1]) {
			boxRGBA(renderer,
			3*(CANVAS_SIZE_X/10) + 100, CANVAS_SIZE_Y - 50,
			3*(CANVAS_SIZE_X/10), CANVAS_SIZE_Y - 26,
			BASE_COLOUR.r, BASE_COLOUR.g, BASE_COLOUR.b, 255);
		}
		if (base_alive[2]) {
			boxRGBA(renderer,
			6*(CANVAS_SIZE_X/10) + 100, CANVAS_SIZE_Y - 50,
			6*(CANVAS_SIZE_X/10), CANVAS_SIZE_Y - 26,
			BASE_COLOUR.r, BASE_COLOUR.g, BASE_COLOUR.b, 255);
		}
		if (base_alive[3]) {
			boxRGBA(renderer,
			8*(CANVAS_SIZE_X/10) + 100, CANVAS_SIZE_Y - 50,
			8*(CANVAS_SIZE_X/10), CANVAS_SIZE_Y - 26,
			BASE_COLOUR.r, BASE_COLOUR.g, BASE_COLOUR.b, 255);
		} else if (!base_alive[0] && !base_alive[1] && !base_alive[2]) {
			gameOver = true;
		}

		// Draw shot
		if (can_shoot == false) {
			float shot_position_delta = frame_delta_seconds * SHOT_VELOCITY;
			Vector2d new_position = shot_position + shot_position_delta*shot_direction.normalize();
			shot_position = new_position;
			filledCircleRGBA(renderer,
				shot_position.x, shot_position.y,
				SHOT_RADIUS,
				SHOT_COLOUR.r, SHOT_COLOUR.g, SHOT_COLOUR.b, 255);
			
			if (abs(shot_position.x - target_position.x) <= 3 && abs(shot_position.y - target_position.y) <= 3) {
				can_shoot = true;
				boom = true;
				explosion_position = target_position;
			}
		}

		// Draw cannon
		cannon_direction = (cursor_position - CANNON_BASE).normalize();
		cannon_end = Vector2d(CANNON_BASE.x + CANNON_LENGTH*cannon_direction.x, CANNON_BASE.y + CANNON_LENGTH*cannon_direction.y);
		thickLineRGBA(renderer,
			CANNON_BASE.x, CANNON_BASE.y,
			cannon_end.x, cannon_end.y,
			CANNON_THICKNESS,
			CANNON_COLOUR.r, CANNON_COLOUR.g, CANNON_COLOUR.b, 255);
		filledTrigonRGBA(renderer,
			CANVAS_SIZE_X/2 - 30, CANVAS_SIZE_Y - 26,
			CANVAS_SIZE_X/2, CANVAS_SIZE_Y - 52,
			CANVAS_SIZE_X/2 + 30, CANVAS_SIZE_Y - 26,
			CANNON_COLOUR.r, CANNON_COLOUR.g, CANNON_COLOUR.b, 255);

		// Draw explosion
		if (boom) {
			draw_explosion(explosion_position, renderer);
		}

		// Update cursor location
		float cursor_position_delta = frame_delta_seconds * CURSOR_VELOCITY;
		Vector2d new_position = cursor_position + cursor_position_delta*cursor_direction.normalize();
		if (new_position.x > 10 && new_position.x < CANVAS_SIZE_X - 10 && new_position.y > 10 && new_position.y < CANVAS_SIZE_Y - 100) {
			cursor_position = new_position;
		}

		// Draw cursor
		filledCircleRGBA(renderer,
			cursor_position.x, cursor_position.y,
			CURSOR_RADIUS,
			CURSOR_COLOUR.r, CURSOR_COLOUR.g, CURSOR_COLOUR.b, 255);

		SDL_RenderPresent(renderer);
	}

	Vector2d cursor_position, cursor_direction, shot_position, 
	shot_direction, cannon_direction, cannon_end, target_position, 
	explosion_position, missile_target, missile_endpoint,
	missile_spawnpoint;
	bool can_shoot, boom, missile_on_screen, gameOver;
	bool base_alive[4];
	int explosion_time, explosion_frame, targeted_base, missile_velocity;
};

int main() {
	SDL_Window* window = SDL_CreateWindow("Missile Defender",
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
