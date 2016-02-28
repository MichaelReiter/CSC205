/* LSViewer.cpp

  Michael Reiter and B. Bird - 02/08/2016

*/
#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stack>

#include "LSystem.h"
#include "matrix.h"
//#include "colourRGB.h"
#include "transformed_renderer.h"

using namespace std;

static const int WINDOW_SIZE_X = 800;
static const int WINDOW_SIZE_Y = 600;

class A3Canvas {
public:
	A3Canvas(LSystem* L) {
		LS_iterations = 0;
		this->L_system = L;
	}
	
	void frame_loop(SDL_Renderer* r) {
		unsigned int last_frame = SDL_GetTicks();
		//unsigned int frame_number = 0;
		draw(r, 0);
		while(1) {
			//cout << "Frame " << frame_number << endl;
			unsigned int current_frame = SDL_GetTicks();
			unsigned int delta_ms = current_frame - last_frame;

			SDL_Event e;
			//Handle all queued events
			while(SDL_PollEvent(&e)) {
				switch(e.type){
					case SDL_QUIT:
						//Exit immediately
						return;
					case SDL_KEYDOWN:
						//e.key stores the key pressed
						handle_key_down(e.key.keysym.sym);
						draw(r, delta_ms);
						break;
					default:
						break;
				}
			}
		}		
	}
private:
	int LS_iterations;
	stack<Matrix3> transformStack;
	LSystem* L_system;
	void handle_key_down(SDL_Keycode key) {
		if (key == SDLK_UP) {
			LS_iterations++;
		} else if (key == SDLK_DOWN) {	
			LS_iterations--;
			if (LS_iterations < 0) {
				LS_iterations = 0;
			}
		}
	}
	
	inline Matrix3 Rotation(float radians) {
		Matrix3 M;
		M.identity();
		M(0,0) = M(1,1) = cos(radians);
		M(1,0) = -(M(0,1) = sin(radians));
		return M;
	}

	inline Matrix3 Translation(float tx, float ty) {
		Matrix3 M;
		M.identity();
		M(0,2) = tx;
		M(1,2) = ty;
		return M;
	}

	inline Matrix3 Scale(float sx, float sy) {
		Matrix3 M;
		M.identity();
		M(0,0) = sx;
		M(1,1) = sy;
		return M;
	}

	void draw_leaf(TransformedRenderer& tr) {
		float vx[] = {0,1.0 ,1.25,   1,  0,  -1,-1.25,-1};
		float vy[] = {0,0.75,1.75,2.75,4.0,2.75, 1.75,0.75};
		int numVerts = 8;
		tr.fillLeaf(vx, vy, numVerts, 46, 204, 113, 255);
		tr.drawPolygon(vx, vy, numVerts, 46, 204, 113, 255);
	}

	void draw_apple(TransformedRenderer& tr) {
		float vx[] = {0, 1.0,1.25,   1,0.25,   0,  -1,-1.25,-1.0};
		float vy[] = {0,0.75,1.75,2.75, 3.0, 2.5,2.75, 1.75,0.75};
		int numVerts = 9;
		tr.fillPolygon(vx, vy, numVerts, 217, 30, 24, 255);
		tr.drawPolygon(vx, vy, numVerts, 217, 30, 24, 255);
	}

	void draw_orange(TransformedRenderer& tr) {
		tr.fillCircle(0, 0, 2, 248, 148, 6, 255);
	}

	void draw_banana(TransformedRenderer& tr) {
		float vx[] = {0,  1.0, 1.5,  1.0, 0,  0.25, 0.75, 0.25};
		float vy[] = {-4, -3.25, -2, -0.75, 0,  -1,  -2.0,  -3};
		int numVerts = 8;
		tr.fillPolygon(vx, vy, numVerts, 244, 208, 63, 255);
		tr.drawPolygon(vx, vy, numVerts, 244, 208, 63, 255);
	}

	void draw(SDL_Renderer *renderer, float frame_delta_ms) {
		//float frame_delta_seconds = frame_delta_ms/1000.0;

		string ls_string = L_system->GenerateSystemString(LS_iterations);
		cerr << "Drawing with " << LS_iterations << " iterations." << endl;
		cerr << "System string: " << ls_string << endl;

		// Draw gradient background
		int color;
		for (int j = 0; j < WINDOW_SIZE_Y; j++) {
			color = (1.0*j/WINDOW_SIZE_Y)*255;
			SDL_SetRenderDrawColor(renderer, color/3, color/2, color, 255);
			SDL_RenderDrawLine(renderer, 0, j, WINDOW_SIZE_X, j);
		}

		TransformedRenderer tr(renderer);
		Matrix3 viewportTransform;

		// Render the L-System multiple times in different positions and orientations
		for (int k = 1; k <= 3; k++) {
			switch (k) {
				case 1:
					viewportTransform.identity();
					viewportTransform *= Translation(WINDOW_SIZE_X*0.25, WINDOW_SIZE_Y*0.8);
					viewportTransform *= Scale(0.5,-0.5);
					viewportTransform *= Rotation(-M_PI/15);
					break;
				case 2:
					viewportTransform.identity();
					viewportTransform *= Translation(WINDOW_SIZE_X/2, WINDOW_SIZE_Y);
					viewportTransform *= Scale(1,-1);
					break;
				case 3:
					viewportTransform.identity();
					viewportTransform *= Translation(WINDOW_SIZE_X*0.75, WINDOW_SIZE_Y*0.6);
					viewportTransform *= Scale(0.5,-0.5);
					viewportTransform *= Rotation(M_PI/12);
					break;
			}
			viewportTransform *= Scale(WINDOW_SIZE_X/100.0, WINDOW_SIZE_Y/100.0);
			tr.set_transform(viewportTransform);

			for (int i = 0; i < ls_string.length(); i++) {
				switch (ls_string[i]) {
					case 'L':
						draw_leaf(tr);
						break;
					case 'T': {
						int h = 10;
						int w = 10;
						tr.drawLine(0, 0, 0, h, w, 153, 112, 79, 255);
						viewportTransform *= Translation(0, h);
						tr.set_transform(viewportTransform);
						break;
					}
					case '+':
						viewportTransform *= Rotation(-M_PI/6);
						tr.set_transform(viewportTransform);
						break;
					case '-':
						viewportTransform *= Rotation(M_PI/6);
						tr.set_transform(viewportTransform);
						break;
					case 's':
						viewportTransform *= Scale(0.9, 0.9);
						tr.set_transform(viewportTransform);
						break;
					case 'S':
						viewportTransform *= Scale(1/0.9, 1/0.9);
						tr.set_transform(viewportTransform);
						break;
					case 'h':
						viewportTransform *= Scale(0.9, 1);
						tr.set_transform(viewportTransform);
						break;
					case 'H':
						viewportTransform *= Scale(1/0.9, 1);
						tr.set_transform(viewportTransform);
						break;
					case 'v':
						viewportTransform *= Scale(1, 0.9);
						tr.set_transform(viewportTransform);
						break;
					case 'V':
						viewportTransform *= Scale(1, 1/0.9);
						tr.set_transform(viewportTransform);
						break;
					case '[':
						transformStack.push(viewportTransform);
						break;
					case ']':
						viewportTransform = transformStack.top();
						transformStack.pop();
						tr.set_transform(viewportTransform);
						break;
					case 'O':
						draw_orange(tr);
						break;
					case 'A':
						draw_apple(tr);
						break;
					case 'B':
						draw_banana(tr);
						break;
				}
			}

		}
	
		SDL_RenderPresent(renderer);
	}
};

int main(int argc, char** argv) {
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <input file>" << endl;
		return 0;
	}

	char* input_filename = argv[1];
	LSystem* L = LSystem::ParseFile(input_filename);

	if (!L) {
		cerr << "Parsing failed." << endl;
		return 0;
	}

	SDL_Window* window = SDL_CreateWindow("CSC 205 A3",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    WINDOW_SIZE_X, WINDOW_SIZE_Y, 
		SDL_WINDOW_SHOWN
	);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	//Initialize the canvas to solid black
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	A3Canvas canvas(L);
	canvas.frame_loop(renderer);
	delete L;
	
	return 0;
}