#ifndef _VECT_HPP_
#define _VECT_HPP_

#include "stdafx.h"


template <class PointType>
class vect {
private: 
	real x, y, z;
	//���������� ������� ��, ���� ������ ������� �������
	bool clockwiseZ, clockwiseX, clockwiseY;
public:
	
	// ������� ����� �� ������
	PointType rotatePoint(PointType A, PointType v) {

		PointType res;
		real alfaZ, alfaX, alfaY(1);
		// ������� ����� � ��������� XY
		alfaZ = (x*v.x + y * v.y) / (sqrt(v.x*v.x + v.y * v.y) * sqrt(x*x + y * y));
		//������� ����� � ��������� YZ
		alfaX = (z*v.z + y * v.y) / (sqrt(v.z*v.z + v.y * v.y) * sqrt(y*y + z * z));
		res = A;
		/*


		if (x!= 0 && y == 0 && z!= 0){
			//������� ����� � ��������� XZ

			// ������� �������� ����� ������ ��� X
			if (clockwiseZ)
				res = rotateOXB(res, z);
			else
				res = rotateOXF(res, z);

			// ������� �������� ����� ������ ��� Z
			if (clockwiseX)
				res = rotateOyF(res, 0.0);
			else
				res = rotateOyB(res, 0.0);


			res.id = A.id;
			return res;
		}
		*/

		// ������� �������� ����� ������ ��� �
		if (!isnan(alfaX) && alfaX != -1)
			if (clockwiseZ)
				res = rotateOXB(A, alfaX);
			else
				res = rotateOXF(A, alfaX);

		// ������� ���������� ����� ������ ��� Z
		if (!isnan(alfaZ) && alfaZ != -1)
			if (clockwiseX)
				res = rotateOzF(res, alfaZ);
			else
				res = rotateOzB(res, alfaZ);

		res.id = A.id;
		return res;

	};

	const real length() {
		return sqrt(x*x + y * y + z * z);
	};
	vect() {};
	// x,y,z, ������ Z,������ X 
	vect(real x1, real x2, real x3, bool A, bool B, bool C) {
		x = x1;
		y = x2;
		z = x3;
		clockwiseZ = A;
		clockwiseX = B;
		clockwiseY = C;
	}
private:
	// �������� ������ ��� OX ������ ������� �������
	PointType rotateOXB(PointType A, real cos) {
		real s = sqrt(1 - cos * cos);
		real c = cos;
		PointType R;
		R.x = A.x;
		R.y = A.y*c - A.z*s;
		R.z = A.y*s + A.z*c;
		return R;
	};
	// �������� ������ ��� OY ������ ������� �������
	PointType rotateOyB(PointType A, real cos) {
		real s = sqrt(1 - cos * cos);
		real c = cos;
		PointType R;
		R.y = A.y;
		R.x = A.x*c + A.z*s;
		R.z = -A.x*s + A.z*c;
		return R;
	};
	// ������� ������ ��� OZ ������ ������� �������
	PointType rotateOzB(PointType A, real cos) {
		real s = sqrt(1 - cos * cos);
		real c = cos;
		PointType R;
		R.x = A.x * c - s * A.y;
		R.y = A.x * s + A.y*c;
		R.z = A.z;
		return R;
	};

	// �������� ������ ��� OX �� ������� �������
	PointType rotateOXF(PointType A, real cos) {
		real s = sqrt(1 - cos * cos);
		real c = cos;
		PointType R;
		R.x = A.x;
		R.y = A.y*c + A.z*s;
		R.z = -A.y*s + A.z*c;
		return R;
	};
	// �������� ������ ��� OY �� ������� �������
	PointType rotateOyF(PointType A, real cos)
	{
		real s = sqrt(1 - cos * cos);
		real c = cos;
		PointType R;
		R.y = A.y;
		R.x = A.x*c - A.z*s;
		R.z = A.x*s + A.z*c;
		return R;
	};
	// ������� ������ ��� OZ �� ������� �������
	PointType rotateOzF(PointType A, real cos) {
		{
			real s = sqrt(1 - cos * cos);
			real c = cos;
			PointType R;
			R.x = A.x * c + s * A.y;
			R.y = -A.x * s + A.y*c;
			R.z = A.z;
			return R;
		}
	};
};

#endif //_VECT_HPP_

