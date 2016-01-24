/* png_canvas.h
   
   B. Bird - 01/02/2016
*/

#ifndef PNG_CANVAS_H
#define PNG_CANVAS_H

#include "vector2d.h"
#include "colourRGB.h"
#include "lodepng.h"
#include <vector>
#include <string>

class PNG_Canvas{
public:
	PNG_Canvas(): width(0),height(0){
			pixels = NULL;
	}
	
	void initialize_canvas(int width, int height){
		if (pixels)
			delete[] pixels;
		pixels = new ColourRGB[width*height];
		this->width = width;
		this->height = height;
	}
	
	ColourRGB& get_pixel(int x, int y){
		return pixels[y*width + x];
	}
	
	void set_pixel(int x, int y, ColourRGB colour) {
		if (x < 0 || y < 0 || x >= width || y >= height) {
			return;
		}
		pixels[y*width + x] = colour;
	}
	
	
	//A workaround to allow relatively simple use of the square brackets
	class RowProxy{
	public:
		ColourRGB& operator[] (int y){
			return p[y*w + x];
		}	
		RowProxy(ColourRGB *pixels, int width, int height, int row): x(row), p(pixels),w(width),h(height){}
	private:
		int x;
		ColourRGB* p;
		int w,h;
	};
	RowProxy operator[] (int x){
		return RowProxy(pixels,width,height,x);
	}
	
	
	
	void save_image(std::string filename){
		std::vector<unsigned char> packed_pixels;
		packed_pixels.resize(width*height*4);
		int k = 0;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++){
				ColourRGB& pixel = get_pixel(x,y);
				packed_pixels[k++] = pixel.r;
				packed_pixels[k++] = pixel.g;
				packed_pixels[k++] = pixel.b;
				packed_pixels[k++] = 255;
			}
		lodepng::encode(filename, packed_pixels, width, height);
	}
	
private:
	int width,height;
	ColourRGB *pixels;
};


#endif