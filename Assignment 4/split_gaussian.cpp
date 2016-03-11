/* split_gaussian.cpp
  CSC 205 - Spring 2016
   
  Grayscale PNG image processor.
   
  B. Bird - 03/02/2016
  Michael Reiter - 03/06/2016
*/

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
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


double sum_gauss(double stddev = 50) {
  double sum = 0;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      int x = j-2;
      int y = i-2;
      sum += exp( -( (pow(x, 2.0) + pow(y, 2.0)) / (2*pow(stddev, 2.0)) ) );
    }
  }
  return sum;
}


vector< vector<double> > compute_matrix() {
  double array[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};

  vector< vector<double> > matrix(3, vector<double>(3));

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      matrix[i][j] = array[i][j];
    }
  }

  return matrix;
}


void apply_filter(PNG_Canvas_BW& image, vector< vector<double> > filter) {
  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW outputImage(width, height);

  int filter_radius = filter.size()/2;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      double newPixel = 0;
      for (int i = -filter_radius; i <= filter_radius; i++) {
        for (int j = -filter_radius; j <= filter_radius; j++) {
          double value;
          if (x+i < 0) {
            value = image[0][y+i];
          } else if (y+j < 0) {
            value = image[x+i][0];
          } else if (x+i > width-1) {
            value = image[width-1][y];
          } else if (y+j > height-1) {
            value = image[x][height-1];
          } else {
            value = image[x+i][y+j];
          }
          newPixel += (filter[i+filter_radius][j+filter_radius] * value);
        }
      }
      outputImage[x][y] = clamp(newPixel);
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
  
  vector< vector<double> > filter;
  if (argc > 3) {
    filter = compute_gaussian_blur_matrix(atoi(argv[3]));
  } else {
    filter = compute_gaussian_blur_matrix();
  }
  apply_filter(canvas, filter);

  canvas.save_image(output_filename);
}