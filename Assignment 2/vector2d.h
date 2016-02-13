/* vector2d.h

  B. Bird and Michael Reiter - 01/02/2016

*/

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <iostream>
#include <cmath>

class Vector2d{
public:
	Vector2d(){
		this->x = this->y = 0;
	}
	Vector2d(float x, float y){
			this->x = x;
			this->y = y;
	}
	//No need to define copy constructors, since the default ones will suffice.
	
	Vector2d operator + (const Vector2d& other){
		return Vector2d(x+other.x, y+other.y);
	}
	//Unary negation
	Vector2d operator-(){
		return Vector2d(-x,-y);
	}
	
	Vector2d operator - (const Vector2d& other){
		return Vector2d(x-other.x, y-other.y);
	}
	
	Vector2d& operator += (const Vector2d& other){
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
	Vector2d& operator -= (const Vector2d& other){
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}
	
	//Multiplication by scalar
	Vector2d& operator *= (float f){
		this->x *= f;
		this->y *= f;
		return *this;
	}
	//Division by scalar
	Vector2d& operator /= (float f){
		this->x /= f;
		this->y /= f;
		return *this;
	}
	
	friend Vector2d operator * (float f, const Vector2d& v){
		return Vector2d(v.x*f, v.y*f);
	}
	friend Vector2d operator * (const Vector2d& v, float f){
		return Vector2d(v.x*f, v.y*f);
	}
	friend Vector2d operator / (const Vector2d& v, float f){
		return Vector2d(v.x/f, v.y/f);
	}
	
	class InvalidVectorIndex{};
	
	float& operator [] (int i){
		if (i == 0)
			return x;
		if (i == 1)
			return y;
		throw InvalidVectorIndex();
	}
	
	float dot(const Vector2d& other){
		return x*other.x + y*other.y;
	}
	
	double length(){
		return sqrt(dot(*this));
	}

	Vector2d normalize() {
		double len = length();
		if (len == 0) {
			// can't normalize a 0 vector so just return itself
			return Vector2d(x, y);
		}

		double new_x = x / len;
		double new_y = y / len;

		// std::cout << len << x << y << std::endl;
		return Vector2d(new_x, new_y);
	}
	
	void print(){
		std::cout << "(" << x << "," << y << ")" << std::endl;
	}
	
	friend std::ostream & operator<< ( std::ostream& s, const Vector2d& v){
		 s << "(" << v.x << "," << v.y << ")";
		 return s;
	}
	
	float x,y;
};

#endif