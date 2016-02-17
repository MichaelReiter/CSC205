/* LSViewer.cpp

   B. Bird - 02/08/2016
*/
#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "LSystem.h"
#include "matrix.h"
//#include "colourRGB.h"
#include "transformed_renderer.h"

using namespace std;

static const int WINDOW_SIZE_X = 800;
static const int WINDOW_SIZE_Y = 600;


class A3Canvas{
public:

	A3Canvas(LSystem* L){
		LS_iterations = 0;
		this->L_system = L;
	}

	
	void frame_loop(SDL_Renderer* r){
		unsigned int last_frame = SDL_GetTicks();
		//unsigned int frame_number = 0;
		draw(r,0);
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
						draw(r,delta_ms);
						break;
					default:
						break;
				}
			}
			
			
			
		}
		
	}
private:
	int LS_iterations;
	LSystem* L_system;
	void handle_key_down(SDL_Keycode key){
		if (key == SDLK_UP){
			LS_iterations++;
		}else if (key == SDLK_DOWN){	
			LS_iterations--;
			if (LS_iterations < 0)
				LS_iterations = 0;
		}
	}
	
	
	inline Matrix3 Rotation(float radians){
		Matrix3 M;
		M.identity();
		M(0,0) = M(1,1) = cos(radians);
		M(1,0) = -(M(0,1) = sin(radians));
		return M;
	}
	inline Matrix3 Translation(float tx, float ty){
		Matrix3 M;
		M.identity();
		M(0,2) = tx;
		M(1,2) = ty;
		return M;
	}
	inline Matrix3 Scale(float sx, float sy){
		Matrix3 M;
		M.identity();
		M(0,0) = sx;
		M(1,1) = sy;
		return M;
	}


	void draw_leaf(TransformedRenderer& tr){
		float vx[] = {0,1.0 ,1.25,   1,  0,  -1,-1.25,-1};
		float vy[] = {0,0.75,1.75,2.75,4.0,2.75, 1.75,0.75};
		int numVerts = 8;
		tr.fillPolygon(vx,vy,numVerts, 64,224,0, 255);
		tr.drawPolygon(vx,vy,numVerts, 64,128,0, 255);
	}



	void draw(SDL_Renderer *renderer, float frame_delta_ms){
	

		//float frame_delta_seconds = frame_delta_ms/1000.0;

		string ls_string = L_system->GenerateSystemString(LS_iterations);
		cerr << "Drawing with " << LS_iterations << " iterations." << endl;
		cerr << "System string: " << ls_string << endl;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		TransformedRenderer tr(renderer);
		Matrix3 viewportTransform;
		viewportTransform.identity();
		viewportTransform *= Translation(WINDOW_SIZE_X/2, WINDOW_SIZE_Y);
		viewportTransform *= Scale(1,-1);
		viewportTransform *= Scale(WINDOW_SIZE_X/100.0,WINDOW_SIZE_Y/100.0);
		
		tr.set_transform(viewportTransform);
		
		//Replace this with actual drawing code...
		draw_leaf(tr);
		
	
		SDL_RenderPresent(renderer);
	}
};

int main(int argc, char** argv){

	if (argc < 2){
		cerr << "Usage: " << argv[0] << " <input file>" << endl;
		return 0;
	}
	char* input_filename = argv[1];
	
	LSystem* L = LSystem::ParseFile(input_filename);
	if (!L){
		cerr << "Parsing failed." << endl;
		return 0;
	}

	SDL_Window* window = SDL_CreateWindow("CSC 205 A3",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_SIZE_X, WINDOW_SIZE_Y, 
							  SDL_WINDOW_SHOWN);
							  
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0/*SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED*/);

	//Initialize the canvas to solid green
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	A3Canvas canvas(L);

	canvas.frame_loop(renderer);
	
	delete L;
	
	return 0;
}
