#ifndef _BENDING_PART_HPP_
#define _BENDING_PART_HPP_

#include "tube-part.hpp" 


template <class PointType, class NetType>
class BendingPart : public TubePart<PointType, NetType> {
private:
	PointType rotationPoint;
	vect<PointType> prev_straight_norm, next_straight_norm;
	
protected:

public:
	BendingPart(json input_configs) {
		
		BendingPart::begin.setValue(input_configs["begin"]["to"]["point"]["x"], input_configs["begin"]["to"]["point"]["y"], input_configs["begin"]["to"]["point"]["z"]);
		BendingPart::end.setValue(input_configs["end"]["from"]["point"]["x"], input_configs["end"]["from"]["point"]["y"], input_configs["end"]["from"]["point"]["z"]);
		rotationPoint.setValue(input_configs["rotationPoint"]["point"]["x"], input_configs["rotationPoint"]["point"]["y"], input_configs["rotationPoint"]["point"]["z"]);
		BendingPart::section_count = input_configs["splits"];

		PointType A(input_configs["begin"]["from"]["point"]["x"], input_configs["begin"]["from"]["point"]["y"], input_configs["begin"]["from"]["point"]["z"]);
		prev_straight_norm = BendingPart::getNorm(A,	BendingPart::begin);

		PointType B(input_configs["end"]["to"]["point"]["x"], input_configs["end"]["to"]["point"]["y"], input_configs["end"]["to"]["point"]["z"]);
		next_straight_norm = BendingPart::getNorm(BendingPart::end, B);

		BendingPart::cut = BendingPart::createCut(input_configs);
		BendingPart::cut->buildNet();
	};
	~BendingPart() {};
	void buildNet() {

		int iter = 0;

		PointType curNorm(BendingPart::begin.x - rotationPoint.x, BendingPart::begin.y - rotationPoint.y, BendingPart::begin.z - rotationPoint.z);
		PointType nextNorm(BendingPart::end.x - rotationPoint.x, BendingPart::end.y - rotationPoint.y, BendingPart::end.z - rotationPoint.z);

		//Ќайдем угол поворота
		real alfa = acos((curNorm.x*nextNorm.x + curNorm.y*nextNorm.y + curNorm.z*nextNorm.z) /
			(sqrt(curNorm.x*curNorm.x + curNorm.y*curNorm.y + curNorm.z*curNorm.z)*
				sqrt(nextNorm.x*nextNorm.x + nextNorm.y*nextNorm.y + nextNorm.z*nextNorm.z)));

		// ¬ыисл€ем плоскость, в которой они лежат
		real Nx, Ny, Nz, D;
		real a21, a22, a23;
		real a31, a32, a33;
		a21 = BendingPart::begin.x - rotationPoint.x; a22 = BendingPart::begin.y - rotationPoint.y; a23 = BendingPart::begin.z - rotationPoint.z;
		a31 = BendingPart::end.x - rotationPoint.x; a32 = BendingPart::end.y - rotationPoint.y; a33 = BendingPart::end.z - rotationPoint.z;

		// ¬ектор нормали к плосткости, в которой происходит поворот
		Nx = a22 * a33 - a32 * a23;
		Ny = a23 * a31 - a21 * a33;
		Nz = a21 * a32 - a22 * a31;
		D = -rotationPoint.x*Nx - rotationPoint.y*Ny - rotationPoint.z*Nz;

		Plane rotatePlane(Nx, Ny, Nz, D);

		//Ќормируем ветор нормали дл€ поворота точек вокруг произвольного единичного вектора
		real N_length = sqrt(Nx*Nx + Ny * Ny + Nz * Nz);
		Nx = -Nx / N_length;
		Ny = -Ny / N_length;
		Nz = -Nz / N_length;

		//¬ычисл€ем шаг по углу
		real alfa_step = alfa / BendingPart::section_count;

		PointType Res;
	
		bool  X, Y(true), Z;

		PointType Res_old = BendingPart::begin;
		
		//Ќашли крайние точки, теперь реализуем сам поворот
		for (int section = 0; section <= BendingPart::section_count; section++) {

			real aop = section * alfa_step*180.0 / M_PI;
			real sn = sin(section*alfa_step);
			real cs = cos(section*alfa_step);

			// ѕовернули центр окружности
			Res.x = (BendingPart::begin.x - rotationPoint.x) *(cs + (1 - cs)*Nx*Nx) + (BendingPart::begin.y - rotationPoint.y)*((1 - cs)*Nx*Ny - sn * Nz) + (BendingPart::begin.z - rotationPoint.z)*((1 - cs)*Nx*Nz + sn * Ny) + rotationPoint.x;
			Res.y = (BendingPart::begin.x - rotationPoint.x) *((1 - cs)*Ny*Nx + sn * Nz) + (BendingPart::begin.y - rotationPoint.y)*(cs + (1 - cs)*Ny*Ny) + (BendingPart::begin.z - rotationPoint.z)*((1 - cs)*Ny*Nz - sn * Nx) + rotationPoint.y;
			Res.z = (BendingPart::begin.x - rotationPoint.x) *((1 - cs)*Nz*Nx - sn * Ny) + (BendingPart::begin.y - rotationPoint.y)*((1 - cs)*Nz*Ny + sn * Nx) + (BendingPart::begin.z - rotationPoint.z)*(cs + (1 - cs)*Nz*Nz) + rotationPoint.z;
		

			if (section) 
				if (section == BendingPart::section_count) 
					BendingPart::calculate2DLayer(BendingPart::end, next_straight_norm);
				else {
					// ќпределили направление поворота

					if (Res.x - Res_old.x >= 0) X = true; else X = false;
					if (Res.z - Res_old.z >= 0) Z = true; else Z = false;

					//определ€ем вектор нормали, в направлении которго будем поворачивать

					vect<PointType> norml(Res.x - Res_old.x, Res.y - Res_old.y, Res.z - Res_old.z, Z, X, Y);
					BendingPart::normals.push_back(norml);
		
					BendingPart::calculate2DLayer(Res, BendingPart::normals[BendingPart::normals.size() - 1]);
				}
			
			else 
				BendingPart::calculate2DLayer(BendingPart::begin, prev_straight_norm);
			
			
			Res_old = Res;
			iter++;
		}
				
		std::sort(BendingPart::coord.begin(), BendingPart::coord.end(), BendingPart::sort_coord_vect);

		BendingPart::compyteTubeFE<NetType, NVTR_2D>(iter);

		IMesh<PointType, NetType>::setNodesSize(BendingPart::coord.size());
		IMesh<PointType, NetType>::setElemsSize(BendingPart::nvtr.size());

		//IMesh<PointType, NetType>::writeMeshInGlassFormatIntoFiles("../../Glass/Test/inftry.dat", "../../Glass/Test/nvkat.dat",
		//	"../../Glass/Test/xyz.dat", "../../Glass/Test/nver.dat");
	};
	
};

#endif
