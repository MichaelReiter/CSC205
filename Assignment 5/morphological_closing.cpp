/* morphological_closing.cpp
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


void convert_to_binary(PNG_Canvas_BW& image) {
  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW output_image(width,height);
  
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (image[x][y] < 128) {
        output_image[x][y] = 0;    // black
      } else {
        output_image[x][y] = 255;  // white
      }
    }
  }

  image = output_image;
}


void morphological_closing(PNG_Canvas_BW& image) {
  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW dilatedImage(width, height);
  PNG_Canvas_BW output_image(width, height);

  // Iterate over all pixels applying dilation
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (x-2 >= 0 && y-2 >= 0 && x+2 < width && y+2 < height) {
        if (image[x-1][y]   == 0 ||
            image[x-2][y]   == 0 ||
            image[x+1][y]   == 0 ||
            image[x+2][y]   == 0 ||
            image[x][y-1]   == 0 ||
            image[x][y-2]   == 0 ||
            image[x][y+1]   == 0 ||
            image[x][y+2]   == 0 ||
            image[x+1][y+1] == 0 ||
            image[x-1][y+1] == 0 ||
            image[x+1][y-1] == 0 ||
            image[x-1][y-1] == 0) {
          dilatedImage[x][y] = 0;
        } else {
          dilatedImage[x][y] = 255;
        }
      }
    }
  }

  // Iterate over all pixels applying erosion
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (x-2 >= 0 && y-2 >= 0 && x+2 < width && y+2 < height) {
        if (dilatedImage[x-1][y]   == 0 &&
            dilatedImage[x-2][y]   == 0 &&
            dilatedImage[x+1][y]   == 0 &&
            dilatedImage[x+2][y]   == 0 &&
            dilatedImage[x][y-1]   == 0 &&
            dilatedImage[x][y-2]   == 0 &&
            dilatedImage[x][y+1]   == 0 &&
            dilatedImage[x][y+2]   == 0 &&
            dilatedImage[x+1][y+1] == 0 &&
            dilatedImage[x-1][y+1] == 0 &&
            dilatedImage[x+1][y-1] == 0 &&
            dilatedImage[x-1][y-1] == 0) {
          output_image[x][y] = 0;
        } else {
          output_image[x][y] = 255;
        }
      } else {
        output_image[x][y] = image[x][y];
      }
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
  
  convert_to_binary(canvas);
  morphological_closing(canvas);
  canvas.save_image(output_filename);
}