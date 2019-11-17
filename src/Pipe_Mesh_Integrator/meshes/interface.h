#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "stdafx.h"

template <class PointType, class NetType> 
class IMesh {
protected:
	std::vector <PointType> coord;
	std::vector <NetType> nvtr;	

	//Кол-во узлов, и конечных элементов в сетке
	size_t nodes_size, el_size;

	bool readMeshInGlassFormatFromFiles(const char* inf, const char *nvkat, const char *xyz, const char *nver) {
		FILE* file = fopen(inf, "r");
		if (file == NULL ) return false;
		fscanf(file, " ISLAU=       0 INDKU1=       0 INDFPO=       0\nKUZLOV=%8d   KPAR=%8d    KT1=       0   KTR2=       0   KTR3=       0\nKISRS1=       0 KISRS2=       0 KISRS3=       0   KBRS=       0\n   KT7=       0   KT10=       0   KTR4=       0  KTSIM=       0\n   KT6=       0\n", &nodes_size, &el_size);
		fclose(file);

		int *tmp = new int[el_size];
		size_t t;
		FILE *fout;
		if (fopen_s(&fout, nvkat, "rb")) return false;
		t = fread(tmp, sizeof(int), el_size, fout);
		fclose(fout);

		real *tmp1 = new real[3 * nodes_size];
		if (fopen_s(&fout, xyz, "rb")) return false;
		t = fread(tmp1, sizeof(double), 3 * nodes_size, fout);
		int k = feof(fout);
		fclose(fout);

		if (fopen_s(&fout, nver, "rb")) return false;
		int  *tmp2 = new int[el_size * 14];
		t = fread(tmp2, sizeof(int), el_size * 14, fout);
		

		//Преобразуем считанные данные в массивы
		PointType a;
		for (int i = 0; i < 3*nodes_size; i+=3) {
			a.x = tmp1[i];
			a.y = tmp1[i + 1];
			a.z = tmp1[i + 2] ;
			a.id = i;
			coord.push_back(a);
		}

		NetType b;
		for (int i = 0, k = 0; k < el_size; k++, i = i + 14) {
			for (int j = 0; j < 8; j++) {
				b.n[j] = tmp2[i + j];
			}
			b.id = k+1;
			b.material = tmp[k];
			nvtr.push_back(b);
		}

		return true;
	};
	bool writeMeshInGlassFormatIntoFiles(const char* inf, const char* nvkat, const char* xyz, const char* nver) {

		// nvkat
		int* tmp = new int[el_size];
		size_t t;
		for (int i = 0; i < el_size; i++)	tmp[i] = nvtr[i].material;
		FILE* fout;
		if (fopen_s(&fout, nvkat, "wb")) return false;
		else {
			t = fwrite(tmp, sizeof(int), el_size, fout);
			if (t == el_size) fclose(fout); else return false;
		}
		delete tmp;

		//nver
		tmp = new int[el_size * 14];
		for (int i = 0, k = 0; k < el_size; k++, i = i + 14)
			for (int j = 0; j < 8; j++)
				tmp[i + j] = nvtr[k].n[j];

		//прежде следует вычленить информацию
		if (fopen_s(&fout, nver, "wb"))  return false;
		t = fwrite(tmp, sizeof(int), el_size * 14, fout);
		fclose(fout);
		delete tmp;

		//xyz
		double* tmp_xyz = new double[3 * nodes_size];
		for (int i = 0, k = 0; k < nodes_size; k++, i = i + 3) {
			tmp_xyz[i] = coord[k].x;
			tmp_xyz[i + 1] = coord[k].y;
			tmp_xyz[i + 2] = coord[k].z;
		}
		if (fopen_s(&fout, xyz, "wb")) return false;
		t = fwrite(tmp_xyz, sizeof(double), 3 * nodes_size, fout);
		fclose(fout);

		//inftry
		FILE* b = fopen(inf, "w");
		fprintf(b, " ISLAU=       0 INDKU1=       0 INDFPO=       0\nKUZLOV=%8d   KPAR=%8d    KT1=       0   KTR2=       0   KTR3=       0\nKISRS1=       0 KISRS2=       0 KISRS3=       0   KBRS=       0\n   KT7=       0   KT10=       0   KTR4=       0  KTSIM=       0\n   KT6=       0\n", nodes_size, el_size);
		fclose(b);

		return true;
	};
	bool setNodesSize(const size_t &n) {
		if (coord.size() == n)
		{
			nodes_size = n;
			return true;
		}
		else return false;
	};
	bool setElemsSize(const size_t &n) {
		if (nvtr.size() == n)
		{
			el_size = n;
			return true;
		}
		else return false; };
	
public:

	IMesh() {
		nodes_size = 0;
		el_size = 0;
	};
	virtual ~IMesh() { coord.clear(); nvtr.clear(); };
	size_t getNodesSize() { return nodes_size; };
	size_t getElemsSize() { return el_size; };

	PointType getNode(int i) { return coord[i]; };
	NetType getElem(int i) { return nvtr[i]; };

	void deleteElem(const int& id) {
		if (id < el_size) {
			nvtr.erase(nvtr.begin() + id);
			setElemsSize(nvtr.size());
		}
	else 
			throw runtime_error("Unable to delete elment with id");
	};

	std::vector<PointType> getNodes() { return coord; };
	std::vector<NetType> getElems() { return nvtr; }
	void moveMesh(PointType O) {
		for (int i = 0; i < nodes_size; i++) {
			coord[i].x += O.x;
			coord[i].y += O.y;
			coord[i].z += O.z;
			coord[i].id += O.id;
		}
		for (int i = 0; i < el_size; i++) {
			for (int j = 0; j < 8; j++)
				nvtr[i].n[j] += O.id;

			for (int j = 0; j < 6; j++)
				nvtr[i].planes[j].moveIds(O.id);
		}

	}
	
	
	virtual void buildNet() = 0;
};

#endif