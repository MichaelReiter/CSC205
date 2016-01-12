/* BVGReader.java

   A parser for the BVG format. It should not be necessary to modify or
   understand the contents of this file, besides the BVGRendererBase class
   (which you should subclass for your solution).

   B. Bird - 01/03/2016
*/

import java.awt.Color;
import java.awt.Point;
import java.util.Map;
import java.util.HashMap;

import java.util.Set;
import java.util.HashSet;

import java.io.File;
import java.util.Scanner;

public class BVGReader{
	
	private Map<String,BVGCommandHandler> commands;
	private BVGRendererBase renderer;
	
	public BVGReader(BVGRendererBase renderer){
		this.renderer = renderer;
		commands = new HashMap<String,BVGCommandHandler>();
		commands.put("Canvas", new BVGCommandHandlerCanvas() );
		commands.put("Line", new BVGCommandHandlerLine() );
		commands.put("Circle", new BVGCommandHandlerCircle() );
		commands.put("FilledCircle", new BVGCommandHandlerFilledCircle() );
		commands.put("Triangle", new BVGCommandHandlerTriangle() );
		commands.put("GradientTriangle", new BVGCommandHandlerGradientTriangle() );
	}
	public boolean ParseFile(String filename){
		Scanner s;
		try{
			s = new Scanner(new File(filename));
		} catch(java.io.FileNotFoundException e){
			System.err.printf("Unable to open %s\n",filename);
			return false;
		}
		try{
			ParseInternal(s);
		}catch(ParsingException e){
			System.err.printf("Error on line %d: %s\n",e.line_number, e.message);
			if (e.position >= 0){
				System.err.printf("\t%s\n",e.line);
				System.err.printf("\t");
				for(int i = 0; i < e.position; i++)
					System.err.printf("\t");
				System.err.printf("^\n");
			}
			return false;
		}
		return true;
	}
	
	
	
	
	
	
	private String GetCommandName(String line){
		String tokens[] = line.trim().split("\\s+");
		if (tokens.length > 0)
			return tokens[0];
		return "";
	}
	
	
	private void ParseInternal(Scanner file_scanner) throws ParsingException{
		int line_number = 0;
		while(file_scanner.hasNextLine()){
			String line = file_scanner.nextLine();
			line_number++;
			if (line.trim().equals(""))
				continue;
			String command = GetCommandName(line);
			if (!commands.containsKey(command))
				throw new ParsingException(line,line_number,"Invalid shape type \""+command+"\"", 0);
			commands.get(command).Parse(renderer,line, line_number);
		}
	}
	
}










class ParsingException extends Throwable{
	public ParsingException(String line, int line_number, String message, int position){
		this.line = line;
		this.message = message;
		this.line_number = line_number;
		this.position = position;
	}
	public ParsingException(String line, int line_number, String message){
		this.line = line;
		this.message = message;
		this.line_number = line_number;
		this.position = -1;
	}
	String line, message;
	int line_number, position;
}




class RefInt{
	public RefInt(int v){
		val = v;
	}
	public int val;
}



class BVGCommandHandler{
	public void Parse(BVGRendererBase renderer, String line, int line_number) throws ParsingException{
	}
	
	String ParseKeyName(String line, int line_number, RefInt pos) throws ParsingException{
		if (pos.val >= line.length()){
			pos.val = -1;
			return "";
		}
		String name = "";
		while(pos.val < line.length()){
			char c = line.charAt(pos.val);
			if (!(Character.isLetterOrDigit(c) || c == '_'))
				break;
			name += c;
			pos.val++;
		}
		while(pos.val < line.length() && Character.isWhitespace(line.charAt(pos.val)))
			pos.val++;
		if (pos.val >= line.length() || line.charAt(pos.val) != '=')
			throw new ParsingException(line,line_number,"Expected '='",pos.val);
		return name;
	}
	
	void ParseKeys(BVGRendererBase renderer, String line, int line_number) throws ParsingException{
		parsed_keys.clear();
		for(Map.Entry<String,BVGParserBase> entry: key_parsers.entrySet()){
			entry.getValue().SetDefault();
		}
		RefInt pos = new RefInt(0);
		//Skip over the command name
		while(pos.val < line.length() && Character.isWhitespace(line.charAt(pos.val)))
			pos.val++;
		while(pos.val < line.length() && !Character.isWhitespace(line.charAt(pos.val)))
			pos.val++;
		
		while(true){
			while(pos.val < line.length() && Character.isWhitespace(line.charAt(pos.val)))
				pos.val++;
			if (pos.val >= line.length())
				break;
			int old_pos = pos.val;
			String key = ParseKeyName(line,line_number,pos);
			if (pos.val == -1)
				break;
			if (!key_parsers.containsKey(key))
				throw new ParsingException(line,line_number,"Invalid attribute name \""+key+"\"",old_pos);
			if (parsed_keys.contains(key))
				throw new ParsingException(line,line_number,"Duplicate attribute name \""+key+"\"",old_pos);
			if (pos.val >= line.length() || line.charAt(pos.val) != '=')
				throw new ParsingException(line,line_number,"Expected '='",pos.val);
			pos.val++;
			parsed_keys.add(key);
			key_parsers.get(key).Parse(line,line_number,pos);
		}
		
		for(Map.Entry<String,BVGParserBase> entry: key_parsers.entrySet()){
			String key_name = entry.getKey();
			BVGParserBase parser = entry.getValue();
			if (parser.IsRequired() && !parsed_keys.contains(key_name))
				throw new ParsingException(line,line_number,"Missing attribute \""+key_name+"\"", pos.val);
		}
	}
	
	protected HashSet<String> parsed_keys = new HashSet<String>();
	protected HashMap<String,BVGParserBase> key_parsers = new HashMap<String,BVGParserBase>();
}





class BVGCommandHandlerCanvas extends BVGCommandHandler{
	public BVGCommandHandlerCanvas(){
		key_parsers.put("dimensions", dimensions );
		key_parsers.put("background_colour", background_colour );
		key_parsers.put("scale_factor", scale_factor );
	}
	public void Parse(BVGRendererBase renderer, String line, int line_number) throws ParsingException{
		ParseKeys(renderer,line,line_number);
		renderer.CreateCanvas(dimensions.v, background_colour.colour, scale_factor.value);
	}
	BVGParserVector dimensions = new BVGParserVector();
	BVGParserColour background_colour = new BVGParserColour();
	BVGParserSingleInt scale_factor = new BVGParserSingleInt(1);
}

class BVGCommandHandlerLine extends BVGCommandHandler{
	public BVGCommandHandlerLine(){
		key_parsers.put("from", from );
		key_parsers.put("to", to );
		key_parsers.put("colour", colour );
		key_parsers.put("thickness", thickness );
	}
	public void Parse(BVGRendererBase renderer, String line, int line_number) throws ParsingException{
		ParseKeys(renderer,line,line_number);
		renderer.RenderLine(from.v, to.v, colour.colour, thickness.value);
	}
	BVGParserVector from = new BVGParserVector();
	BVGParserVector to = new BVGParserVector();
	BVGParserColour colour = new BVGParserColour();
	BVGParserSingleInt thickness = new BVGParserSingleInt(1);
}

class BVGCommandHandlerCircle extends BVGCommandHandler{
	public BVGCommandHandlerCircle(){
		key_parsers.put("center", center );
		key_parsers.put("radius", radius );
		key_parsers.put("line_colour", line_colour );
		key_parsers.put("line_thickness", line_thickness );
	}
	public void Parse(BVGRendererBase renderer, String line, int line_number) throws ParsingException{
		ParseKeys(renderer,line,line_number);
		renderer.RenderCircle(center.v, radius.value, line_colour.colour, line_thickness.value);
	}
	BVGParserVector center = new BVGParserVector();
	BVGParserSingleInt radius = new BVGParserSingleInt();
	BVGParserColour line_colour = new BVGParserColour();
	BVGParserSingleInt line_thickness = new BVGParserSingleInt(1);
}

class BVGCommandHandlerFilledCircle extends BVGCommandHandler{
	public BVGCommandHandlerFilledCircle(){
		key_parsers.put("center", center );
		key_parsers.put("radius", radius );
		key_parsers.put("line_colour", line_colour );
		key_parsers.put("line_thickness", line_thickness );
		key_parsers.put("fill_colour", fill_colour );
	}
	public void Parse(BVGRendererBase renderer, String line, int line_number) throws ParsingException{
		ParseKeys(renderer,line,line_number);
		renderer.RenderFilledCircle(center.v, radius.value, line_colour.colour, line_thickness.value, fill_colour.colour);
	}
	BVGParserVector center = new BVGParserVector();
	BVGParserSingleInt radius = new BVGParserSingleInt();
	BVGParserColour line_colour = new BVGParserColour();
	BVGParserSingleInt line_thickness = new BVGParserSingleInt(1);
	BVGParserColour fill_colour = new BVGParserColour();
}
class BVGCommandHandlerTriangle extends BVGCommandHandler{
	public BVGCommandHandlerTriangle(){
		key_parsers.put("point1", point1 );
		key_parsers.put("point2", point2 );
		key_parsers.put("point3", point3 );
		key_parsers.put("line_colour", line_colour );
		key_parsers.put("line_thickness", line_thickness );
		key_parsers.put("fill_colour", fill_colour );
	}
	public void Parse(BVGRendererBase renderer, String line, int line_number) throws ParsingException{
		ParseKeys(renderer,line,line_number);
		renderer.RenderTriangle(point1.v,point2.v,point3.v, line_colour.colour, line_thickness.value, fill_colour.colour);
	}
	BVGParserVector point1 = new BVGParserVector();
	BVGParserVector point2 = new BVGParserVector();
	BVGParserVector point3 = new BVGParserVector();
	BVGParserColour line_colour = new BVGParserColour();
	BVGParserSingleInt line_thickness = new BVGParserSingleInt(1);
	BVGParserColour fill_colour = new BVGParserColour();
}
class BVGCommandHandlerGradientTriangle extends BVGCommandHandler{
	public BVGCommandHandlerGradientTriangle(){
		key_parsers.put("point1", point1 );
		key_parsers.put("point2", point2 );
		key_parsers.put("point3", point3 );
		key_parsers.put("line_colour", line_colour );
		key_parsers.put("line_thickness", line_thickness );
		key_parsers.put("colour1", colour1 );
		key_parsers.put("colour2", colour2 );
		key_parsers.put("colour3", colour3 );
	}
	public void Parse(BVGRendererBase renderer, String line, int line_number) throws ParsingException{
		ParseKeys(renderer,line,line_number);
		renderer.RenderGradientTriangle(point1.v, point2.v, point3.v, line_colour.colour, line_thickness.value, colour1.colour,colour2.colour,colour3.colour);
	}		
	BVGParserVector point1 = new BVGParserVector();
	BVGParserVector point2 = new BVGParserVector();
	BVGParserVector point3 = new BVGParserVector();
	BVGParserColour line_colour = new BVGParserColour();
	BVGParserSingleInt line_thickness = new BVGParserSingleInt(1);
	BVGParserColour colour1 = new BVGParserColour(new Color(255,0,0));
	BVGParserColour colour2 = new BVGParserColour(new Color(0,255,0));
	BVGParserColour colour3 = new BVGParserColour(new Color(0,0,255));
}















interface BVGParserBase{
	public void Parse(String line, int line_number, RefInt pos) throws ParsingException;
	public void SetDefault();
	public boolean IsRequired();
}


class BVGParserSingleInt implements BVGParserBase{
	public BVGParserSingleInt(){
		has_default = false;
	}
	public BVGParserSingleInt(int default_value){
		has_default = true;
		this.default_value = default_value;
	}
	public void SetDefault(){
		this.value = default_value;
	}
	public boolean IsRequired(){
		return !has_default;
	}
	
	public void Parse(String line, int line_number, RefInt pos) throws ParsingException{
		while(pos.val < line.length() && Character.isWhitespace(line.charAt(pos.val)))
			pos.val++;
		String digits = "";
		while(pos.val < line.length() && Character.isDigit(line.charAt(pos.val))){
			digits += line.charAt(pos.val);
			pos.val++;
		}
		if (digits.length() == 0)
			throw new ParsingException(line,line_number,"Expected a number", pos.val);
		this.value = Integer.parseInt(digits);
	}
	
	public int value;
	
	private boolean has_default;
	private int default_value;
};


class BVGParserTuple implements BVGParserBase{
	public BVGParserTuple(int size){
		this.size = size;
		this.tuple = new int[size];
		has_default = false;
	}
	public BVGParserTuple(int size, int[] default_value){
		this.size = size;
		this.tuple = new int[size];
		has_default = true;
		this.default_value = (int[])default_value.clone();
	}
	public void SetDefault(){
		this.tuple = (int[])default_value.clone();
	}
	public boolean IsRequired(){
		return !has_default;
	}
	
	public void Parse(String line, int line_number, RefInt pos) throws ParsingException{
		while(pos.val < line.length() && Character.isWhitespace(line.charAt(pos.val)))
			pos.val++;
		if (pos.val >= line.length() || line.charAt(pos.val) != '(')
			throw new ParsingException(line,line_number,"Expected '('", pos.val);
		pos.val++;
		for(int i = 0; i < size; i++){
			while(pos.val < line.length() && Character.isWhitespace(line.charAt(pos.val)))
				pos.val++;
			String digits = "";
			while(pos.val < line.length() && Character.isDigit(line.charAt(pos.val))){
				digits += line.charAt(pos.val);
				pos.val++;
			}
			if (digits.length() == 0)
				throw new ParsingException(line,line_number,"Expected a number", pos.val);
			tuple[i] = Integer.parseInt(digits);
			while(pos.val < line.length() && Character.isWhitespace(line.charAt(pos.val)))
				pos.val++;
			if (i < size-1){
				if (pos.val >= line.length() || line.charAt(pos.val) != ',')
					throw new ParsingException(line,line_number,"Expected ','", pos.val);
				pos.val++;
			}
		}
		if (pos.val >= line.length() || line.charAt(pos.val) != ')')
			throw new ParsingException(line,line_number,"Expected ')'", pos.val);
		pos.val++;
	}
	
	public int[] tuple;
	
	protected int size;
	private boolean has_default;
	private int[] default_value;
};


class BVGParserColour extends BVGParserTuple{
	public BVGParserColour(){
		super(3);
	}
	public BVGParserColour(Color default_value){
		super(3);
		has_default = true;
		this.default_value = default_value;
	}
	public void SetDefault(){
		this.colour = default_value;
	}
	public boolean IsRequired(){
		return !has_default;
	}
	public void Parse(String line, int line_number, RefInt pos) throws ParsingException{
		super.Parse(line,line_number,pos);
		colour = new Color(tuple[0],tuple[1],tuple[2]);
	}
	public Color colour;
	
	protected int size;
	private boolean has_default;
	private Color default_value;
}


class BVGParserVector extends BVGParserTuple{
	public BVGParserVector(){
		super(2);
	}
	public BVGParserVector(Point default_value){
		super(2);
		has_default = true;
		this.default_value = default_value;
	}
	public void SetDefault(){
		this.v = default_value;
	}
	public boolean IsRequired(){
		return !has_default;
	}
	public void Parse(String line, int line_number, RefInt pos) throws ParsingException{
		super.Parse(line,line_number,pos);
		v = new Point(tuple[0],tuple[1]);
	}
	public Point v;
	
	protected int size;
	private boolean has_default;
	private Point default_value;
}
