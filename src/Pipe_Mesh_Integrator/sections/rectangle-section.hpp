#ifndef _RECTANGLE_SECTION_HPP_
#define _RECTANGLE_SECTION_HPP_

#include "pipe-section.hpp"

template <class PointType, class NetType>
class RectangleSection : public PipeSection <PointType, NetType> {
private:

	real a,b;		// ����� ������ a � b 
	int a_n, b_n;  // ���-�� ��������� �� ������� a � b ��������������
	
	//������� �������� ��������� �� ����
	vector<NetType> nvtrTubeOnly() {
		vector<NetType> nv;

		int i, j;
		int a, b, c, d;
		int material;

		for (i = 0; i < b_n; i++)
			for (j = 0; j < a_n; j++) {
				material = (!i || !j || i == b_n - 1 || j == a_n - 1) ? RectangleSection::innner_material_id + 1 : RectangleSection::innner_material_id;
				a =  (a_n + 1)*i + j;
				b =  (a_n + 1)*i + j + 1;
				c =  (a_n + 1)*(i + 1) + j;
				d =  (a_n + 1)*(i + 1) + j + 1;
				NetType A(a, c, b, d, material);
				nv.push_back(A);
			}

		nv.shrink_to_fit();
		return nv;

	};

	//���������� ��������� ����� � �������
	vector<PointType> coordTubeOnly() {

		PointType T(0, 0, 0, 0);
		vector<PointType> points;

		int i, j, k, l = 0;
		real a_layer(a), b_layer(b);
		int number_of_points_on_layer = 2 * (a_n + b_n);
		int layer_id = 0;

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

		layer_id++;
		if (a_layer <= 0 || b_layer <= 0)
			throw runtime_error("The sum of the thicknesses of the layers is greater than or equal to the radius of the pipe! It is incorrect! The program is stopped. Edit the config file and try again!");

		RectangleSection::coor_on_layer = points.size();
		points.shrink_to_fit();
		return points;
	}
public:
	RectangleSection() {

		a = 2;
		a_n = 4;
		b = 1;
		b_n = 2;

		RectangleSection::number_of_inner_layers = 0;
	};
	
	RectangleSection(json cut_configs) {
		a = cut_configs["a-side"];
		a_n= cut_configs["a-splits"];
		b = cut_configs["b-side"];
		b_n = cut_configs["b-splits"];

		RectangleSection::innner_material_id = cut_configs["inner-material-id"];
	};
	
	~RectangleSection() {};

	int getIdOfExternalNodes() {
		return 2 * (a_n + b_n);
	};

};

#endif //_RECTANGLE_SECTION_HPP_