/* bvg.cpp
   
   A parser for the BVG format. It should not be necessary to modify or
   understand the contents of this file, besides the BVGRendererBase class
   (which you should subclass for your solution).
   
   B. Bird - 01/02/2016
*/

#include <string>
#include <iostream>
#include <map>
#include <set>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include "vector2d.h"
#include "colourRGB.h"
#include "bvg.h"

using namespace std;

static const int MAX_LINE = 10000;

class ParsingException{
public:
	ParsingException(string line, int line_number, string message, int position=-1){
		this->line = line;
		this->line_number = line_number;
		this->message = message;
		this->position = position;
	}
	string line, message;
	int line_number,position;
};










class BVGParserBase{
public:
	virtual ~BVGParserBase() { }
	virtual void parse(string line, int line_number, unsigned int &pos) = 0;
	virtual void set_default() = 0;
	virtual bool required() = 0;
};

class BVGParserSingleInt: public BVGParserBase{
public:
	BVGParserSingleInt(bool has_default, int default_value){
		this->has_default = has_default;
		this->default_value = default_value;
	}
	virtual void parse(string line, int line_number, unsigned int &pos){
		while (pos < line.length() && isspace(line[pos]))
			pos++;
		char* raw_line = const_cast<char*>(line.c_str());
		char* s = &raw_line[pos];
		char* next = NULL;
		int value = strtol(s,&next,10);
		this->value = value;
		if (next == s)
			throw ParsingException(line,line_number,"Expected a number", pos);
		s = next;
		pos = s - raw_line;
	}
	
	virtual void set_default(){
		value = default_value;
	}
	virtual bool required(){
		return !has_default;
	}
	
	int value;
private:
	bool has_default;
	int default_value;
};



template<int size>
class BVGParserTuple: public BVGParserBase{
public:
	BVGParserTuple(int default_value[]){
		this->has_default = (default_value != NULL);
		if (default_value){
			for(int i = 0; i < size; i++)
				this->default_value[i] = default_value[i];
		}
	}
	virtual void parse(string line, int line_number, unsigned int &pos){
		while (pos < line.length() && isspace(line[pos]))
			pos++;
		if (pos >= line.length() || line[pos] != '(')
			throw ParsingException(line,line_number,"Expected '('", pos);
		pos++;
		char* raw_line = const_cast<char*>(line.c_str());
		for(int i = 0; i < size; i++){
			char* s = &raw_line[pos];
			char* next = NULL;
			int value = strtol(s,&next,10);
			tuple[i] = value;
			if (next == s)
				throw ParsingException(line,line_number,"Expected a number", pos);
			s = next;
			pos = s - raw_line;
			while (pos < line.length() && isspace(line[pos]))
				pos++;
			if (i < size-1){
				if ( (pos >= line.length() || line[pos] != ',') )
					throw ParsingException(line,line_number,"Expected ','", pos);
				pos++;
			}
		}
		if(pos >= line.length() || line[pos] != ')')
			throw ParsingException(line,line_number,"Expected ')'", pos);
		pos++;
	}
	
	virtual void set_default(){
		for(int i = 0; i < size; i++)
			tuple[i] = default_value[i];
	}
	virtual bool required(){
		return !has_default;
	}
	
	int tuple[size];
private:
	bool has_default;
	int default_value[size];
};

class BVGParserColour: public BVGParserTuple<3>{
public:
	BVGParserColour(): BVGParserTuple(NULL){
		this->has_default = false;
	}
	BVGParserColour(ColourRGB default_value): BVGParserTuple(NULL){
		this->has_default = true;
		this->default_value = default_value;
	}
	virtual void parse(string line, int line_number, unsigned int &pos){
		BVGParserTuple::parse(line,line_number,pos);
		colour.setRGB(tuple[0],tuple[1],tuple[2]);
	}
	
	virtual void set_default(){
		colour = default_value;
	}
	virtual bool required(){
		return !has_default;
	}
	
	ColourRGB colour;
private:
	bool has_default;
	ColourRGB default_value;
};

class BVGParserVector: public BVGParserTuple<2>{
public:
	BVGParserVector(): BVGParserTuple(NULL){
		this->has_default = false;
	}
	BVGParserVector(Vector2d default_value): BVGParserTuple(NULL){
		this->has_default = true;
		this->default_value = default_value;

	}
	virtual void parse(string line, int line_number, unsigned int &pos){
		BVGParserTuple::parse(line,line_number,pos);
		v.x = tuple[0];
		v.y = tuple[1];
	}
	
	virtual void set_default(){
		v = default_value;
	}
	virtual bool required(){
		return !has_default;
	}
	
	Vector2d v;
private:
	bool has_default;
	Vector2d default_value;
};















class BVGCommandHandler{
public:
	virtual ~BVGCommandHandler(){
		//The parser instances are all locally allocated
		//for (map<string,BVGParserBase*>::iterator i=key_parsers.begin(); i != key_parsers.end(); i++)
		//	delete i->second;
	}
	
	virtual void parse(BVGRendererBase& renderer, string line, int line_number) = 0;
	
	string parse_key_name(string line, int line_number, unsigned int &pos){
		if (pos >= line.length()){
			pos = 0x7fffffff; //Bad idea
			return "";
		}
		string name;
		while (pos < line.length() && (isalpha(line[pos]) || isdigit(line[pos]) || line[pos] == '_')){
			name += line[pos];
			pos++;
		}
		while (pos < line.length() && isspace(line[pos]))
			pos++;
		if (pos >= line.length() || line[pos] != '=')
			throw ParsingException(line,line_number,"Expected '='", pos);
		return name;
	}
	void parse_keys(BVGRendererBase& renderer, string line, int line_number){
		parsed_keys.clear();
		for (map<string,BVGParserBase*>::iterator i=key_parsers.begin(); i != key_parsers.end(); i++)
			i->second->set_default();
		unsigned int pos = 0;
		//Skip over the command name
		while (pos < line.length() && isspace(line[pos]))
			pos++;
		while (pos < line.length() && !isspace(line[pos]))
			pos++;
			
		while(1){
			while (pos < line.length() && isspace(line[pos]))
				pos++;
			if (pos >= line.length())
				break;
			unsigned int next_pos = pos;
			string key = parse_key_name(line,line_number,next_pos);
			if (next_pos == 0x7fffffff)
				break;
			if (key_parsers.find(key) == key_parsers.end())
				throw ParsingException(line,line_number,"Invalid attribute name \""+key+"\"", pos);
			if (parsed_keys.find(key) != parsed_keys.end())
				throw ParsingException(line,line_number,"Duplicate attribute name \""+key+"\"", pos);
			if (next_pos >= line.length() || line[next_pos] != '=')
				throw ParsingException(line,line_number,"Expected '='", pos);
			next_pos++;
			parsed_keys.insert(key);
			key_parsers[key]->parse(line,line_number,next_pos);
			pos = next_pos;
		}
		
		for(map<string,BVGParserBase*>::iterator i = key_parsers.begin(); i != key_parsers.end(); i++){
			string key_name = i->first;
			BVGParserBase* parser = i->second;
			if (parsed_keys.find(key_name) == parsed_keys.end() && parser->required() ){
				throw ParsingException(line,line_number,"Missing attribute \""+key_name+"\"", pos);
			}
		}
	}
protected:
	set<string> parsed_keys;
	map<string,BVGParserBase*> key_parsers;
};

	
	
	
	

class BVGCommandHandlerCanvas: public BVGCommandHandler{
public:
	BVGCommandHandlerCanvas(): scale_factor(true,1) {
		key_parsers["dimensions"] = &dimensions;
		key_parsers["background_colour"] = &background_colour;
		key_parsers["scale_factor"] = &scale_factor;
	}
	virtual void parse(BVGRendererBase& renderer, string line, int line_number){
		BVGCommandHandler::parse_keys(renderer,line,line_number);
		renderer.create_canvas(
					dimensions.v,
					background_colour.colour,
					scale_factor.value
					);
	}
private:
	BVGParserVector dimensions;
	BVGParserColour background_colour;
	BVGParserSingleInt scale_factor;
};

class BVGCommandHandlerLine: public BVGCommandHandler{
public:
	BVGCommandHandlerLine(): thickness(true,1){
		key_parsers["from"] = &from;
		key_parsers["to"] = &to;
		key_parsers["colour"] = &colour;
		key_parsers["thickness"] = &thickness;
	}
	virtual void parse(BVGRendererBase& renderer, string line, int line_number){
		BVGCommandHandler::parse_keys(renderer,line,line_number);
		renderer.render_line(from.v, to.v, colour.colour, thickness.value);
	}
private:
	BVGParserVector from;
	BVGParserVector to;
	BVGParserColour colour;
	BVGParserSingleInt thickness;
};

class BVGCommandHandlerCircle: public BVGCommandHandler{
public:
	BVGCommandHandlerCircle(): radius(false,-1), line_thickness(true,1){
		key_parsers["center"] = &center;
		key_parsers["radius"] = &radius;
		key_parsers["line_colour"] = &line_colour;
		key_parsers["line_thickness"] = &line_thickness;
	}
	virtual void parse(BVGRendererBase& renderer, string line, int line_number){
		BVGCommandHandler::parse_keys(renderer,line,line_number);
		renderer.render_circle(center.v, radius.value, line_colour.colour, line_thickness.value);
	}
private:
	BVGParserVector center;
	BVGParserSingleInt radius;
	BVGParserColour line_colour;
	BVGParserSingleInt line_thickness;
};

class BVGCommandHandlerFilledCircle: public BVGCommandHandler{
public:
	BVGCommandHandlerFilledCircle():radius(false,-1), line_thickness(true,1){
		key_parsers["center"] = &center;
		key_parsers["radius"] = &radius;
		key_parsers["line_colour"] = &line_colour;
		key_parsers["line_thickness"] = &line_thickness;
		key_parsers["fill_colour"] = &fill_colour;
	}
	virtual void parse(BVGRendererBase& renderer, string line, int line_number){
		BVGCommandHandler::parse_keys(renderer,line,line_number);
		renderer.render_filledcircle(center.v, radius.value, line_colour.colour, line_thickness.value, fill_colour.colour);
	}
private:
	BVGParserVector center;
	BVGParserSingleInt radius;
	BVGParserColour line_colour;
	BVGParserSingleInt line_thickness;
	BVGParserColour fill_colour;
};

class BVGCommandHandlerTriangle: public BVGCommandHandler{
public:
	BVGCommandHandlerTriangle(): line_thickness(true,1){
		key_parsers["point1"] = &point1;
		key_parsers["point2"] = &point2;
		key_parsers["point3"] = &point3;
		key_parsers["line_colour"] = &line_colour;
		key_parsers["line_thickness"] = &line_thickness;
		key_parsers["fill_colour"] = &fill_colour;
	}
	virtual void parse(BVGRendererBase& renderer, string line, int line_number){
		BVGCommandHandler::parse_keys(renderer,line,line_number);
		renderer.render_triangle(point1.v,point2.v,point3.v, line_colour.colour, line_thickness.value, fill_colour.colour);
	}
private:
	BVGParserVector point1;
	BVGParserVector point2;
	BVGParserVector point3;
	BVGParserColour line_colour;
	BVGParserSingleInt line_thickness;
	BVGParserColour fill_colour;
};

class BVGCommandHandlerGradientTriangle: public BVGCommandHandler{
public:
	BVGCommandHandlerGradientTriangle(): 
						line_thickness(true,1),
						colour1(ColourRGB(255,0,0)),
						colour2(ColourRGB(0,255,0)),
						colour3(ColourRGB(0,0,255))
						{
		key_parsers["point1"] = &point1;
		key_parsers["point2"] = &point2;
		key_parsers["point3"] = &point3;
		key_parsers["line_colour"] = &line_colour;
		key_parsers["line_thickness"] = &line_thickness;
		key_parsers["colour1"] = &colour1;
		key_parsers["colour2"] = &colour2;
		key_parsers["colour3"] = &colour3;
	}
	virtual void parse(BVGRendererBase& renderer, string line, int line_number){
		BVGCommandHandler::parse_keys(renderer,line,line_number);
		renderer.render_gradient_triangle(point1.v, point2.v, point3.v, line_colour.colour, line_thickness.value, colour1.colour,colour2.colour,colour3.colour);
	}
private:		
	BVGParserVector point1;
	BVGParserVector point2;
	BVGParserVector point3;
	BVGParserColour line_colour;
	BVGParserSingleInt line_thickness;
	BVGParserColour colour1;
	BVGParserColour colour2;
	BVGParserColour colour3;
};
	
	
	
	
	
	
	
BVGReader::BVGReader(BVGRendererBase& r): renderer(r){
	commands["Canvas"] = new BVGCommandHandlerCanvas;
	commands["Line"] = new BVGCommandHandlerLine;
	commands["Circle"] = new BVGCommandHandlerCircle;
	commands["FilledCircle"] = new BVGCommandHandlerFilledCircle;
	commands["Triangle"] = new BVGCommandHandlerTriangle;
	commands["GradientTriangle"] = new BVGCommandHandlerGradientTriangle;
}

BVGReader::~BVGReader(){
	for (map<string,BVGCommandHandler*>::iterator i=commands.begin(); i != commands.end(); i++)
		delete i->second;
}
	
bool BVGReader::parse_file(string filename){
	FILE* f = fopen(filename.c_str(), "r");
	if (!f){
		cerr << "Unable to open file " << filename << endl;
		return false;
	}
	try{
		parse_internal(f);
		fclose(f);
	}catch(ParsingException& e){
		std::cerr << "Error on line " << e.line_number << ": " << e.message << std::endl;
		if (e.position >= 0){
			std::cerr << "\t" << e.line << std::endl;
			std::cerr << "\t";
			for(int i = 0; i < e.position; i++)
				std::cerr << " ";
			std::cerr << "^" << endl;
		}
		fclose(f);
		return false;
	}
	return true;
}


string get_command_name(string line){
	char command[MAX_LINE];
	unsigned int i = 0, j = 0;
	while (i < line.length() && isspace(line[i]))
		i++;
	while (i < line.length() && !isspace(line[i]))
		command[j++] = line[i++];
	command[j] = '\0';
	return string(command);
}
	
void BVGReader::parse_internal(FILE* f){
	char line[MAX_LINE+1];
	line[MAX_LINE] = '\0';
	int line_number = 0;
	while(fgets(line,MAX_LINE,f)){
		line_number++;
		int len = strlen(line);
		while(len > 0  && isspace(line[len-1])){
			line[--len] = '\0';
		}
		if( len == 0 )
			continue;
		string L(line);
		string command = get_command_name(L);
		if (commands.find(command) == commands.end())
			throw ParsingException(L,line_number,"Invalid shape type \""+command+"\"",0);
		commands[command]->parse(renderer, L, line_number);
	}
}