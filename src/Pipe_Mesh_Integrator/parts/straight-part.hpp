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
		StraightPart::begin.setValue(input_configs["from"]["x"], input_configs["from"]["y"], input_configs["from"]["z"]);
		StraightPart::end.setValue(input_configs["to"]["x"], input_configs["to"]["y"], input_configs["to"]["z"]);

		StraightPart::cut = StraightPart::createCut(input_configs);
		StraightPart::cut->buildNet();
	
	};
	~StraightPart() {};
	void buildNet() {

		int iter = 0;

		bool  X, Y, Z;
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

		for (int j = 0; j <= StraightPart::section_count; j++) {

			// Вычисляется точка на прямой
			real nx = j * dx;
			real ny = j * dy;
			real nz = j * dz;
		
			// находим нужную точку
			PointType Res( StraightPart::begin.x + nx ,  StraightPart::begin.y + ny,  StraightPart::begin.z + nz);

			// Если мы на пути следования трубы
			StraightPart::calculate2DLayer(Res, StraightPart::normals[0]);

			iter++;
		}

		StraightPart::compyteTubeFE<NetType, NVTR_2D>(iter);

		//Сортируем точки
		std::sort(StraightPart::coord.begin(), StraightPart::coord.end(), StraightPart::sort_coord_vect);
		
		IMesh<PointType, NetType>::setNodesSize(StraightPart::coord.size());
		IMesh<PointType, NetType>::setElemsSize(StraightPart::nvtr.size());

	//	IMesh<PointType, NetType>::writeMeshInGlassFormatIntoFiles("../../Glass/Test/inftry.dat", "../../Glass/Test/nvkat.dat",
	//		"../../Glass/Test/xyz.dat", "../../Glass/Test/nver.dat");

	}
	
};

#endif
