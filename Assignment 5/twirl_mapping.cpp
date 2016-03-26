/* twirl_mapping.cpp
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

void apply_twirl(PNG_Canvas_BW& image, float scale = 3) {
  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW output_image(width, height);

  for (int x = 0; x < scaled_width; x++) {
    for (int y = 0; y < scaled_height; y++) {
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

  apply_twirl(canvas);
  canvas.save_image(output_filename);
}