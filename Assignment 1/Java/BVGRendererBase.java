/* BVGRendererBase.java

   Base class for BVG renderers.

   B. Bird - 01/03/2016
*/

import java.awt.Color;
import java.awt.Point;

public interface BVGRendererBase{
	public void CreateCanvas(Point dimensions, Color background_colour, int scale_factor);
	public void RenderLine(Point endpoint1, Point endpoint2, Color colour, int thickness);
	public void RenderCircle(Point center, int radius, Color line_colour, int line_thickness);
	public void RenderFilledCircle(Point center, int radius, Color line_colour, int line_thickness, Color fill_colour);
	public void RenderTriangle(Point point1, Point point2, Point point3, Color line_colour, int line_thickness, Color fill_colour);
	public void RenderGradientTriangle(Point point1, Point point2, Point point3, Color line_colour, int line_thickness, Color colour1, Color colour2, Color colour3);
}