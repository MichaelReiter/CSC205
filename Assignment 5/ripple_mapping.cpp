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


int check_bounds(int x, int boundary) {
  if (x < 0 || x >= boundary) {
    return 128;
  } else {
    return x;
  }
}


void apply_ripple(PNG_Canvas_BW& image, float scale = 3) {
  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW output_image(width, height);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      int x_prime = inverse_transform_x(x, y);
      int y_prime = inverse_transform_y(x, y);

      x_prime = check_bounds(x_prime, width);
      y_prime = check_bounds(y_prime, height);

      int x0 = floor(x_prime);
      int y0 = floor(y_prime);
      int x1 = ceil(x_prime);
      int y1 = ceil(y_prime);
      float xs = x_prime - x0;
      float ys = y_prime - y0;
      float p0 = image[x0][y0] * (1 - xs) + image[x1][y0] * xs;
      float p1 = image[x0][y1] * (1 - xs) + image[x1][y1] * xs;

      output_image[x][y] = p0 * (1 - ys) + p1 * ys;
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