/* scale_image.cpp
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

void scale_image(PNG_Canvas_BW& image, float scale = 3) {
  int width = image.get_width();
  int height = image.get_height();

  int scaled_width = scale * width;
  int scaled_height = scale * height;
  PNG_Canvas_BW scaled_image(scaled_width, scaled_height);

  for (int x = 0; x < scaled_width; x++) {
    for (int y = 0; y < scaled_height; y++) {
      float x_prime = x * width / scaled_width;
      float y_prime = y * height / scaled_height;

      int x0 = floor(x_prime);
      int y0 = floor(y_prime);
      int x1 = ceil(x_prime);
      int y1 = ceil(y_prime);
      float xs = x_prime - x0;
      float ys = y_prime - y0;
      float p0 = image[x0][y0] * (1 - xs) + image[x1][y0] * xs;
      float p1 = image[x0][y1] * (1 - xs) + image[x1][y1] * xs;

      scaled_image[x][y] = p0 * (1 - ys) + p1 * ys;
    }
  }

  image = scaled_image;
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

  if (argc > 3) {
    scale_image(canvas, atof(argv[3]));
  } else {
    scale_image(canvas);
  }

  canvas.save_image(output_filename);
}