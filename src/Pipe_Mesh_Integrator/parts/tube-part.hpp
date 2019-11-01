#ifndef _TUBE_PART_HPP_
#define _TUBE_PART_HPP_

#include "../meshes/interface.h"
#include "../sections/rectangle-section.hpp"
#include "../sections/round-section.hpp"
#include "vect.hpp"


template <class PointType> class sort_coord_vector
{
public:
	bool operator() (PointType i, PointType j)
	{
		return (i.id < j.id);
	}
};

template <class PointType, class NetType>
class TubePart : public TridimensionalMesh<PointType, NetType> {
private:

protected:

	sort_coord_vector <PointType> sort_coord_vect;	//Класс для сортировки
	PipeSection<PointType, NVTR_2D> *cut; // шаблонное сечение

	std::vector <vect<PointType>> normals; //Вектор нормалей
	
	int section_count{ 0 };
	PointType begin, end; //Точки начала и конца секции

	// Поворот заданных точек на ветор
	void rotateSection(vector<PointType>& tmp, vect<PointType>& norma)
	{
		real y = (norma.forvardRoationY) ? 1 : -1;
		Point normal{ 0, y, 0 };
		for (int i = 0; i < tmp.size(); i++)
			norma.rotatePoint(tmp[i], normal);
	}
	vect<PointType> getNorm(PointType begin, PointType end) {
		bool  X, Y, Z;
		if (end.x - begin.x >= 0) X = true; else X = false;
		if (end.y - begin.y >= 0) Y = true; else Y = false;
		if (end.z - begin.z >= 0) Z = true; else Z = false;
		vect<PointType> temp(end.x - begin.x, end.y - begin.y, end.z - begin.z,  X, Y,Z );
		return temp;
	}
	
	// Сдвиг вектора точек на вектор
	void moveSection(vector<PointType>& tmp, const PointType A) {

		for (int i = 0; i < tmp.size(); i++) {
			tmp[i].x += A.x;
			tmp[i].y += A.y;
			tmp[i].z += A.z;
			tmp[i].id += A.id;
		}
	}
	 // Построение сетки на двумрном слое
	void calculate2DLayer(PointType current_cut_center, vect<PointType>& norma) {

		vector<PointType> newCut = cut->getNodes();
		size_t node = TubePart::coord.size();
		current_cut_center.id = node;
		rotateSection(newCut, norma);
		moveSection(newCut, current_cut_center);
		TubePart::coord.insert(TubePart::coord.end(), newCut.begin(), newCut.end());
	}
	// построение КЭ, зная чило слоёв
	template <class NetType, class CutNetType> void compyteTubeFE(const int k) {
		size_t el_on_layer = cut->getElemsSize();
		size_t coor_on_layer = cut->getNodesSize();
		vector<CutNetType> nv = cut->getSectionNVTR();

		for (int q = 1; q < k; q++)
			for (int j = 0; j < el_on_layer; j++)
			{
				NetType tmp_FE(
					nv[j].n[0] + (q - 1) * coor_on_layer + 1,
					nv[j].n[1] + (q - 1) * coor_on_layer + 1,
					nv[j].n[2] + (q - 1) * coor_on_layer + 1,
					nv[j].n[3] + (q - 1) * coor_on_layer + 1,
					nv[j].n[0] + q * coor_on_layer + 1,
					nv[j].n[1] + q * coor_on_layer + 1,
					nv[j].n[2] + q * coor_on_layer + 1,
					nv[j].n[3] + q * coor_on_layer + 1,
					(q - 1) * coor_on_layer + j + 1,
					nv[j].material
				);
				TubePart::nvtr.push_back(tmp_FE);

				//Добавим плоскости
			}
		TubePart::nvtr.shrink_to_fit();
	}

	PipeSection<PointType, NVTR_2D>* createCut(json& cut_params) {


		if (cut_params["cut"]["path"] != nullptr)
		{
			TaskConfig currentConfigs;
			string cutPath = cut_params["cut"]["path"];
			cut_params["cut"] = currentConfigs.deserializeJsonFile(cutPath.c_str());
		}


		if (cut_params["cut"]["type"] == "circle")
		{
			return new RoundeSection<PointType, NVTR_2D>(cut_params["cut"]);
		}
		if (cut_params["cut"]["type"] == "rectangle")
		{
			return new RectangleSection<PointType, NVTR_2D>(cut_params["cut"]);
		}
		else
			return new RoundeSection<PointType, NVTR_2D>();
	}
public:
	TubePart(){
		cut = nullptr;
	};
	virtual ~TubePart() {
		delete cut;
		normals.clear();
	};
	virtual void buildNet() =0;
};

#endif
