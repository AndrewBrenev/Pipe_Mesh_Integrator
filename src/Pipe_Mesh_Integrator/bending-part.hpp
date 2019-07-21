#ifndef _BENDING_PART_HPP_
#define _BENDING_PART_HPP_

#include "tube-part.hpp" 


template <class PointType, class NetType>
class BendingPart : public TubePart<PointType, NetType> {
private:

protected:


public:
	BendingPart(json input_configs) {

	};
	~BendingPart() {};
	void buildNet() {

		/*
		// ����� � �������
		{

			PointType curNorm = normals[i].getCoord();
			PointType nextNorm = normals[i + 1].getCoord();

			// ���� ������ � �������
			real alfa = acos((curNorm.x*nextNorm.x + curNorm.y*nextNorm.y + curNorm.z*nextNorm.z) /
				(sqrt(curNorm.x*curNorm.x + curNorm.y*curNorm.y + curNorm.z*curNorm.z)*
					sqrt(nextNorm.x*nextNorm.x + nextNorm.y*nextNorm.y + nextNorm.z*nextNorm.z)));

			//������� ����� �� ������
			real Ax, Ay, Az;
			real Bx, By, Bz;

			Ax = path[i + 1].O.x - (curNorm.x * stretch_coeff * path[i + 1].R) / normals[i].length();
			Ay = path[i + 1].O.y - (curNorm.y * stretch_coeff * path[i + 1].R) / normals[i].length();
			Az = path[i + 1].O.z - (curNorm.z * stretch_coeff * path[i + 1].R) / normals[i].length();

			Bx = path[i + 1].O.x + (nextNorm.x * stretch_coeff * path[i + 1].R) / normals[i + 1].length();
			By = path[i + 1].O.y + (nextNorm.y * stretch_coeff * path[i + 1].R) / normals[i + 1].length();
			Bz = path[i + 1].O.z + (nextNorm.z * stretch_coeff * path[i + 1].R) / normals[i + 1].length();

			PointType A(Ax, Ay, Az), B(Bx, By, Bz), O;

			// �������� ���������, � ������� ��� �����
			real Nx, Ny, Nz, D;
			real a21, a22, a23;
			real a31, a32, a33;
			a21 = Ax - path[i + 1].O.x; a22 = Ay - path[i + 1].O.y; a23 = Az - path[i + 1].O.z;
			a31 = Bx - path[i + 1].O.x; a32 = By - path[i + 1].O.y; a33 = Bz - path[i + 1].O.z;

			// ������ ������� � ����������, � ������� ���������� �������
			Nx = a22 * a33 - a32 * a23;
			Ny = a23 * a31 - a21 * a33;
			Nz = a21 * a32 - a22 * a31;
			D = -path[i + 1].O.x*Nx - path[i + 1].O.y*Ny - path[i + 1].O.z*Nz;

			Plane rotatePlane(Nx, Ny, Nz, D);

			//������� ����� ��������
			O = findPointOfRotation(A, B, rotatePlane, i, alfa, curNorm, nextNorm);

			//��������� ����� ������� ��� �������� ����� ������ ������������� ���������� �������
			real N_length = sqrt(Nx*Nx + Ny * Ny + Nz * Nz);
			Nx = -Nx / N_length;
			Ny = -Ny / N_length;
			Nz = -Nz / N_length;

			//��������� ��� �� ����
			real alfa_step = alfa / m;

			SectionType Res_old, Res;
			Res_old.O.x = path[i].O.x; Res_old.O.y = path[i].O.y; Res_old.O.z = path[i].O.z;
			Res_old.R = path[i].R; Res_old.r = path[i].r;

			//����� ������� �����, ������ ��������� ��� �������
			for (int section = 0; section <= m; section++) {

				real aop = section * alfa_step*180.0 / M_PI;
				real sn = sin(section*alfa_step);
				real cs = cos(section*alfa_step);

				// ��������� ����� ����������
				Res.O.x = (Ax - O.x) *(cs + (1 - cs)*Nx*Nx) + (Ay - O.y)*((1 - cs)*Nx*Ny - sn * Nz) + (Az - O.z)*((1 - cs)*Nx*Nz + sn * Ny) + O.x;
				Res.O.y = (Ax - O.x) *((1 - cs)*Ny*Nx + sn * Nz) + (Ay - O.y)*(cs + (1 - cs)*Ny*Ny) + (Az - O.z)*((1 - cs)*Ny*Nz - sn * Nx) + O.y;
				Res.O.z = (Ax - O.x) *((1 - cs)*Nz*Nx - sn * Ny) + (Ay - O.y)*((1 - cs)*Nz*Ny + sn * Nx) + (Az - O.z)*(cs + (1 - cs)*Nz*Nz) + O.z;
				Res.R = path[i + 1].R; Res.r = path[i + 1].r;

				// ���������� ����������� ��������
				bool Z, X, Y(true);
				if (Res.O.x - Res_old.O.x >= 0) X = true; else X = false;
				if (Res.O.z - Res_old.O.z >= 0) Z = true; else Z = false;
				//��������� ������, � ����������� �������� ��������
				vect<PointType> norml(Res.O.x - Res_old.O.x, Res.O.y - Res_old.O.y, Res.O.z - Res_old.O.z, Z, X, Y);
				normals.push_back(norml);
			
				//������ ����
				calculate2DLayer(Res, normals.size() - 1);

				Res_old = Res;
				iter++;
			}
			// ���������� � ������ � ���������

			dx = (path[i + 2].O.x - path[i + 1].O.x) / n;
			dy = (path[i + 2].O.y - path[i + 1].O.y) / n;
			dz = (path[i + 2].O.z - path[i + 1].O.z) / n;

			real n_ny = abs(path[i + 2].O.y - path[i + 1].O.y) / n;
			bool j_f = true;

			for (int w = 0; w < n && j_f; w++) {
				// ����������� ����� �� ������
				nx = w * dx;
				ny = w * dy;
				nz = w * dz;
				vect<PointType> tmp1(nx, ny, nz, normals[i].clockwiseZ, normals[i].clockwiseX, normals[i].clockwiseY);
				if (tmp1.length() >= stretch_coeff * path[i + 1].R) {
					j = w;
					j_f = false;
				}
			}
			end = true;

		}

		*/
		std::sort(BendingPart::coord.begin(), BendingPart::coord.end(), BendingPart::sort_coord_vect);


		IMesh<PointType, NetType>::setNodesSize(BendingPart::coord.size());
		IMesh<PointType, NetType>::setElemsSize(BendingPart::nvtr.size());
	};
	
};

#endif
