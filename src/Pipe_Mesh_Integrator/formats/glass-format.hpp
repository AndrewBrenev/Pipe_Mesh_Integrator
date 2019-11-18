#ifndef _GLASS_FORMAT_HPP_
#define _GLASS_FORMAT_HPP_

#include "flie-format.hpp"

template <class PointType, class NetType>
class GlassFormat : public FileFormater<PointType, NetType> {

private:
	 
	  string inf, xyz, nvkat, nver;

public:
	GlassFormat() {};
	GlassFormat(IMesh<PointType, NetType>* mesh) {
		FileFormater<PointType, NetType>::meshToOperate = mesh; 
	};

	~GlassFormat() {};

	// 0 - inftry , 1 - nvkat, 2 - xyz , 3 - nver
	void setFilePathes(const vector<string>& inputFiles) override
	{
		inf = inputFiles[0];
		nvkat = inputFiles[1];
		xyz = inputFiles[2];
		nver = inputFiles[3];

	};
	bool saveMeshToFiles() override
	{
		size_t nodes_size = FileFormater<PointType, NetType>::meshToOperate->getNodesSize(),
			el_size = FileFormater<PointType, NetType>::meshToOperate->getElemsSize();

		// nvkat
		int* tmp = new int[el_size];
		size_t t;
		for (int i = 0; i < el_size; i++)	tmp[i] = FileFormater<PointType, NetType>::getElement(i).material;
		FILE* fout;
		if (fopen_s(&fout, nvkat.c_str(), "wb")) return false;
		else {
			t = fwrite(tmp, sizeof(int), el_size, fout);
			if (t == el_size) fclose(fout); else return false;
		}
		delete tmp;

		//nver
		tmp = new int[el_size * 14];
		for (int i = 0, k = 0; k < el_size; k++, i = i + 14) {
			auto el = FileFormater<PointType, NetType>::getElement(k);
			for (int j = 0; j < 8; j++)
				tmp[i + j] = el.n[j];
		}
		//прежде следует вычленить информацию
		if (fopen_s(&fout, nver.c_str(), "wb"))  return false;
		t = fwrite(tmp, sizeof(int), el_size * 14, fout);
		fclose(fout);
		delete tmp;

		//xyz
		double* tmp_xyz = new double[3 * nodes_size];
		for (int i = 0, k = 0; k < nodes_size; k++, i = i + 3) {
			auto node = FileFormater<PointType, NetType>::getPoint(k);
			tmp_xyz[i] = node.x;
			tmp_xyz[i + 1] = node.y;
			tmp_xyz[i + 2] = node.z;
		}
		if (fopen_s(&fout, xyz.c_str(), "wb")) return false;
		t = fwrite(tmp_xyz, sizeof(double), 3 * nodes_size, fout);
		fclose(fout);

		//inftry
		FILE* b = fopen(inf.c_str(), "w");
		fprintf(b, " ISLAU=       0 INDKU1=       0 INDFPO=       0\nKUZLOV=%8d   KPAR=%8d    KT1=       0   KTR2=       0   KTR3=       0\nKISRS1=       0 KISRS2=       0 KISRS3=       0   KBRS=       0\n   KT7=       0   KT10=       0   KTR4=       0  KTSIM=       0\n   KT6=       0\n", nodes_size, el_size);
		fclose(b);

		return true;
	};

	bool readMeshFromFiles() override
	{

		FILE* file = fopen(inf.c_str(), "r");

		uint32_t nodes_size, el_size;
		fscanf(file, " ISLAU=       0 INDKU1=       0 INDFPO=       0\nKUZLOV=%8d   KPAR=%8d    KT1=       0   KTR2=       0   KTR3=       0\nKISRS1=       0 KISRS2=       0 KISRS3=       0   KBRS=       0\n   KT7=       0   KT10=       0   KTR4=       0  KTSIM=       0\n   KT6=       0\n", &nodes_size, &el_size);
		fclose(file);
		
		int* tmp = new int[el_size];
		size_t t;
		FILE* fout;
		fopen_s(&fout, nvkat.c_str(), "rb");
		t = fread(tmp, sizeof(int), el_size, fout);
		fclose(fout);

		real* tmp1 = new real[3 * nodes_size];
		fopen_s(&fout, xyz.c_str(), "rb");
		t = fread(tmp1, sizeof(double), 3 * nodes_size, fout);
		int k = feof(fout);
		fclose(fout);

		fopen_s(&fout, nver.c_str(), "rb");
		int* tmp2 = new int[el_size * 14];
		t = fread(tmp2, sizeof(int), el_size * 14, fout);


		//Преобразуем считанные данные в массивы
		PointType a;
		for (int i = 0; i < 3 * nodes_size; i += 3) {
			a.x = tmp1[i];
			a.y = tmp1[i + 1];
			a.z = tmp1[i + 2];
			a.id = i;
			FileFormater<PointType, NetType>::pushPoint(a);
		}

		NetType b;
		for (int i = 0, k = 0; k < el_size; k++, i = i + 14) {
			for (int j = 0; j < 8; j++) {
				b.n[j] = tmp2[i + j];
			}
			b.id = k + 1;
			b.material = tmp[k];
			FileFormater<PointType, NetType>::pushElement(b);
		}

		FileFormater<PointType, NetType>::setPointsSize(nodes_size);
		FileFormater<PointType, NetType>::setElementsSize(el_size);

		return true;
	};

};

#endif //_GLASS_FORMAT_HPP_