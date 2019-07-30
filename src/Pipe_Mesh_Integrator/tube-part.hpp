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

protected:

	sort_coord_vector <PointType> sort_coord_vect;	//Класс для сортировки
	PipeSection<PointType, NVTR_2D, SectionType> *cut; // шаблонное сечение

	std::vector <vect<PointType>> normals; //Вектор нормалей

	int section_count;
	PointType begin, end; //Точки начала и конца секции

	// Поворот заданных точек на ветор
	void rotateSection(vector<PointType>&tmp, vect<PointType> &norma) {
		Point normal(0, 1, 0, 0);
		for (int i = 0; i < tmp.size(); i++)
			tmp[i] = norma.rotatePoint(tmp[i], normal);
	}

	vect<PointType> getNorm(PointType begin, PointType end) {
		bool  X, Y, Z;
		if (end.x - begin.x >= 0) X = true; else X = false;
		if (end.y - begin.y > 0) Y = true; else Y = false;
		if (end.z - begin.z >= 0) Z = true; else Z = false;
		vect<PointType> temp(end.x - begin.x,end.y - begin.y,end.z - begin.z, Z, X, Y);
		return temp;
	}

	// Сдвиг вектора точек на вектор
	void moveSection(vector<PointType> &tmp, const PointType A) {
		for (int i = 0; i < tmp.size(); i++) {
			tmp[i].x += A.x;
			tmp[i].y += A.y;
			tmp[i].z += A.z;
			tmp[i].id += A.id;
		}
	}
	 // Построение сетки на слое
	void calculate2DLayer(PointType current_cut_center, vect<PointType> &norma) {
		//Вычисляем
		size_t node = TubePart::coord.size();
		vector<PointType> newCut = cut->getNodes();
		current_cut_center.id = node;
		//vector<PointType> newCut =cut->coordTubeOnly(node);
		//circle_centers.push_back(c);
		rotateSection(newCut, norma);
		moveSection(newCut, current_cut_center);
		TubePart::coord.insert(TubePart::coord.end(), newCut.begin(), newCut.end());

	};
	// построение КЭ, зная чило слоёв
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
