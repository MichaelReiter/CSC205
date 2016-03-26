/* ripple_mapping.cpp
  CSC 205 - Spring 2016
   
  A PNG image processor.
  
  B. Bird - 03/02/2016
  Michael Reiter - 03/21/2016
*/

#include <string>
#include <iostream>
#include "lodepng.h"
#include "png_canvas.h"
#include <math.h>

using namespace std;

int tau_x = 120;
int tau_y = 250;
int a_x = 10;
int a_y = 15;

double inverse_transform_x(int x, int y) {
  return x + a_x * sin(2*M_PI * y / tau_x);
}


double inverse_transform_y(int x, int y) {
  return y + a_y * sin(2*M_PI * x / tau_y);
}


void apply_ripple(PNG_Canvas_BW& image, float scale = 3) {
  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW output_image(width, height);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      output_image[x][y];
    }
  }

  image = output_image;
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

  apply_ripple(canvas);
  canvas.save_image(output_filename);
}