# draw_bvg.py
#
# Starter code for CSC 205 Assignment 1 (Python)
#
# B. Bird - 01/02/2016

import sys
import bvg

import png


class PNGCanvas:
	def __init__(self, width, height):
		self.width = width
		self.height = height
		#Store each pixel as a sequence of 3 bytes (r, g, b) in a flat array
		#(This is to cater to the odd input format of the PNG encoder)
		self.pixels = [0]*width*height*3
		
	#Return a pixel value as an (R,G,B) tuple
	def get_pixel(self,x,y):
		base_index = 3*(x + y*self.width)
		return (self.pixels[base_index],self.pixels[base_index+1],self.pixels[base_index+2])
		
	#Set a pixel value to a given (R,G,B) tuple
	def set_pixel(self,x,y,colour):
		base_index = 3*(x + y*self.width)
		self.pixels[base_index] = colour[0]
		self.pixels[base_index+1] = colour[1]
		self.pixels[base_index+2] = colour[2]
		
	#Convenience operators to allow use of square brackets instead of get_pixel
	#and set_pixel
	
	def __getitem__(self,key):
		return self.get_pixel(key[0],key[1])
	def __setitem__(self,key,colour):
		self.set_pixel(key[0],key[1],colour)
		
	def save_image(self,filename):
		f = open(filename, 'wb')
		pngwriter = png.Writer(self.width, self.height,compression=9)
		pngwriter.write_array(f, self.pixels)
		f.close()

		
		

		
class BVGRenderer(bvg.BVGRendererBase):
	def create_canvas(self, dimensions, background_colour, scale_factor):
		width,height = dimensions
		self.width = width
		self.height = height
		self.canvas = PNGCanvas(width, height)
		for x in xrange(width):
			for y in xrange(height):
				self.canvas[x,y] = background_colour
				
				
	def render_line(self, endpoint1, endpoint2, colour, thickness):
		raise NotImplementedError()
			
	def render_circle(self, center, radius, line_colour, line_thickness):
		raise NotImplementedError()
		
	def render_filledcircle(self, center, radius, line_colour, line_thickness, fill_colour):
		raise NotImplementedError()
	
	def render_triangle(self, point1, point2, point3, line_colour, line_thickness, fill_colour):
		raise NotImplementedError()
	
	def render_gradient_triangle(self, point1, point2, point3, line_colour, line_thickness, colour1, colour2, colour3):
		raise NotImplementedError()
		
	def save_image(self,filename):
		self.canvas.save_image(filename)

		
if len(sys.argv) < 3:
	print >>sys.stderr,'Usage: %s <input filename> <output filename>'%(sys.argv[0])
	sys.exit(0)
	
input_filename = sys.argv[1]
output_filename = sys.argv[2]

renderer = BVGRenderer()
reader = bvg.BVGReader(renderer)
with open(input_filename) as input_file:
	if reader.parse_file(input_file):
		print >>sys.stderr,"File parsed successfully"
		renderer.save_image(output_filename)
