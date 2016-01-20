/* draw_bvg.cpp
   
   
   B. Bird - 01/02/2016
*/

#include <string>
#include <iostream>
#include "vector2d.h"
#include "colourRGB.h"
#include "bvg.h"
#include "png_canvas.h"
#include "stdlib.h"

using namespace std;


class BVGRenderer: public BVGRendererBase{
public:
	virtual void create_canvas(Vector2d dimensions, ColourRGB background_colour, int scale_factor){
		cout << "Canvas " << dimensions << background_colour << scale_factor << endl;
		width = dimensions[0];
		height = dimensions[1];
		canvas.initialize_canvas(dimensions[0],dimensions[1]);
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				canvas[x][y] = background_colour;
	}

	virtual void render_line(Vector2d endpoint1, Vector2d endpoint2, ColourRGB colour, int thickness){
		cout << "Line " << endpoint1 << endpoint2 << colour << thickness << endl;
		int F = 0;
		int x = endpoint1.x;
		int y = endpoint1.y;
		Vector2d L = endpoint2 - endpoint1;
		// if (L.x < 0 && L.y < 0) {
		// 	float temp = L.x;
		// 	L.x = L.y;
		// 	L.y = temp;
		// }
		// cout << endpoint2.x << endl;

		// if endpoint2.y < endpoint.x then slope <= 1, so walk along x, else walk along y
		if (endpoint2.y < endpoint2.x) {
			while (x < endpoint2.x) {
				// draw points
				canvas[x][y] = colour;
				if (abs(F + L.y) < abs(F + L.y - L.x)) {
					x += 1;
					F = F + L.y;
				} else {
					x += 1;
					y += 1;
					F = F + L.y - L.x;
				}
			}
		} else {
			while (y <= endpoint2.y) {
				// draw points
				canvas[x][y] = colour;
				if (abs(F + L.x) < abs(F + L.x - L.y)) {
					y += 1;
					F = F + L.x;
				} else {
					y += 1;
					x += 1;
					F = F + L.x - L.y;
				}
			}
		}
	}

	virtual void render_circle(Vector2d center, int radius, ColourRGB line_colour, int line_thickness){
		cout << "Circle " << center << radius << line_colour << line_thickness << endl;
		int F = 0;
		int x = 0;
		int y = radius;

		while (x <= y) {
			// draw points
			canvas[x + center.x][y + center.y] = line_colour;
			canvas[y + center.x][x + center.y] = line_colour;
			canvas[-x + center.x][y + center.y] = line_colour;
			canvas[-y + center.x][x + center.y] = line_colour;
			canvas[x + center.x][-y + center.y] = line_colour;
			canvas[y + center.x][-x + center.y] = line_colour;
			canvas[-x + center.x][-y + center.y] = line_colour;
			canvas[-y + center.x][-x + center.y] = line_colour;

			if (abs(F + 2*x + 1) < abs(F + 2*(x-y) + 2)) {
				x += 1;
				F += (2*x + 1);
			} else {
				x += 1;
				y -= 1;
				F += (2*(x-y) + 2);
			}
		}
	}

	virtual void render_filledcircle(Vector2d center, int radius, ColourRGB line_colour, int line_thickness, ColourRGB fill_colour){
		cout << "Filled Circle " << center << radius << line_colour << line_thickness << fill_colour << endl;
		int F = 0;
		int x = 0;
		int y = radius;

		while (x <= y) {
			// draw points
			Vector2d *endpoint1 = new Vector2d((float)(x + center.x), (float)(y + center.y));
			Vector2d *endpoint2 = new Vector2d((float)(y + center.x), (float)(x + center.y));
			render_line(*endpoint1, *endpoint2, line_colour, 1);
			delete endpoint1;
			delete endpoint2;

			canvas[-x + center.x][y + center.y] = line_colour;
			canvas[-y + center.x][x + center.y] = line_colour;
			canvas[x + center.x][-y + center.y] = line_colour;
			canvas[y + center.x][-x + center.y] = line_colour;
			canvas[-x + center.x][-y + center.y] = line_colour;
			canvas[-y + center.x][-x + center.y] = line_colour;

			if (abs(F + 2*x + 1) < abs(F + 2*(x-y) + 2)) {
				x += 1;
				F += (2*x + 1);
			} else {
				x += 1;
				y -= 1;
				F += (2*(x-y) + 2);
			}
		}

	}

	virtual void render_triangle(Vector2d point1, Vector2d point2, Vector2d point3, ColourRGB line_colour, int line_thickness, ColourRGB fill_colour){
		cout << "Triangle " << point1 << point2 << point3 << line_colour << line_thickness << fill_colour << endl;


	}

	virtual void render_gradient_triangle(Vector2d point1, Vector2d point2, Vector2d point3, ColourRGB line_colour, int line_thickness, ColourRGB colour1, ColourRGB colour2, ColourRGB colour3){
		cout << "Triangle " << point1 << point2 << point3 << line_colour << line_thickness << colour1 << colour2 << colour3 << endl;


	}
	
	void save_image(string filename){
		canvas.save_image(filename);
	}
private:
	PNG_Canvas canvas;
	int width,height;
};

int main(int argc, char** argv){
	if (argc < 3){
		cerr << "Usage: " << argv[0] << " <input file> <output file>" << endl;
		return 0;
	}
	string input_filename = argv[1];
	string output_filename = argv[2];
	
	BVGRenderer r;
	BVGReader reader(r);
	if (!reader.parse_file(input_filename)){
		cerr << "Unable to parse file" << endl;
		return 0;
	}
	r.save_image(output_filename);
	
	
}