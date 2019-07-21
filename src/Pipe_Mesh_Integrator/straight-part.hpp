#ifndef _STRAIGHT_PART_HPP_
#define _STRAIGHT_PART_HPP_

#include "tube-part.hpp"

template <class PointType, class NetType>
class StraightPart : public TubePart<PointType, NetType> {
private:

protected:
	
public:
	StraightPart(json input_configs) {
		StraightPart::section_count = input_configs["splits"];
		StraightPart::begin.setValue(input_configs["from"]["point"]["x"], input_configs["from"]["point"]["y"], input_configs["from"]["point"]["z"]);
		StraightPart::end.setValue(input_configs["to"]["point"]["x"], input_configs["to"]["point"]["y"], input_configs["to"]["point"]["z"]);

		StraightPart::cut = StraightPart::createCut(input_configs);
		StraightPart::cut->buildNet();
	
	};
	~StraightPart() {};
	void buildNet() {

		int iter = 0;

		bool Z, X, Y;
		if (StraightPart::end.x - StraightPart::begin.x >= 0) X = true; else X = false;
		if (StraightPart::end.y - StraightPart::begin.y > 0) Y = true; else Y = false;
		if (StraightPart::end.z - StraightPart::begin.z >= 0) Z = true; else Z = false;
		vect<PointType> temp(StraightPart::end.x - StraightPart::begin.x,
			StraightPart::end.y - StraightPart::begin.y,
			StraightPart::end.z - StraightPart::begin.z, Z, X, Y);
		StraightPart::normals.push_back(temp);

		//Вычисляем шаг на данном отрезке
		real dx = (StraightPart::end.x - StraightPart::begin.x) / StraightPart::section_count;
		real dy = (StraightPart::end.y - StraightPart::begin.y) / StraightPart::section_count;
		real dz = (StraightPart::end.z - StraightPart::begin.z) / StraightPart::section_count;

		for (int j = 0, i = 0; j < StraightPart::section_count; i++, j++) {

			// Вычисляется точка на прямой
			real nx = j * dx;
			real ny = j * dy;
			real nz = j * dz;
			vect< PointType> tmp(nx, ny, nz, StraightPart::normals[i].clockwiseZ, StraightPart::normals[i].clockwiseX, StraightPart::normals[i].clockwiseY);

			// находим нужную точку
			PointType Res( StraightPart::begin.x + nx ,  StraightPart::begin.y + ny,  StraightPart::begin.z + nz);

			// Если мы на пути следования трубы
			StraightPart::calculate2DLayer(Res, i);

			iter++;
		}


		//Отдельно обработаем последнюю точку
		//calculate2DLayer(path[n_path - 1], n_path - 2);
		//iter++;
		//считаем все остальные точки
		StraightPart::compyteTubeFE<NetType, NVTR_2D>(iter);



		//Сортируем точки
		std::sort(StraightPart::coord.begin(), StraightPart::coord.end(), StraightPart::sort_coord_vect);


		IMesh<PointType, NetType>::setNodesSize(StraightPart::coord.size());
		IMesh<PointType, NetType>::setElemsSize(StraightPart::nvtr.size());

	}
	
};

#endif
