#ifndef _SQUARE_SECTION_HPP_
#define _SQUARE_SECTION_HPP_

#include "pipe-section.hpp"

template <class PointType, class NetType, class SectionType>
class SquareSection : public PipeSection<PointType, NetType, SectionType> {
private:

	//Задание конечных элементов на слое
	vector<NetType> nvtrTubeOnly() {
		int n = SquareSection::n;
		int l = SquareSection::l;

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

public:
	SquareSection() {
		SectionType A(0, 0, 0, 1, 0.8);
		SquareSection::face = A;
	};
	SquareSection(SectionType round) {
		SquareSection::face = round;
		SquareSection::n = 6;
		SquareSection::l = 5;
	};
	~SquareSection() {};

	//Нахождение координат трубы в сечении
	vector<PointType> coordTubeOnly(SectionType c, int id) {

		int n = SquareSection::n;
		int l = SquareSection::l;

		PointType T(0,0,0,0);
		vector<PointType> tmp;
		
		real b = c.R*2;
		real a = n*b / l;
		real a_step = a / n;
		real b_step = b / l;
		

		T.z = b / 2;
		int start_id = 0;

		// Движение вниз по Oy вправо по Ox
		for (int j = 0; j <= l; j++) {
			//Движение по OX
			T.x =  - a/2;

			if (j == l )
				T.z = - b / 2; 
			else
				if (j)
					T.z -= b_step; 

			for (int i = 0; i <= n; i++)
			{
				T.id = start_id + id;
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
};

#endif //_SQUARE_SECTION_HPP_