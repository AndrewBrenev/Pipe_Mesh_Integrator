#ifndef _RECTANGLE_SECTION_HPP_
#define _RECTANGLE_SECTION_HPP_

#include "pipe-section.hpp"

template <class PointType, class NetType>
class RectangleSection : public PipeSection <PointType, NetType> {
private:

	real a,b;		// Длины сторон a и b 
	int a_n, b_n;  // Кол-во разбиений по стороне a и b соответственно
	
	//Задание конечных элементов на слое
	vector<NetType> nvtrTubeOnly() {
		vector<NetType> nv;

		int i, j;
		int a, b, c, d;

		for (i = 0; i < b_n; i++)
			for (j = 0; j < a_n; j++) {
				a =  (a_n + 1)*i + j;
				b =  (a_n + 1)*i + j + 1;
				c =  (a_n + 1)*(i + 1) + j;
				d =  (a_n + 1)*(i + 1) + j + 1;
				NetType A(a, c, b, d, RectangleSection::innner_material_id);
				nv.push_back(A);
			}

		return nv;

	};

	//Нахождение координат трубы в сечении
	vector<PointType> coordTubeOnly() {

		PointType T(0, 0, 0, 0);
		vector<PointType> points;

		int i,j,k,l = 0;
		real a_layer(a), b_layer(b);
		int number_of_points_on_layer = 2 * (a_n + b_n);
		int layer_id = 0;

		for ( k = 0; k < RectangleSection::number_of_inner_layers; k++)
		{
			for (l = 0; l < RectangleSection::section_layers[k].splits; l++)
			{

				real a_layer_step = a_layer / a_n;
				real b_layer_step = b_layer / b_n;

				for (i = 0; i <= b_n; i++)
					for (j = 0; j <= a_n; j++)
					{
						T.x = a_layer - j * a_layer_step;
						T.z = b_layer - i * b_layer_step;
						T.id = (a_n + 1)*i + j + layer_id * number_of_points_on_layer;
						points.push_back(T);
					}

				a_layer -= RectangleSection::section_layers[k].thickness / RectangleSection::section_layers[k].splits;
				b_layer -= RectangleSection::section_layers[k].thickness / RectangleSection::section_layers[k].splits;

				layer_id++;
				if (a_layer <= 0 || b_layer <= 0)
					throw runtime_error("The sum of the thicknesses of the layers is greater than or equal to the radius of the pipe! It is incorrect! The program is stopped. Edit the config file and try again!");

			}
		}
		RectangleSection::coor_on_layer = points.size();
		return points;
	}
public:
	RectangleSection() {

		a = 2;
		a_n = 4;
		b = 1;
		b_n = 2;

		Layer def{ 4,0.2,1 };
		RectangleSection::section_layers.push_back(def);
	};
	
	RectangleSection(json cut_configs) {
		a = cut_configs["a-side"];
		a_n= cut_configs["a-splits"];
		b = cut_configs["b-side"];
		b_n = cut_configs["b-splits"];

		RectangleSection::innner_material_id = cut_configs["inner-material-id"];
		RectangleSection::number_of_inner_layers = cut_configs["layers-count"];
		RectangleSection::section_layers.resize(RectangleSection::number_of_inner_layers);
		for (int i = 0; i < RectangleSection::number_of_inner_layers; i++) {
			RectangleSection::section_layers[i].material = cut_configs["layers"][i]["material-id"];
			RectangleSection::section_layers[i].splits = cut_configs["layers"][i]["splits"];
			RectangleSection::section_layers[i].thickness = cut_configs["layers"][i]["d"];
		}
	};
	
	~RectangleSection() {};

	int getIdOfExternalNodes() {
		return 2 * (a_n + b_n);
	};

};

#endif //_RECTANGLE_SECTION_HPP_