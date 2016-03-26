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


int clamp(double intensity) {
  if (intensity > 255) {
    return 255;
  } else if (intensity < 0) {
    return 0;
  } else {
    return intensity;
  }
}


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

  PNG_Canvas_BW outputImage(width,height);
  
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (image[x][y] < 128) {
        outputImage[x][y] = 0;    // black
      } else {
        outputImage[x][y] = 255;  // white
      }
    }
  }

  image = outputImage;
}


void morphological_closing(PNG_Canvas_BW& image) {
  // bool h[5][5] = {
  //   {false, false,  true, false, false},
  //   {false,  true,  true,  true, false},
  //   { true,  true, false,  true,  true},
  //   {false,  true,  true,  true, false},
  //   {false, false,  true, false, false}
  // };

  bool h[3][3] = {
    {false, true, false},
    { true, true,  true},
    {false, true, false}
  };

  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW outputImage(width, height);

  int radius = 1;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      outputImage[x][y] = image[x][y];
    }
  }

  // Iterate over all pixels applying erosion
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {

      bool should_break = false;

      for (int i = -radius; i <= radius; i++) {
        if (should_break) {
          break;
        }

        for (int j = -radius; j <= radius; j++) {
          // if (x+i >= 0 && x+i < width && y+j >= 0 && y+j < height) {
            
            if (h[x+i][y+j] && image[x+i][y+j] != 255) {
              should_break = true;
              break;
            }

          // }
        }
      }

      if (!should_break) {
        outputImage[x][y] = 0;
      }
    }
  }

  image = outputImage;
}


void scale_image() {

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