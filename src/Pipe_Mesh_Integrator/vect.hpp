#ifndef _VECT_HPP_
#define _VECT_HPP_

#include "stdafx.h"

template <class PointType>
class vect {
private: 
	real x, y, z;
	bool directionX;
public:
	
	// ������� ����� �� ������
	void rotatePoint(PointType& res, PointType v) {

		real alfaZ, alfaX, alfaY;

		// �������� ������ ��� Z
		// ������� ����� � ��������� XY
		alfaZ = (x * v.x + y * v.y) / (sqrt(v.x * v.x + v.y * v.y) * sqrt(x * x + y * y));
		alfaX = (z * v.z + y * v.y) / (sqrt(v.z * v.z + v.y * v.y) * sqrt(y * y + z * z));

		if (alfaZ < 0 && alfaZ>-1) alfaZ += 1;
		if (!isnan(alfaZ) && alfaZ != 1 && alfaZ != -1)
			if (v.y > 0 && x >= 0 && y >= 0 ||
				v.y < 0 && x <= 0 && y <= 0)
			{
				rotateOzF(res, alfaZ);
				//rotateOzF(v, alfaZ);
			}
			else
			{
				rotateOzB(res, alfaZ);
				//rotateOzB(v, alfaZ);
			}

		// �������� ������ ��� �
		//������� ����� � ��������� YZ
		if (alfaX < 0 && alfaX > -1) alfaX += 1;
		if ( !isnan(alfaX) && alfaX != 1 && alfaZ != -1)
			if (v.y < 0 && z >= 0 && y <= 0 ||
				v.y > 0 && z <= 0 && y >= 0)
			{
				rotateOxF(res, alfaX);
				//rotateOxF(v, alfaX);
			}
			else
			{
				rotateOxB(res, alfaX);
				//rotateOxB(v, alfaX);
			}

		/*

		// �������� ������ ��� Y
		//������� ����� � ��������� XZ
		alfaY = (z * v.z + x * v.x) / (sqrt(v.z * v.z + v.x * v.x) * sqrt(x * x + z * z));

		if (alfaY < 0 && alfaY > -1)  alfaY = (directionX)? abs(alfaY): alfaY + 1 ;
		if (!isnan(alfaY) && alfaY != 1 && alfaY != -1)
			if (x <= 0 && z <= 0  ||
				x >= 0 && z >= 0 )
			{
				rotateOyF(res, alfaY);
				rotateOyF(v, alfaY);
			}
			else {
				rotateOyB(res, alfaY);
				rotateOyB(v, alfaY);
			}
*/
	}
	PointType getCoord(){ 
		PointType res(x, y,z);
		return res;
	};

	const real length() {
		return sqrt(x*x + y * y + z * z);
	};
void	setXDirection(bool value) {
	directionX = value;
	}
	vect() {
		x = 0;
		y = 0;
		z = 0;
		directionX = false;
	};
	// x,y,z, ������ Z,������ X 
	vect(real x1, real x2, real x3) {
		x = x1;
		y = x2;
		z = x3;
		directionX = false;
	}
private:
	// ������� ����� ������ ��� OX ������ ������� �������
	void rotateOxB(PointType& A,const real & cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldY = A.y;
		A.y = A.y* cosAlfa - A.z* sinAlfa;
		A.z = oldY * sinAlfa + A.z* cosAlfa;
	};

	// ������� ����� ������ ��� OY ������ ������� �������
	void rotateOyB(PointType& A,const  real& cosAlfa) 
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldX = A.x;
		A.x = A.x * cosAlfa + A.z * sinAlfa;
		A.z = -oldX * sinAlfa + A.z * cosAlfa;
		
	};
	// ������� ����� ������ ��� OZ ������ ������� �������
	void rotateOzB(PointType& A,const real& cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldX = A.x;
		
		A.x = A.x * cosAlfa - sinAlfa * A.y;
		A.y = oldX * sinAlfa + A.y* cosAlfa;
		
	};

	// ������� ����� ������ ��� OX �� ������� �������
	void rotateOxF(PointType & A, const real & cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldY = A.y;
		A.y = A.y* cosAlfa + A.z* sinAlfa;
		A.z = -oldY* sinAlfa + A.z* cosAlfa;
		
	};
	// ������� ����� ������ ��� OY �� ������� �������
	void rotateOyF(PointType & A, const real & cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldX = A.x;
		A.x = A.x * cosAlfa - A.z * sinAlfa;
		A.z = oldX * sinAlfa + A.z * cosAlfa;		
	};
	// ������� ����� ������ ��� OZ �� ������� �������
	void rotateOzF(PointType& A, const real& cosAlfa)
	{
		real sinAlfa = sqrt(1 - cosAlfa * cosAlfa);
		real oldX = A.x;
		A.x = A.x * cosAlfa + sinAlfa * A.y;
		A.y = -oldX* sinAlfa + A.y * cosAlfa;
	
	};
};

#endif //_VECT_HPP_

