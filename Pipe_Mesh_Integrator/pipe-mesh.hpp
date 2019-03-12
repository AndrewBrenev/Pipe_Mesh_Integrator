#ifndef _PIPE_MESH_HPP_
#define _PIPE_MESH_HPP_

#include "interface.h"
#include "pipe-section.hpp"
#include "vect.hpp"


template <class PointType, class NetType, class SectionType>
class PipeMesh : public TridimensionalMesh<PointType, NetType> {
private:
	PipeSection<PointType, NetType, SectionType> cut; // ��������� ������� 
	int  m, p;  // ���-�� ��������� ��� ��������, ���-�� ��������� � �������
	int n_path; //���-�� ��
	real stretch_coeff;  // ���������� ��� ���������

	SectionType findCircleOnLoyer(const SectionType &Begin, const  SectionType &End, const real &step) {}; // ���������� ���������� ����� ����� ��
//	PointType findPointOfRotation(PointType A, PointType B, Plane P, int inorm);  // ���������� ����� �������� ��� ��������
	real countMatrixDeterminant(const real a[][3]) {};   // ���������� ������������� �������
	
	bool buildPath(const char *dir) {

		//��������� ��������� 
		FILE* file = fopen(dir, "r");
		if (file == NULL) return false;
		fscanf(file, "%d\n", &n_path);
		path.resize(n_path);
		for (int i = 0; i < n_path; i++)
			fscanf(file, " ( %lf ; %lf ; %lf ) R = %lf r = %lf\n", &path[i].O.x, &path[i].O.y, &path[i].O.z, &path[i].R, &path[i].r);

		//�������� ������� ��������
		normals.resize(n_path - 1);
		for (int i = 1; i < n_path; i++) {
			bool Z, X, Y;
			if (path[i].O.x - path[i - 1].O.x >= 0) X = true; else X = false;
			if (path[i].O.z - path[i - 1].O.z >= 0) Z = true; else Z = false;
			if (path[i].O.y - path[i - 1].O.y > 0) Y = true; else Y = false;
			vect<PointType> temp(path[i].O.x - path[i - 1].O.x,
				path[i].O.y - path[i - 1].O.y,
				path[i].O.z - path[i - 1].O.z, Z, X, Y);
			normals[i - 1] = temp;
		}

		return true;

	};
	bool readFromFiles(const char *path) {

		if (!buildPath("./trajectory/Path.txt")) return false;

		FILE* file = fopen(path, "r");
		if (file == NULL) return false;
		fscanf(file, "p = %d m = %d k = %lf\n",&p, &m, &stretch_coeff);

		return true;
		

	};

protected:
	std::vector <SectionType> path;	//������ 3-������ �������� ���������
	std::vector <vect<PointType>> normals; //������ ��������

public:
	PipeMesh() {};
	~PipeMesh() {};
	void buildNet() {

		if(!IMesh<PointType,NetType>::readFromFiles("./incoming-pipe/inftry.dat", "./incoming-pipe/nvkat.dat", "./incoming-pipe/xyz.dat", "./incoming-pipe/nver.dat"))
			if(!readFromFiles("./input-info/input_pipe.txt")) 
				cout << "Error : Unable to read pipe mesh or its parameters" << endl;

		cut.buildNet();


	};
	
};

#endif
