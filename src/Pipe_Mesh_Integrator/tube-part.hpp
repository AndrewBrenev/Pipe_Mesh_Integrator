#ifndef _TUBE_PART_HPP_
#define _TUBE_PART_HPP_



#include "interface.h"
#include "square-section.hpp"
#include "round-section.hpp"
#include "vect.hpp"


template <class PointType> class sort_coord_vector
{
public:
	bool operator() (PointType i, PointType j)
	{
		return (i.id < j.id);
	}
};
typedef Circle SectionType;
template <class PointType, class NetType>
class TubePart : public TridimensionalMesh<PointType, NetType> {
private:
	

	/*
	int  m, p;  // ���-�� ��������� ��� ��������, ���-�� ��������� � �������
	int n_path; //���-�� ��
	real stretch_coeff;  // ���������� ��� ���������

	std::vector <SectionType> circle_centers;
	int iter;

	// ������� �������� ����� �� �����
	void rotateSection(vector<PointType>&tmp, const int j) {
		Point normal(0, 1, 0, 0);
		for (int i = 0; i < tmp.size(); i++)
			tmp[i] = normals[j].rotatePoint(tmp[i], normal);
	}
	// ����� ������� ����� �� ������
	void moveSection(vector<PointType> &tmp, const PointType A) {
		for (int i = 0; i < tmp.size(); i++) {
			tmp[i].x += A.x;
			tmp[i].y += A.y;
			tmp[i].z += A.z;
		}
	}

	// ���������� ����� �� ����
	void calculate2DLayer(const SectionType c, int norma) {
		//���������
		size_t node = PipeMesh::coord.size();
		vector<PointType> newCut = cut->coordTubeOnly(c, node);
		PointType A(c.O.x, c.O.y, c.O.z, c.R);
		circle_centers.push_back(c);
		rotateSection(newCut, norma);
		moveSection(newCut, A);
		PipeMesh::coord.insert(PipeMesh::coord.end(), newCut.begin(), newCut.end());

	};
	*/

	/*
	//���������� 3-� ������ �����
	void combine3DNet() {

		// ����� ����� � ������� ��������
		int i_n;
		// ��� �� ������
		int j = 0;

		// 
		int n, l;
		cut->getParam(n, l);
		// ���� ��������� �������� �� ������ ������
		bool end = false;
		for (int i = 0; i < n_path - 1; i++) {

			end = false;
			//��������� ��� �� ������ �������
			real dx = (path[i + 1].O.x - path[i].O.x) / n;
			real dy = (path[i + 1].O.y - path[i].O.y) / n;
			real dz = (path[i + 1].O.z - path[i].O.z) / n;

			while (!end) {

				// ����������� ����� �� ������
				real nx = j * dx;
				real ny = j * dy;
				real nz = j * dz;
				vect< PointType> tmp(nx, ny, nz, normals[i].clockwiseZ, normals[i].clockwiseX, normals[i].clockwiseY);

				if (normals[i].length() - tmp.length() <= stretch_coeff * path[i + 1].R && i != n_path - 2)
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
						/*
						{
							// ���� ������ � �������� � ������� ������
							int n_centers = circle_centers.size() - 1;
							int extra_loyer = testExternalGrid(circle_centers[n_centers].O.y, Res.O.y);
							if (extra_loyer != -1)
							{
								SectionType prev_circle = circle_centers[n_centers];
								SectionType oneMore = findCircleOnLoyer(prev_circle, Res, Y0 + extra_loyer * Yh);
								// ���� �� �� ���� ���������� �����
								calculate2DLayer(oneMore, normals.size() - 1);
								iter++;
							}
						}*//*
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
				else
					// ���� �������� �� ������
				{
					//���� �� ����� � ������ ��������
					if (i == n_path - 2 && normals[i].length() - tmp.length() < stretch_coeff * path[i + 1].R)
						end = true;
					else {

						// ������� ������ �����
						SectionType Res(nx + path[i].O.x, ny + path[i].O.y, nz + path[i].O.z, path[i].R, path[i].r);
						/*if (iter) {
							// ���� ������ � �������� � ������� ������
							int n_centers = circle_centers.size() - 1;

							int extra_loyer = testExternalGrid(circle_centers[n_centers].O.y, Res.O.y);
							if (extra_loyer != -1)
							{
								SectionType prev_circle = circle_centers[n_centers];
								SectionType oneMore = findCircleOnLoyer(prev_circle, Res, Y0 + extra_loyer * Yh);
								// ���� �� �� ���� ���������� �����
								calculate2DLayer(oneMore, i);
								iter++;
							}

						}*/
/*
						// ���� �� �� ���� ���������� �����
						calculate2DLayer(Res, i);

						iter++;
					}
					j++;
				}
			}
		}
		/*
		{
			// ���� ������ � �������� � ������� ������
			int n_centers = circle_centers.size() - 1;
			int extra_loyer = testExternalGrid(circle_centers[n_centers].O.y, path[n_path - 1].O.y);
			if (extra_loyer != -1)
			{
				SectionType prev_circle = circle_centers[n_centers];
				SectionType oneMore = findCircleOnLoyer(prev_circle, path[n_path - 1], Y0 + extra_loyer * Yh);
				// ���� �� �� ���� ���������� �����
				calculate2DLayer(oneMore, normals.size() - 1);
				iter++;
			}
		}
		//�������� ���������� ��������� �����
		calculate2DLayer(path[n_path - 1], n_path - 2);
		iter++;
		//������� ��� ��������� �����
		compyteTubeFE<NetType, NVTR_2D>(iter);

		//start_ind = coord.size();
		//start_elem = nvtr.size();

		//��������� �����
		std::sort(PipeMesh::coord.begin(), PipeMesh::coord.end(), sort_coord_vect);


		IMesh<PointType, NetType>::setNodesSize(PipeMesh::coord.size());
		IMesh<PointType, NetType>::setElemsSize(PipeMesh::nvtr.size());
		//IMesh<PointType,NetType>::writeToFile("./Glass/Test/inftry.dat","./Glass/Test/nvkat.dat",
		//	"./Glass/Test/xyz.dat", "./Glass/Test//nver.dat");

	}

	*/
protected:

	sort_coord_vector <PointType> sort_coord_vect;	//����� ��� ����������
	PipeSection<PointType, NVTR_2D, SectionType> *cut; // ��������� �������

	std::vector <SectionType> path;	//������ 3-������ �������� ���������
	std::vector <vect<PointType>> normals; //������ ��������

	int section_count;
	PointType begin, end; //����� ������ � ����� ������

	// ������� �������� ����� �� �����
	void rotateSection(vector<PointType>&tmp, const int j) {
		Point normal(0, 1, 0, 0);
		for (int i = 0; i < tmp.size(); i++)
			tmp[i] = normals[j].rotatePoint(tmp[i], normal);
	}
	// ����� ������� ����� �� ������
	void moveSection(vector<PointType> &tmp, const PointType A) {
		for (int i = 0; i < tmp.size(); i++) {
			tmp[i].x += A.x;
			tmp[i].y += A.y;
			tmp[i].z += A.z;
		}
	}
	 // ���������� ����� �� ����
	void calculate2DLayer(const PointType current_cut_center, int norma) {
		//���������
		size_t node = TubePart::coord.size();
		vector<PointType> newCut = cut->coordTubeOnly(node);
		//circle_centers.push_back(c);
		rotateSection(newCut, norma);
		moveSection(newCut, current_cut_center);
		TubePart::coord.insert(TubePart::coord.end(), newCut.begin(), newCut.end());

	};
	// ���������� ��, ���� ���� ����
	template <class NetType, class CutNetType> void compyteTubeFE(const int k) {
		size_t el_on_layer = cut->getElemsSize();
		size_t coor_on_layer = cut->getNodesSize();
		vector<CutNetType> nv = cut->getSectionNVTR();
		for (int q = 1; q < k; q++)
			for (int j = 0; j < el_on_layer; j++)
			{
				NetType tmp_FE(
					nv[j].n[0] + (q - 1)*coor_on_layer + 1,
					nv[j].n[1] + (q - 1)*coor_on_layer + 1,
					nv[j].n[2] + (q - 1)*coor_on_layer + 1,
					nv[j].n[3] + (q - 1)*coor_on_layer + 1,
					nv[j].n[0] + q * coor_on_layer + 1,
					nv[j].n[1] + q * coor_on_layer + 1,
					nv[j].n[2] + q * coor_on_layer + 1,
					nv[j].n[3] + q * coor_on_layer + 1,
					(q - 1)*coor_on_layer + j + 1,
					nv[j].material
				);
				TubePart::nvtr.push_back(tmp_FE);
			}
	}
	PipeSection<PointType, NVTR_2D, SectionType> * createCut(json cut_params) {
		if (cut_params["cut"]["type"] == "circle") {
			Circle A(0, 0, 0, cut_params["sections"][0]["R"], cut_params["sections"][0]["d"]);
			return new RoundeSection<PointType, NVTR_2D, Circle>(A, cut_params["cut"]["splits"], cut_params["cut"]["layer_splits"]);
		}
		if (cut_params["cut"]["type"] == "rectangle") {
			Circle A(0, 0, 0, cut_params["sections"][0]["R"], cut_params["sections"][0]["d"]);
			return new RoundeSection<PointType, NVTR_2D, Circle>(A, cut_params["cut"]["splits"], cut_params["cut"]["layer_splits"]);
		}
		else 
		return NULL;
	}
public:
	TubePart(){};
	~TubePart() {};
	virtual void buildNet() =0;

};

#endif
