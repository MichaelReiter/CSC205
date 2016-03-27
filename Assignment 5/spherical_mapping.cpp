/* spherical_mapping.cpp
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

double p = 1.8;

double inverse_transform_x(int x, int y, int width, int height) {
  int xc = width/2;
  int yc = height/2;

  int dx = x - xc;
  int dy = y - yc;

  double r_max = width / 2;

  double r = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
  double z = sqrt(pow(r_max, 2.0) - pow(r, 2.0));
  double denominator = sqrt(pow(dx, 2.0) + pow(z, 2.0));

  double beta = (1 - 1.0/p) * asin(dx / denominator);

  if (r <= r_max) {
    return x - (z * tan(beta));
  } else {
    return x;
  }
}


double inverse_transform_y(int x, int y, int width, int height) {
  int xc = width/2;
  int yc = height/2;

  int dx = x - xc;
  int dy = y - yc;

  double r_max = width / 2;

  double r = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
  double z = sqrt(pow(r_max, 2.0) - pow(r, 2.0));
  double denominator = sqrt(pow(dy, 2.0) + pow(z, 2.0));

  double beta = (1 - 1.0/p) * asin(dy / denominator);

  if (r <= r_max) {
    return y - (z * tan(beta));
  } else {
    return y;
  }
}


int check_bounds(int x, int boundary) {
  if (x < 0 || x >= boundary) {
    return 128;
  } else {
    return x;
  }
}


void apply_sphere(PNG_Canvas_BW& image, float scale = 3) {
  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW output_image(width, height);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      int x_prime = inverse_transform_x(x, y, width, height);
      int y_prime = inverse_transform_y(x, y, width, height);

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

  apply_sphere(canvas);
  canvas.save_image(output_filename);
}