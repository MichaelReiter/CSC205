/* DrawBVG.java


   B. Bird - 01/03/2016
*/

import java.awt.Color;
import java.awt.Point;


public class DrawBVG{

	public static void main(String[] args){
		if (args.length < 2){
			System.err.println("Usage: java DrawBVG <input filename> <output filename>");
			return;
		}
		String input_filename = args[0];
		String output_filename = args[1];
		
		
		BVGRenderer r = new BVGRenderer();
		BVGReader reader = new BVGReader(r);
		if (!reader.ParseFile(input_filename)){
			System.err.println("Unable to parse file");
			return;
		}
		
		r.SaveImage(output_filename);
		
	}
}