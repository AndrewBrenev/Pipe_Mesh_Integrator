#ifndef _ROUND_SECTION_HPP_
#define _ROUND_SECTION_HPP_

#include "pipe-section.hpp"

template <class PointType, class NetType, class SectionType>
class RoundeSection :public PipeSection <PointType, NetType, SectionType > {
private:
	size_t start_ind;		//кол-во вершин в окружностях
	
	// поиск  точки пересечения прямой, заданной двумя точками, и окружностью
	void circle_point(real &res_x, real &res_y,
		const real x1, const real y1,
		const real x2, const real y2,
		const real x0, const real y0, const real r) {
		if (x1 == x2) {
			res_x = x1;
			res_y = y0 + r;
		}
		if (y1 == y2) {
			res_x = x0 + r;
			res_y = y1;
		}
		else {
			real dx = x1 - x2;
			real dy = y1 - y2;
			real l = sqrt(dx*dx + dy * dy);
			real c = dx / l;
			real s = dy / l;
			real x = x0 + c * r;
			real y = y0 + s * r;
			res_x = x;
			res_y = y;
		}
	};

	//Задание конечных элементов на слое
	vector<NetType> nvtrTubeOnly() {
		//Займёмся сеткой
		int i, j, material = 0;
		int a, b, c, d;

		int n = RoundeSection::n;
		int l = RoundeSection::l;
		vector<NetType> nv;
		//Сетка по окружностям
		for (int k = 0; k < 4 * n; k++)
			for (i = 0; i <= l; i++)
			{
				 
				if (i == l) material = OIL; else
					if (  i > 6 || i < 2 )  material = IRON; else material = AIR;
				//Склейка конца с началом
				if (k == 4 * n - 1) {
					a = (l + 2)*k + i;
					b = (l + 2)*k + i + 1;
					c = i;
					d = i + 1;
					NetType A(c, a, d, b, material);
					nv.push_back(A);
				}
				else {
					a = (l + 2)*k + i;
					b = (l + 2)*k + i + 1;
					c = (l + 2)*(k + 1) + i;
					d = (l + 2)*(k + 1) + i + 1;
					if (k / n == 0 || k / n == 2)
					{
						NetType A(a, b, c, d, material); nv.push_back(A);
					}
					else
					{
						NetType A(b, d, a, c, material); nv.push_back(A);
					}

				}
			}
		//Верхняя полоса внутренней сетки
		for (i = 0; i < n - 1; i++) {
			a = (l + 2)*i + l + 1;
			b = (l + 2)*(i + 1) + l + 1;
			c = start_ind + i;
			d = start_ind + i + 1;
			NetType A(a, c, b, d, OIL);
			nv.push_back(A);
		}
		//Верхняя левая в квадрате
		a = (l + 2)*n - 1;
		b = (l + 2)*n + l + 1;
		c = start_ind + n - 1;
		d = (l + 2)*(n + 1) + l + 1;
		NetType A1(a, c, b, d, OIL);
		nv.push_back(A1);
		//Нижняя левая
		a = (l + 2)*(2 * n) - 1;
		b = RoundeSection::coor_on_layer - 1;
		c = (l + 2)*(2 * n) + l + 1;
		d = (l + 2)*(2 * n + 2) - 1;
		NetType A2(b, d, a, c, OIL);
		nv.push_back(A2);
		//Нижняя правая
		a = (l + 2)*(3 * n) - 1;
		b = RoundeSection::coor_on_layer - n + 1;
		c = (l + 2)*(3 * n + 1) - 1;
		d = (l + 2)*(3 * n + 2) - 1;
		NetType A3(d, c, b, a, OIL);
		nv.push_back(A3);

		for (i = 1; i < n - 1; i++)
		{
			// по левой стенке
			a = (l + 2)*(n + i) + l + 1;
			b = start_ind + i * (n - 1);
			c = (l + 2)*(n + i + 1) + l + 1;
			d = start_ind + (i + 1)*(n - 1);
			NetType A4(b, d, a, c, OIL);
			nv.push_back(A4);
			//по низу
			a = (l + 2)*(2 * n + 1 + i) - 1;
			b = RoundeSection::coor_on_layer - i;
			c = (l + 2)*(2 * n + 2 + i) - 1;
			d = RoundeSection::coor_on_layer - i - 1;
			NetType A5(d, c, b, a, OIL);
			nv.push_back(A5);
			// По правой стенке
			a = start_ind + (n - 1)*(i - 1) + 1;
			b = start_ind - (i - 1)*(l + 2);
			c = start_ind + (n - 1)*i + 1;
			d = start_ind - (i)*(l + 2);
			NetType A6(b, d, a, c, OIL);
			nv.push_back(A6);
		}
		//Внутренние КЭ
		for (i = 1; i < n - 1; i++)
			for (j = 1; j < n - 1; j++) {
				a = start_ind + (n - 1)*(i - 1) + j;
				b = start_ind + (n - 1)*(i - 1) + j + 1;
				c = start_ind + (n - 1)*(i)+j;
				d = start_ind + (n - 1)*i + j + 1;
				NetType A(a, c, b, d, OIL);
				nv.push_back(A);
			}
		return nv;
	};


public:
	RoundeSection() {
		SectionType A(0, 0, 0, 1, 0.8);
		RoundeSection::face = A;
	};
	RoundeSection(SectionType circle) {
		RoundeSection::face = circle;
		RoundeSection::n = 6;
		RoundeSection::l = 5;
	};
	RoundeSection(SectionType circle,int _n,int _l) {
		RoundeSection::face = circle;
		RoundeSection::n = _n;
		RoundeSection::l = _l;
	};
	~RoundeSection() {};

	//Нахождение координат трубы в сечении
	vector<PointType> coordTubeOnly(const int id) {

		int n = RoundeSection::n;
		int l = RoundeSection::l;

		PointType Temp(0, 0, 0, 0);
		vector<PointType> tmp;
		int i, j;
		real a = RoundeSection::face.R*1.1;
		real b = (RoundeSection::face.R- RoundeSection::face.d)*0.6;
		real a_step = 2 * a / (int)n;
		real b_step = 2 * b / (int)n;
		real p = a / a_step;

		real step = RoundeSection::face.d / (int)l;

		//Точки на внутреннем квадрате
		for (int k = 0; k < n; k++)
		{
			//Верхняя сторона
			Temp.x = b - k * b_step;
			Temp.z = b;
			Temp.id = (l + 1)*(k + 1) + k + id;
			tmp.push_back(Temp);

			//Левая сторона
			Temp.x = -b;
			Temp.z = b - k * b_step;
			Temp.id = (l + 2)*(n + k) + l + 1 + id;
			tmp.push_back(Temp);

			//Нижняя сторона
			Temp.x = -b + k * b_step;
			Temp.z = -b;
			Temp.id = (k + 2 * n) * (l + 2) + l + 1 + id;
			tmp.push_back(Temp);

			//Правая сторона
			Temp.x = b;
			Temp.z = -b + k * b_step;
			Temp.id = (k + 3 * n) * (l + 2) + l + 1 + id;
			tmp.push_back(Temp);

		}
		//Точки на радиусах
		for (int k = 0; k <= p; k++)
		{
			// Если попали ровно в середину стороны
			if (k == p && (a - p * a_step <= 2))
				for (i = 0; i <= l; i++)
				{
					//Вертикальная верх
					Temp.x = 0;
					Temp.z = RoundeSection::face.R - i * step;
					Temp.id = (l + 2)*k + i + id;
					tmp.push_back(Temp);
					//Горизонтальная лево
					Temp.x = -RoundeSection::face.R + i * step;
					Temp.z = 0;
					Temp.id = (l + 2) * n + (l + 2)*k + i + id;
					tmp.push_back(Temp);
					//Вертикальная низ
					Temp.x = 0;
					Temp.z = -RoundeSection::face.R + i * step;
					Temp.id = 2 * n *(l + 2) + (l + 2)*k + i + id;
					tmp.push_back(Temp);
					//Горизонтальная право
					Temp.x = RoundeSection::face.R - i * step;
					Temp.z = 0;
					Temp.id = 3 * n*(l + 2) + (l + 2)*k + i + id;
					tmp.push_back(Temp);
				}
			else {

				real x1, y1, x2, y2;
				//Точка с внешнего квадрата
				x1 = a - k * a_step;
				y1 = a;

				// Точка с внутреннего квадрата
				x2 = b - k * b_step;
				y2 = b;

				real r_x, r_z, R_x, R_z;
				circle_point(R_x, R_z, x1, y1, x2, y2, 0.0, 0.0, RoundeSection::face.R);
				circle_point(r_x, r_z, x1, y1, x2, y2, 0.0, 0.0, RoundeSection::face.R- RoundeSection::face.d);

				real x_step = abs(R_x - r_x) / l;
				real z_step = abs(R_z - r_z) / l;

				//концентрические окружности
				for (i = 0; i <= l; i++) {

					//Верх право
					Temp.x = R_x - i * x_step;
					Temp.z = R_z - i * z_step;
					Temp.id = (l + 2)*k + i + id;
					tmp.push_back(Temp);

					real dx = Temp.x;
					real dy = Temp.z;
					Point Temp1(0, 0, 0, 0);

					//Верх лево
					if (k != 0) {
						Temp1.x = -dx;
						Temp1.z = Temp.z;
						Temp1.id = (l + 2)*n - (l + 2)*k + i + id;
						tmp.push_back(Temp1);
					}

					//низ лево
					Temp1.x = -dx;
					Temp1.z = -dy;
					Temp1.id = (l + 2)*n * 2 + (l + 2)*k + i + id;
					tmp.push_back(Temp1);

					if (k != 0) {
						//низ право
						Temp1.x = Temp.x;
						Temp1.z = -dy;
						Temp1.id = (l + 2)*n * 3 - (l + 2)*k + i + id;
						tmp.push_back(Temp1);
					}

					//Право верх
					if (k != 0) {
						Temp1.x = dy;
						Temp1.z = dx;
						Temp1.id = (l + 2)* n * 4 + i - (l + 2)*k + id;
						tmp.push_back(Temp1);
					}

					//Право низ
					Temp1.x = dy;
					Temp1.z = -dx;
					Temp1.id = (l + 2)*n * 3 + i + (l + 2)*k + id;
					tmp.push_back(Temp1);

					//Лево верх
					Temp1.x = -dy;
					Temp1.z = dx;
					Temp1.id = (l + 2)*n + i + (l + 2)*k + id;
					tmp.push_back(Temp1);

					if (k != 0) {
						//Лево низ
						Temp1.x = -dy;
						Temp1.z = -dx;
						Temp1.id = (l + 2)*n * 2 + i - (l + 2)*k + id;
						tmp.push_back(Temp1);
					}

				}
			}
		}
		start_ind = tmp.size() - 1;
		//Добавляем внутренюю сетку
		for (i = 1; i < n; i++)
			for (j = 1; j < n; j++) {
				Temp.x = b - j * b_step;
				Temp.z = b - i * b_step;
				Temp.id = start_ind + (n - 1)*(i - 1) + j + id;
				tmp.push_back(Temp);
			}
		RoundeSection::coor_on_layer = tmp.size();
		return tmp;
	}

};

#endif //_ROUND_SECTION_HPP_