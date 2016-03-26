/* image_processor_bw.cpp
  CSC 205 - Spring 2016
   
  A grayscale PNG image processor.
  
  B. Bird - 03/02/2016
  Michael Reiter - 03/21/2016
*/

#include <string>
#include <iostream>
#include "lodepng.h"
#include "png_canvas.h"

using namespace std;

void process_image(PNG_Canvas_BW& image) {
  int width = image.get_width();
  int height = image.get_height();
  
  // Make a new image canvas for the output to avoid conflicts
  PNG_Canvas_BW outputImage(width,height);
  
  // Invert the image
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      outputImage[x][y] = 255 - image[x][y];
    }
  }
  
  // Copy the result back into the provided image
  image = outputImage;
}


void convert_to_binary(PNG_Canvas_BW& image) {
  int width = image.get_width();
  int height = image.get_height();
  
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (image[x][y] < 128) {
        image[x][y] = 0;
      } else {
        image[x][y] = 255;
      }
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
  
  convert_to_binary(canvas);
  canvas.save_image(output_filename);
}