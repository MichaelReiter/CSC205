/* matrix_test.cpp

   B. Bird - 02/08/2016
*/
#include <iostream>
#include <vector>
#include <cmath>
#include "matrix.h"
#include "colourRGB.h"
#include "transformed_renderer.h"

using namespace std;

int main(){

	cout << "Vector3 V(1,2,3):" << endl;
	Vector3 V(1,2,3);
	V.print();
	cout << "Changing v.x to 100" << endl;
	V.x = 100;
	V.print();
	
	cout << endl;
	
	
	cout << "Matrix3 M" << endl;
	Matrix3 M;
	M.print();
	cout << "Setting M to the identity" << endl;
	M.identity();
	M.print();
	cout << "Setting M(1,0) to 6" << endl;
	M(1,0) = 6;
	M.print();
	cout << "Setting M(1,2) to 10" << endl;
	M.Entry(1,2) = 10;
	M.print();
	
	cout << "Matrix3 M2(0,0,1,0,0,1,1,0,0)" << endl;
	Matrix3 M2(0,0,1,0,0,1,1,0,0);
	M2.print();
	cout << "Matrix3 M3 = M*M2" << endl;
	Matrix3 M3 = M*M2;
	M3.print();
	cout << "Vector3 V2 = M*M2*V" << endl;
	Vector3 V2 = M*M2*V;
	V2.print();
	
	
	
	
	cout << endl;
	
	return 0;
}
