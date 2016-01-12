/* BVGRenderer.java

   BVG Renderer

   B. Bird - 01/03/2016
*/

import java.awt.Color;
import java.awt.Point;

public class BVGRenderer implements BVGRendererBase {
	public void CreateCanvas(Point dimensions, Color background_colour, int scale_factor){
		System.out.println("CreateCanvas " + dimensions + background_colour + scale_factor);
		this.width = dimensions.x;
		this.height = dimensions.y;
		canvas = new PNGCanvas(width,height);
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				canvas.SetPixel(x,y, background_colour);
		
	}
	public void RenderLine(Point endpoint1, Point endpoint2, Color colour, int thickness){
		System.out.println("RenderLine " + endpoint1 + endpoint2 + colour + thickness);
	}
	public void RenderCircle(Point center, int radius, Color line_colour, int line_thickness){
		System.out.println("RenderCircle " + center + radius + line_colour + line_thickness);
	}
	public void RenderFilledCircle(Point center, int radius, Color line_colour, int line_thickness, Color fill_colour){
		System.out.println("RenderFilledCircle " + center + radius + line_colour + line_thickness + fill_colour);
	}
	public void RenderTriangle(Point point1, Point point2, Point point3, Color line_colour, int line_thickness, Color fill_colour){
		System.out.println("RenderTriangle " + point1 + point2 + point3 + line_colour + line_thickness + fill_colour);
	}
	public void RenderGradientTriangle(Point point1, Point point2, Point point3, Color line_colour, int line_thickness, Color colour1, Color colour2, Color colour3){
		System.out.println("RenderGradientTriangle " + point1 + point2 + point3 + line_colour + line_thickness + colour1 + colour2 + colour3);
	}
	
	public void SaveImage(String filename){
		canvas.SaveImage(filename);
	}
	
	private int width,height;
	private PNGCanvas canvas;
}