#ifndef _VECT_HPP_
#define _VECT_HPP_

#include "stdafx.h"

template <class PointType>
class vect {
private: 
	real x, y, z;
	
public:
	bool forvardRoationX, forvardRoationY, forvardRoationZ;
	// поворот точки на вектор
	void rotatePoint(PointType& res, PointType v) {

		real alfaZ, alfaX, alfaY;
			
		// Вращение вокруг оси Z
		// косинус угола в плоскости XY
		alfaZ = (x * v.x + y * v.y) / (sqrt(v.x * v.x + v.y * v.y) * sqrt(x * x + y * y));
		
		if (alfaZ < 0 && alfaZ>-1) alfaZ += 1;
		if (!isnan(alfaZ) && alfaZ != 1 && alfaZ != -1)
			if ( v.y > 0 && forvardRoationX && forvardRoationY ||
				v.y < 0 && !forvardRoationX && !forvardRoationY )
			{
				rotateOzF(res, alfaZ);
				rotateOzF(v, alfaZ);
			}
			else
			{
				rotateOzB(res, alfaZ);
				rotateOzB(v, alfaZ);
			}

		// Вращение вокруг оси Х
		//косинус угола в плоскости YZ
		alfaX = (z * v.z + y * v.y) / (sqrt(v.z * v.z + v.y * v.y) * sqrt(y * y + z * z));
		if (alfaX < 0 && alfaX > -1) alfaX += 1;
		if (!isnan(alfaX) && alfaX != 1 && alfaX != -1)
			if (v.y < 0 && forvardRoationZ && !forvardRoationY ||
				v.y > 0 && !forvardRoationZ && forvardRoationY)
			{
				rotateOxF(res, alfaX);
				rotateOxF(v, alfaX);
			}
			else
			{
				rotateOxB(res, alfaX);
				rotateOxB(v, alfaX);
			}

		

		// Вращение вокруг оси Y
		//косинус угола в плоскости XZ
		alfaY = (z * v.z + x * v.x) / (sqrt(v.z * v.z + v.x * v.x) * sqrt(x * x + z * z));
		if (alfaY < 0 && alfaY > -1)
			alfaY += 1;
		if(!y)
		if (z && !isnan(alfaY) && alfaY == 1) {
			if (  forvardRoationX && forvardRoationY ||
				  !forvardRoationX && !forvardRoationY)
			{
				rotateOzF(res, 0);
				rotateOzF(v, 0);
			}
			else
			{
				rotateOzB(res, 0);
				rotateOzB(v, 0);
			}
		}
		else {

			if (!isnan(alfaY) && alfaY != 1 && alfaY != -1)
				if (!forvardRoationX && !forvardRoationZ ||
					forvardRoationX && forvardRoationZ)
				{
					rotateOyF(res, alfaY);
					rotateOyF(v, alfaY);
				}
				else {
					rotateOyB(res, alfaY);
					rotateOyB(v, alfaY);
				}
		}
	}
	PointType getCoord(){ 
		PointType res(x, y,z);
		return res;
	};

	const real length() {
		return sqrt(x*x + y * y + z * z);
	};

	void setOrtRotations(bool valueX, bool valueY, bool valueZ) {
		forvardRoationX = valueX;
		forvardRoationY = valueY;
		forvardRoationZ = valueZ;
	}
	vect() {
		x = 0;
		y = 0;
		z = 0;
		forvardRoationX = true;
		forvardRoationY = true;
		forvardRoationZ = true;
	};
	~vect() {};
	// x,y,z, вокруг Z,вокруг X 
	vect(real x1, real x2, real x3, bool valueX, bool valueY, bool valueZ) {
		x = x1;
		y = x2;
		z = x3;
		forvardRoationX = valueX;
		forvardRoationY = valueY;
		forvardRoationZ = valueZ;
	}
private:
	// поворот точки вокруг оси OX против часовой стрелке
	void rotateOxB(PointType& A,const real & cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldY = A.y;
		A.y = A.y* cosAlfa - A.z* sinAlfa;
		A.z = oldY * sinAlfa + A.z* cosAlfa;
	};

	// поворот точки вокруг оси OY против часовой стрелке
	void rotateOyB(PointType& A,const  real& cosAlfa) 
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldX = A.x;
		A.x = A.x * cosAlfa + A.z * sinAlfa;
		A.z = -oldX * sinAlfa + A.z * cosAlfa;
		
	};
	// поворот точки вокруг оси OZ против часовой стрелке
	void rotateOzB(PointType& A,const real& cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldX = A.x;
		
		A.x = A.x * cosAlfa - sinAlfa * A.y;
		A.y = oldX * sinAlfa + A.y* cosAlfa;
		
	};

	// поворот точки вокруг оси OX по часовой стрелке
	void rotateOxF(PointType & A, const real & cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldY = A.y;
		A.y = A.y* cosAlfa + A.z* sinAlfa;
		A.z = -oldY* sinAlfa + A.z* cosAlfa;
		
	};
	// поворот точки вокруг оси OY по часовой стрелке
	void rotateOyF(PointType & A, const real & cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldX = A.x;
		A.x = A.x * cosAlfa - A.z * sinAlfa;
		A.z = oldX * sinAlfa + A.z * cosAlfa;		
	};
	// поворот точки вокруг оси OZ по часовой стрелке
	void rotateOzF(PointType& A, const real& cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldX = A.x;
		A.x = A.x * cosAlfa + sinAlfa * A.y;
		A.y = -oldX* sinAlfa + A.y * cosAlfa;
	
	};
};

#endif //_VECT_HPP_

