#ifndef _SQUARE_SECTION_HPP_
#define _SQUARE_SECTION_HPP_

#include "pipe-section.hpp"

template <class PointType, class NetType>
class SquareSection : public PipeSection<PointType, NetType> {
private:

	real side = 1;
	int l = 5;

	//Задание конечных элементов на слое
	vector<NetType> nvtrTubeOnly() {
		int n = SquareSection::n;
	
		int i, j, material;
		int a, b, c, d;
		int point_id = 0;
		vector<NetType> nv;

		for (int k = 0; k <= l; k++)
			for (i = 0; i <= n; i++) {
				if (i != n && k != l) {

					if (k == 0 || i == 0 || k == l - 1 || i == n - 1) 
						material = IRON;
					else 
						material = OIL;

					a = point_id;
					b = point_id + 1;
					c = point_id + n + 1;
					d = point_id + n + 2;
					NetType A(a, b, c, d, material);
					nv.push_back(A);
				}
				point_id++;
			}
		return nv;
	};
	//Нахождение координат трубы в сечении
	vector<PointType> coordTubeOnly() {

		int n = SquareSection::n;
	
		PointType T(0, 0, 0, 0);
		vector<PointType> tmp;

		real b = side;
		real a = n * b / l;
		real a_step = a / n;
		real b_step = b / l;
		
		T.z = b / 2;
		int start_id = 0;

		// Движение вниз по Oy вправо по Ox
		for (int j = 0; j <= l; j++) {
			//Движение по OX
			T.x = -a / 2;

			if (j == l)
				T.z = -b / 2;
			else
				if (j)
					T.z -= b_step;

			for (int i = 0; i <= n; i++)
			{
				T.id = start_id;
				if (i == n) T.x = a / 2;
				else
					if (i) T.x += a_step;

				tmp.push_back(T);
				start_id++;
			}

		}
		SquareSection::coor_on_layer = tmp.size();
		return tmp;
	}
public:
	SquareSection() {
		
	};
	SquareSection(real square_side) {
		side = square_side;
		SquareSection::n = 6;
	};
	SquareSection(json cut_configs) {
	
	};
	SquareSection(real square_side ,int _n,int _l) {
		side = square_side;
		SquareSection::n = _n;
		SquareSection::l = _l;
	};
	~SquareSection() {};

};

#endif //_SQUARE_SECTION_HPP_