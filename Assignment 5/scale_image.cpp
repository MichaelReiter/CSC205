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

using namespace std;


void scale_image(PNG_Canvas_BW& image, double scale = 3) {
  int width = image.get_width();
  int height = image.get_height();

  int scaled_width = scale * width;
  int scaled_height = scale * height;
  PNG_Canvas_BW scaled_image(scaled_width, scaled_height);

  for (int x = 0; x < scaled_width; x++) {
    for (int y = 0; y < scaled_height; y++) {
      scaled_image;
    }
  }

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
  
  scaled_image(canvas);
  canvas.save_image(output_filename);
}