/* bvg.h
   
   A parser for the BVG format. It should not be necessary to modify or
   understand the contents of this file, besides the BVGRendererBase class
   (which you should subclass for your solution).
   
   B. Bird - 01/02/2016
*/

#ifndef BVG_H
#define BVG_H

#include "vector2d.h"
#include "colourRGB.h"
#include <map>

class BVGRendererBase{
public:
	virtual void create_canvas(Vector2d dimensions, ColourRGB background_colour, int scale_factor) = 0;
	virtual void render_line(Vector2d endpoint1, Vector2d endpoint2, ColourRGB colour, int thickness) = 0;
	virtual void render_circle(Vector2d center, int radius, ColourRGB line_colour, int line_thickness) = 0;
	virtual void render_filledcircle(Vector2d center, int radius, ColourRGB line_colour, int line_thickness, ColourRGB fill_colour) = 0;
	virtual void render_triangle(Vector2d point1, Vector2d point2, Vector2d point3, ColourRGB line_colour, int line_thickness, ColourRGB fill_colour) = 0;
	virtual void render_gradient_triangle(Vector2d point1, Vector2d point2, Vector2d point3, ColourRGB line_colour, int line_thickness, ColourRGB colour1, ColourRGB colour2, ColourRGB colour3) = 0;
};

class BVGCommandHandler;
class BVGReader{
public:
	
	BVGReader(BVGRendererBase& r);
	virtual ~BVGReader();
	
	bool parse_file(std::string filename);
private:
	BVGRendererBase& renderer;
	std::map<std::string,BVGCommandHandler*> commands;

	void parse_internal(FILE* f);
};

#endif