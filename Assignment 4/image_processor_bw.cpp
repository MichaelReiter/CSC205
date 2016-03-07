/* image_processor_bw.cpp
  CSC 205 - Spring 2016
   
	Grayscale PNG image processor.
   
  B. Bird - 03/02/2016
  Michael Reiter - 03/06/2016
*/

#include <string>
#include <iostream>
#include <vector>
#include "lodepng.h"
#include "png_canvas.h"

using namespace std;

vector<int> compute_histogram(PNG_Canvas_BW& image) {
	vector<int> h(256, 0);

	int width = image.get_width();
	int height = image.get_height();
	
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			h[image[x][y]]++;
		}
	}

	return h;
}

vector<int> compute_cumulative_histogram(PNG_Canvas_BW& image) {
	vector<int> h = compute_histogram(image);
	vector<int> H = h;

	for (int i = 1; i < h.size(); i++) {
		H[i] = H[i-1] + h[i];
	}

	return H;
}

void invert_image(PNG_Canvas_BW& image) {
	int width = image.get_width();
	int height = image.get_height();

	//Make a new image canvas for the output to avoid conflicts
	PNG_Canvas_BW outputImage(width, height);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			outputImage[x][y] = 255 - image[x][y];
		}
	}

	//Copy the result back into the provided image
	image = outputImage;
}

void match_histogram(PNG_Canvas_BW& image, int Href[]) {
	int width = image.get_width();
	int height = image.get_height();
	int n = width * height;
	vector<int> h = compute_histogram(image);
	
	PNG_Canvas_BW outputImage(width, height);
	
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			
		}
	}

	image = outputImage;
}

int main(int argc, char** argv) {
	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <output file>" << endl;
		return 0;
	}
	string input_filename = argv[1];
	string output_filename = argv[2];
	
	PNG_Canvas_BW canvas;
	if (!canvas.load_image(input_filename)) {
		cerr << "Unable to load " << input_filename << ". Exiting..." << endl;
		return 0;
	}
	
	vector<int> Href = compute_cumulative_histogram(canvas);
	// match_histogram(canvas, Href);
	canvas.save_image(output_filename);
}