/* draw_bvg.cpp
   
   
   B. Bird - 01/02/2016
*/

#include <string>
#include <iostream>
#include "vector2d.h"
#include "colourRGB.h"
#include "bvg.h"
#include "png_canvas.h"

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
	}
	virtual void render_circle(Vector2d center, int radius, ColourRGB line_colour, int line_thickness){
		cout << "Circle " << center << radius << line_colour << line_thickness << endl;
	}
	virtual void render_filledcircle(Vector2d center, int radius, ColourRGB line_colour, int line_thickness, ColourRGB fill_colour){
		cout << "Filled Circle " << center << radius << line_colour << line_thickness << fill_colour << endl;
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