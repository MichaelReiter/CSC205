/* png_canvas.h
   
   B. Bird - 01/02/2016
*/

#ifndef PNG_CANVAS_H
#define PNG_CANVAS_H

#include "lodepng.h"
#include <vector>
#include <string>

class PNG_Canvas_BW{
public:
	PNG_Canvas_BW(): width(0),height(0){
			pixels = NULL;
	}
	PNG_Canvas_BW(int w, int h): width(0),height(0){
			pixels = NULL;
			initialize_canvas(w,h);
	}
	
	void initialize_canvas(int width, int height){
		if (pixels)
			delete[] pixels;
		pixels = new int[width*height];
		this->width = width;
		this->height = height;
	}
	
	int& get_pixel(int x, int y) const{
		return pixels[y*width + x];
	}
	
	void set_pixel(int x, int y, int intensity){
		pixels[y*width + x] = intensity;
	}
	
	
	//A workaround to allow relatively simple use of the square brackets
	class RowProxy{
	public:
		int& operator[] (int y){
			return p[y*w + x];
		}	
		RowProxy(int *pixels, int width, int height, int row): x(row), p(pixels),w(width),h(height){}
	private:
		int x;
		int* p;
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
				int& pixel = get_pixel(x,y);
				packed_pixels[k++] = pixel;
				packed_pixels[k++] = pixel;
				packed_pixels[k++] = pixel;
				packed_pixels[k++] = 255;
			}
		lodepng::encode(filename, packed_pixels, width, height);
	}
	
	bool load_image(std::string filename){
		std::vector<unsigned char> packed_pixels;
		unsigned int w, h;
		unsigned int error = lodepng::decode(packed_pixels, w, h, filename);
		if (error)
			return false;
		initialize_canvas(w,h);
		int k = 0;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++){
				int r = packed_pixels[k++];
				int g = packed_pixels[k++];
				int b = packed_pixels[k++];
				//int a = packed_pixels[k++];
				k++; //Skip the alpha component
				set_pixel(x,y,(r+g+b)/3);
			}
		return true;
	}
	
	PNG_Canvas_BW& operator=(const PNG_Canvas_BW& other){
		initialize_canvas(other.width,other.height);
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				set_pixel(x,y,other.get_pixel(x,y));
		return *this;
	}
	
	PNG_Canvas_BW(const PNG_Canvas_BW& other){
		initialize_canvas(other.width,other.height);
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				set_pixel(x,y,other.get_pixel(x,y));
	}
	
	int get_width(){ return width; }
	int get_height(){ return height; }
	
private:
	int width,height;
	int *pixels;
};


#endif