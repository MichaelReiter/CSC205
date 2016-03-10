/* image_processor_bw.cpp
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


vector<double> inverse_gaussian_cdf(double mean = 128, double stddev = 50) {
  vector<double> histogram(256);

  for (int i = 0; i < histogram.size(); i++) {
    histogram[i] = gaussian_pdf(mean, mean, stddev)-gaussian_pdf(i, mean, stddev);
    // cout << histogram[i] << endl;
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


void invert_image(PNG_Canvas_BW& image) {
  int width = image.get_width();
  int height = image.get_height();

  //Make a new image canvas for the output to avoid conflicts
  PNG_Canvas_BW outputImage(width, height);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      outputImage[x][y] = 255 - image[x][y];
    }
  }

  //Copy the result back into the provided image
  image = outputImage;
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


vector< vector<double> > compute_gaussian_blur_matrix(double stddev = 50) {
  vector< vector<double> > matrix(5, vector<double>(5));

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      int x = j-2;
      int y = i-2;
      matrix[i][j] = 1/sum_gauss() * exp( -( (pow(x, 2.0) + pow(y, 2.0)) / (2*pow(stddev, 2.0)) ) );
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
  
  vector< vector<double> > filter = compute_gaussian_blur_matrix();
  apply_filter(canvas, filter);

  // vector<double> gaussian_dist  = gaussian_cdf();
  // vector<double> gaussian_point = create_histogram_match_point_operation(canvas, gaussian_dist);
  // apply_point_operation(canvas, gaussian_point);

  // vector<double> Href = compute_cumulative_histogram(canvas);

  canvas.save_image(output_filename);
}