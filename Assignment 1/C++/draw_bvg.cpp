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
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				canvas[x][y] = background_colour;
			}
		}
	}

	virtual void render_line(Vector2d endpoint1, Vector2d endpoint2, ColourRGB colour, int thickness){
		cout << "Line " << endpoint1 << endpoint2 << colour << thickness << endl;

		// Eliminate left half of cartesian plane by checking which endpoint is leftmost
		// This way we are always walking left to right
		Vector2d startpoint, endpoint;
		if (endpoint2.x > endpoint1.x) {
			startpoint = endpoint1;
			endpoint = endpoint2;
		} else {
			startpoint = endpoint2;
			endpoint = endpoint1;
		}

		cout << "Start: (" << startpoint.x << ", " << startpoint.y << ")" << endl;
		cout << "End:   (" << endpoint.x << ", " << endpoint.y << ")" << endl;

		int upDown;
		if (endpoint.y - startpoint.y > 0) {
			upDown = 1;
		} else {
			upDown = -1;
		}

		int deltaX = abs(endpoint.x - startpoint.x);
		int deltaY = abs(endpoint.y - startpoint.y);

		Vector2d L = endpoint - startpoint;
		int F = 0;
		int x = startpoint.x;
		int y = startpoint.y;

		// if deltaX >= deltaY then x is long side, so walk along x, else walk along y
		if (deltaX >= deltaY) {
			cout << "walking " << (upDown > 0 ? "up" : "down") << " along x" << endl;
			while (x <= endpoint.x) {
				// draw points
				canvas[x][y] = colour;
				if (abs(F + L.y) < abs(F + L.y - upDown*L.x)) {
					x += 1;
					F = F + L.y;
				} else {
					x += 1;
					y += upDown;
					F = F + L.y - upDown*L.x;
				}
			}
		} else {
			cout << "walking " << (upDown > 0 ? "up" : "down") << " along y" << endl;
			while (y != endpoint.y) {
				// draw points
				canvas[x][y] = colour;
				if (abs(F + L.x) < abs(F + L.x - upDown*L.y)) {
					y += upDown;
					F = F + L.x;
				} else {
					y += upDown;
					x += 1;
					F = F + L.x - upDown*L.y;
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
			Vector2d *endpoint1 = new Vector2d(x + center.x, y + center.y);
			Vector2d *endpoint2 = new Vector2d(-x + center.x, -y + center.y);
			render_line(*endpoint1, *endpoint2, fill_colour, 1);
			delete endpoint1;
			delete endpoint2;

			endpoint1 = new Vector2d(x + center.x, y + center.y);
			endpoint2 = new Vector2d(y + center.x, x + center.y);
			render_line(*endpoint1, *endpoint2, fill_colour, 1);
			delete endpoint1;
			delete endpoint2;

			endpoint1 = new Vector2d(x + center.x, -y + center.y);
			endpoint2 = new Vector2d(y + center.x, -x + center.y);
			render_line(*endpoint1, *endpoint2, fill_colour, 1);
			delete endpoint1;
			delete endpoint2;

			endpoint1 = new Vector2d(-x + center.x, -y + center.y);
			endpoint2 = new Vector2d(-y + center.x, -x + center.y);
			render_line(*endpoint1, *endpoint2, fill_colour, 1);
			delete endpoint1;
			delete endpoint2;

			if (abs(F + 2*x + 1) < abs(F + 2*(x-y) + 2)) {
				x += 1;
				F += (2*x + 1);
			} else {
				x += 1;
				y -= 1;
				F += (2*(x-y) + 2);
			}
		}

		// Draw a border circle overtop of filled circle
		render_circle(center, radius, line_colour, line_thickness);
	}

	virtual void render_triangle(Vector2d point1, Vector2d point2, Vector2d point3, ColourRGB line_colour, int line_thickness, ColourRGB fill_colour){
		cout << "Triangle " << point1 << point2 << point3 << line_colour << line_thickness << fill_colour << endl;

		render_line(point1, point2, line_colour, line_thickness);
		render_line(point2, point3, line_colour, line_thickness);
		render_line(point3, point1, line_colour, line_thickness);
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