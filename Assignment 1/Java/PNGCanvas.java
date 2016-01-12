/* PNGCanvas.java


   B. Bird - 01/03/2016
*/

import java.awt.Color;
import java.awt.Point;
import javax.imageio.ImageIO;
import java.awt.Dimension;
import java.awt.image.BufferedImage;
import java.io.File;


public class PNGCanvas{
	public PNGCanvas(int width, int height){
		this.width = width;
		this.height = height;
		pixels = new Color[width][height];
	}
	
	public Color GetPixel(int x, int y){
		return pixels[x][y];
	}
	public void SetPixel(int x, int y, Color colour){
		//Normally we would want to make a copy of the colour,
		//but Color objects are immutable.
		pixels[x][y] = colour;
	}
	
	public void SaveImage(String filename){
		BufferedImage outputImage = new BufferedImage(width,height,BufferedImage.TYPE_INT_RGB);
		for (int x = 0; x < width; x++)
			for(int y = 0; y < height; y++)
				outputImage.setRGB(x,y,pixels[x][y].getRGB());
		
		try{
			ImageIO.write(outputImage, "png", new File(filename));
		}catch(java.io.IOException e){
			System.err.printf("Unable to write %s: %s\n",filename,e.getMessage());
			return;
		}
		System.err.printf("Wrote a %d by %d image\n",width,height);
	}
	
	private int width, height;
	private Color[][] pixels;
}