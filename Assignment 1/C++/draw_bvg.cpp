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
		scale = scale_factor;
		width = dimensions[0] * scale;
		height = dimensions[1] * scale;
		canvas.initialize_canvas(width, height);
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
			startpoint = endpoint1 * scale;
			endpoint = endpoint2 * scale;
		} else {
			startpoint = endpoint2 * scale;
			endpoint = endpoint1 * scale;
		}

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
			while (x <= endpoint.x) {
				// draw points
				if (thickness > 1) {
					int increment = thickness/2;
					Vector2d *newEndpoint1 = new Vector2d(x, y + increment);
					Vector2d *newEndpoint2;

					if (thickness % 2 == 1) {
						newEndpoint2 = new Vector2d(x, y - increment - 1);
					} else {
						newEndpoint2 = new Vector2d(x, y - increment);
					}
					render_line(*newEndpoint1, *newEndpoint2, colour, 1);
					delete newEndpoint1;
					delete newEndpoint2;
				} else {
					canvas.set_pixel(x, y, colour);
				}

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
			while (y != endpoint.y) {
				// draw points
				if (thickness > 1) {
					int increment = thickness/2;
					Vector2d *newEndpoint1 = new Vector2d(x + increment, y);
					Vector2d *newEndpoint2;

					if (thickness % 2 == 1) {
						newEndpoint2 = new Vector2d(x - increment, y);
					} else {
						newEndpoint2 = new Vector2d(x - increment + 1, y);
					}
					render_line(*newEndpoint1, *newEndpoint2, colour, 1);
					delete newEndpoint1;
					delete newEndpoint2;
				} else {
					canvas.set_pixel(x, y, colour);
				}

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
		int y = radius * scale;

		while (x <= y) {
			// draw points
			if (line_thickness > 1) {
				int increment = line_thickness/2;
				Vector2d *newEndpoint1;
				Vector2d *newEndpoint2;

				newEndpoint1 = new Vector2d(x + center.x, y + center.y + increment);
				if (line_thickness % 2 == 1) {
					newEndpoint2 = new Vector2d(x + center.x, y + center.y - increment - 1);
				} else {
					newEndpoint2 = new Vector2d(x + center.x, y + center.y - increment);
				}
				render_line(*newEndpoint1, *newEndpoint2, line_colour, 1);
				delete newEndpoint1;
				delete newEndpoint2;

				newEndpoint1 = new Vector2d(y + center.x + increment, x + center.y);
				if (line_thickness % 2 == 1) {
					newEndpoint2 = new Vector2d(y + center.x - increment, x + center.y);
				} else {
					newEndpoint2 = new Vector2d(y + center.x - increment + 1, x + center.y);
				}
				render_line(*newEndpoint1, *newEndpoint2, line_colour, 1);
				delete newEndpoint1;
				delete newEndpoint2;

				newEndpoint1 = new Vector2d(-x + center.x, y + center.y + increment);
				if (line_thickness % 2 == 1) {
					newEndpoint2 = new Vector2d(-x + center.x, y + center.y - increment - 1);
				} else {
					newEndpoint2 = new Vector2d(-x + center.x, y + center.y - increment);
				}
				render_line(*newEndpoint1, *newEndpoint2, line_colour, 1);
				delete newEndpoint1;
				delete newEndpoint2;

				newEndpoint1 = new Vector2d(-y + center.x + increment, x + center.y);
				if (line_thickness % 2 == 1) {
					newEndpoint2 = new Vector2d(-y + center.x - increment, x + center.y);
				} else {
					newEndpoint2 = new Vector2d(-y + center.x - increment + 1, x + center.y);
				}
				render_line(*newEndpoint1, *newEndpoint2, line_colour, 1);
				delete newEndpoint1;
				delete newEndpoint2;

				newEndpoint1 = new Vector2d(x + center.x, -y + center.y + increment);
				if (line_thickness % 2 == 1) {
					newEndpoint2 = new Vector2d(x + center.x, -y + center.y - increment - 1);
				} else {
					newEndpoint2 = new Vector2d(x + center.x, -y + center.y - increment);
				}
				render_line(*newEndpoint1, *newEndpoint2, line_colour, 1);
				delete newEndpoint1;
				delete newEndpoint2;

				newEndpoint1 = new Vector2d(y + center.x + increment, -x + center.y);
				if (line_thickness % 2 == 1) {
					newEndpoint2 = new Vector2d(y + center.x - increment, -x + center.y);
				} else {
					newEndpoint2 = new Vector2d(y + center.x - increment + 1, -x + center.y);
				}
				render_line(*newEndpoint1, *newEndpoint2, line_colour, 1);
				delete newEndpoint1;
				delete newEndpoint2;

				newEndpoint1 = new Vector2d(-x + center.x, -y + center.y + increment);
				if (line_thickness % 2 == 1) {
					newEndpoint2 = new Vector2d(-x + center.x, -y + center.y - increment - 1);
				} else {
					newEndpoint2 = new Vector2d(-x + center.x, -y + center.y - increment);
				}
				render_line(*newEndpoint1, *newEndpoint2, line_colour, 1);
				delete newEndpoint1;
				delete newEndpoint2;

				newEndpoint1 = new Vector2d(-y + center.x + increment, -x + center.y);
				if (line_thickness % 2 == 1) {
					newEndpoint2 = new Vector2d(-y + center.x - increment, -x + center.y);
				} else {
					newEndpoint2 = new Vector2d(-y + center.x - increment + 1, -x + center.y);
				}
				render_line(*newEndpoint1, *newEndpoint2, line_colour, 1);
				delete newEndpoint1;
				delete newEndpoint2;

			} else {
				canvas.set_pixel(x + center.x * scale, y + center.y * scale, line_colour);
				canvas.set_pixel(y + center.x * scale, x + center.y * scale, line_colour);
				canvas.set_pixel(-x + center.x * scale, y + center.y * scale, line_colour);
				canvas.set_pixel(-y + center.x * scale, x + center.y * scale, line_colour);
				canvas.set_pixel(x + center.x * scale, -y + center.y * scale, line_colour);
				canvas.set_pixel(y + center.x * scale, -x + center.y * scale, line_colour);
				canvas.set_pixel(-x + center.x * scale, -y + center.y * scale, line_colour);
				canvas.set_pixel(-y + center.x * scale, -x + center.y * scale, line_colour);
			}

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
		int y = radius * scale;

		while (x <= y) {
			// draw points
			Vector2d *endpoint1;
			Vector2d *endpoint2;

			endpoint1 = new Vector2d(x + center.x * scale, y + center.y * scale);
			endpoint2 = new Vector2d(x + center.x * scale, -y + center.y * scale);
			render_line(*endpoint1 / scale, *endpoint2 / scale, fill_colour, 1);
			delete endpoint1;
			delete endpoint2;

			endpoint1 = new Vector2d(-x + center.x * scale, y + center.y * scale);
			endpoint2 = new Vector2d(-x + center.x * scale, -y + center.y * scale);
			render_line(*endpoint1 / scale, *endpoint2 / scale, fill_colour, 1);
			delete endpoint1;
			delete endpoint2;

			endpoint1 = new Vector2d(y + center.x * scale, x + center.y * scale);
			endpoint2 = new Vector2d(-y + center.x * scale, x + center.y * scale);
			render_line(*endpoint1 / scale, *endpoint2 / scale, fill_colour, 1);
			delete endpoint1;
			delete endpoint2;

			endpoint1 = new Vector2d(y + center.x * scale, -x + center.y * scale);
			endpoint2 = new Vector2d(-y + center.x * scale, -x + center.y * scale);
			render_line(*endpoint1 / scale, *endpoint2 / scale, fill_colour, 1);
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

		Vector2d scaledPoint1 = point1 * scale;
		Vector2d scaledPoint2 = point2 * scale;
		Vector2d scaledPoint3 = point3 * scale;

		// Fill triangle
		int x0 = min(min(scaledPoint1.x, scaledPoint2.x), scaledPoint3.x);
		int x1 = max(max(scaledPoint1.x, scaledPoint2.x), scaledPoint3.x);
		int y0 = min(min(scaledPoint1.y, scaledPoint2.y), scaledPoint3.y);
		int y1 = max(max(scaledPoint1.y, scaledPoint2.y), scaledPoint3.y);

		float lambda1, lambda2, lambda3;

		Vector2d *v = new Vector2d(scaledPoint2.y - scaledPoint3.y, scaledPoint3.x - scaledPoint2.x);
		Vector2d *u = new Vector2d(scaledPoint1.x - scaledPoint3.x, scaledPoint1.y - scaledPoint3.y);
		Vector2d *w = new Vector2d(scaledPoint3.y - scaledPoint1.y, scaledPoint1.x - scaledPoint3.x);
		Vector2d *vector1;

		// Iterate over all pixels in bounding box
		for (int i = x0; i <= x1; i++) {
			for (int j = y0; j <= y1; j++) {
				vector1 = new Vector2d(i - scaledPoint3.x, j - scaledPoint3.y);

				lambda1 = v->dot(*vector1) / v->dot(*u);
				lambda2 = w->dot(*vector1) / v->dot(*u);
				lambda3 = 1 - lambda1 - lambda2;

				delete vector1;

				if (lambda1 > 0 && lambda2 > 0 && lambda3 > 0) {
					canvas.set_pixel(i, j, fill_colour);
				}
			}
		}

		delete v;
		delete u;
		delete w;

		// Draw outline
		render_line(point1, point2, line_colour, line_thickness);
		render_line(point2, point3, line_colour, line_thickness);
		render_line(point3, point1, line_colour, line_thickness);
	}

	virtual void render_gradient_triangle(Vector2d point1, Vector2d point2, Vector2d point3, ColourRGB line_colour, int line_thickness, ColourRGB colour1, ColourRGB colour2, ColourRGB colour3){
		cout << "Triangle " << point1 << point2 << point3 << line_colour << line_thickness << colour1 << colour2 << colour3 << endl;

		Vector2d scaledPoint1 = point1 * scale;
		Vector2d scaledPoint2 = point2 * scale;
		Vector2d scaledPoint3 = point3 * scale;

		// Fill triangle
		int x0 = min(min(scaledPoint1.x, scaledPoint2.x), scaledPoint3.x);
		int x1 = max(max(scaledPoint1.x, scaledPoint2.x), scaledPoint3.x);
		int y0 = min(min(scaledPoint1.y, scaledPoint2.y), scaledPoint3.y);
		int y1 = max(max(scaledPoint1.y, scaledPoint2.y), scaledPoint3.y);

		float lambda1, lambda2, lambda3;

		Vector2d *v = new Vector2d(scaledPoint2.y - scaledPoint3.y, scaledPoint3.x - scaledPoint2.x);
		Vector2d *u = new Vector2d(scaledPoint1.x - scaledPoint3.x, scaledPoint1.y - scaledPoint3.y);
		Vector2d *w = new Vector2d(scaledPoint3.y - scaledPoint1.y, scaledPoint1.x - scaledPoint3.x);
		Vector2d *vector1;

		// Iterate over all pixels in bounding box
		for (int i = x0; i <= x1; i++) {
			for (int j = y0; j <= y1; j++) {
				vector1 = new Vector2d(i - scaledPoint3.x, j - scaledPoint3.y);

				lambda1 = v->dot(*vector1) / v->dot(*u);
				lambda2 = w->dot(*vector1) / v->dot(*u);
				lambda3 = 1 - lambda1 - lambda2;

				int r = lambda1 * colour1.r + lambda2 * colour2.r + lambda3 * colour3.r;
				int g = lambda2 * colour1.g + lambda2 * colour2.g + lambda3 * colour3.g;
				int b = lambda1 * colour1.b + lambda2 * colour2.b + lambda3 * colour3.b;

				// int r = lambda1 * (colour1.r + colour2.r + colour3.r);
				// int g = lambda2 * (colour1.g + colour2.g + colour3.g);
				// int b = lambda3 * (colour1.b + colour2.b + colour3.b);

				ColourRGB *fill_colour = new ColourRGB(r, g, b);

				if (lambda1 > 0 && lambda2 > 0 && lambda3 > 0) {
					canvas.set_pixel(i, j, *fill_colour);
				}

				delete vector1;
				delete fill_colour;
			}
		}

		delete v;
		delete u;
		delete w;

		// Draw outline
		render_line(point1, point2, line_colour, line_thickness);
		render_line(point2, point3, line_colour, line_thickness);
		render_line(point3, point1, line_colour, line_thickness);
	}
	
	void save_image(string filename){
		canvas.save_image(filename);
	}
private:
	PNG_Canvas canvas;
	int width, height, scale;
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