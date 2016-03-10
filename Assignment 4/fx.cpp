/* fx.cpp
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

double gaussian_pdf(double x, double mean = 128, double stddev = 50) {
  return (1.0 / (stddev * sqrt(2*M_PI))) * exp(-0.5 * pow((x - mean)/stddev, 2.0));
}


vector<double> gaussian_cdf(double mean = 128, double stddev = 50) {
  vector<double> histogram(256);

  for (int i = 0; i < histogram.size(); i++) {
    histogram[i] = gaussian_pdf(i, mean, stddev);
  }

  vector<double> cdf(256, 0);
  for (int i = 1; i < cdf.size(); i++) {
    cdf[i] = cdf[i-1] + histogram[i];
  }

  return cdf;
}


vector<double> compute_histogram(PNG_Canvas_BW& image) {
  vector<double> h(256, 0);

  int width = image.get_width();
  int height = image.get_height();

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      h[image[x][y]]++;
    }
  }

  return h;
}


vector<double> compute_cumulative_histogram(PNG_Canvas_BW& image) {
  vector<double> h = compute_histogram(image);
  vector<double> H = h;

  for (int i = 1; i < h.size(); i++) {
    H[i] = H[i-1] + h[i];
  }

  int width = image.get_width();
  int height = image.get_height();
  int n = width * height;

  return H;
}


vector<double> create_histogram_match_point_operation(PNG_Canvas_BW& image, vector<double> Href, int nref = 1) {
  int width = image.get_width();
  int height = image.get_height();

  int n = width * height;
  int r = n / nref;
  vector<double> h = compute_histogram(image);

  vector<double> F(256, 0);
  int i = 0;
  int j = 0;
  int c = 0;

  while (i < 256) {
    if (c <= r * Href[j]) {
      c += h[i];
      F[i] = j;
      i++;
    } else {
      j++;
    }
  }

  return F;
}


void apply_point_operation(PNG_Canvas_BW& image, vector<double> H) {
  int width = image.get_width();
  int height = image.get_height();

  PNG_Canvas_BW outputImage(width, height);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      outputImage[x][y] = H[image[x][y]];
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
  
  vector<double> gaussian_cumulative_hist = gaussian_cdf();
  vector<double> gaussian_point_operation = create_histogram_match_point_operation(canvas, gaussian_cumulative_hist);
  apply_point_operation(canvas, gaussian_point_operation);

  canvas.save_image(output_filename);
}