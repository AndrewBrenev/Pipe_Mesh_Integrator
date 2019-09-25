#ifndef _ROUND_SECTION_HPP_
#define _ROUND_SECTION_HPP_

#include "pipe-section.hpp"

template <class PointType, class NetType>
class RoundeSection :public PipeSection <PointType, NetType> {
private:

	real outter_R =1;

	size_t start_ind;		//кол-во вершин в окружностях
	
	vector<PointType> findPointsOfTheUnitCircle() {
		vector<PointType> points;
		PointType Temp;
		int n = RoundeSection::n;
		int p = n / (int)2;
		real alfa_step = 90 / n;

		{
			real sn = sin(45 * M_PI / 180.0);
			//первая четверть
			Temp.x = sn;
			Temp.z = sn;
			Temp.id = 0;
			points.push_back(Temp);

			//вторая четверть
			Temp.x = -sn;
			Temp.z = sn;
			Temp.id = n;
			points.push_back(Temp);

			//третья четверть
			Temp.x = -sn;
			Temp.z = -sn;
			Temp.id = 2 * n;
			points.push_back(Temp);

			//четвертая четверть
			Temp.x = sn;
			Temp.z = -sn;
			Temp.id = 3 * n;
			points.push_back(Temp);
		}

		//Точки на радиусе
		for (int k = 1; k <= p; k++)
			if ((n % 2) == 0 && k == p)
			{
				//первая четверть
				Temp.x = 0;
				Temp.z = 1;
				Temp.id = p;
				points.push_back(Temp);

				//вторая четверть
				Temp.x = -1;
				Temp.z = 0;
				Temp.id = n + p;
				points.push_back(Temp);

				//третья четверть
				Temp.x = 0;
				Temp.z = -1;
				Temp.id = 2 * n + p;
				points.push_back(Temp);

				//четвертая четверть
				Temp.x = 1;
				Temp.z = 0;
				Temp.id = 3 * n + p;
				points.push_back(Temp);
			}
			else {
				real s_alfa = sin((45 + k * alfa_step) * M_PI / 180.0);
				real c_alfa = cos((45 + k * alfa_step) * M_PI / 180.0);

				//первая четверть
				Temp.x = c_alfa;
				Temp.z = s_alfa;
				Temp.id = k;
				points.push_back(Temp);

				//вторая четверть
				Temp.x = -c_alfa;
				Temp.z = s_alfa;
				Temp.id = n - k;
				points.push_back(Temp);

				//четвертая четверть
				Temp.x = c_alfa;
				Temp.z = -s_alfa;
				Temp.id = 3 * n - k;
				points.push_back(Temp);

				//третья четверть
				Temp.x = -c_alfa;
				Temp.z = -s_alfa;
				Temp.id = 2 * n + k;
				points.push_back(Temp);

				//___________________________________________
				//первая четверть
				Temp.x = s_alfa;
				Temp.z = c_alfa;
				Temp.id = 4 * n - k;
				points.push_back(Temp);

				//вторая четверть
				Temp.x = -s_alfa;
				Temp.z = c_alfa;
				Temp.id = n + k;
				points.push_back(Temp);

				//четвертая четверть
				Temp.x = s_alfa;
				Temp.z = -c_alfa;
				Temp.id = 3 * n + k;
				points.push_back(Temp);

				//третья четверть
				Temp.x = -s_alfa;
				Temp.z = -c_alfa;
				Temp.id = 2 * n - k;
				points.push_back(Temp);
			}

		return points;
	}


	//Задание конечных элементов на слое
	vector<NetType> nvtrTubeOnly() {
		//Займёмся сеткой
		int i(0), j, material = 0;

		int a, b, c, d;

		int n = RoundeSection::n;
		vector<NetType> nv;

		//Сетка по окружностям
		for (int i = 0; i < 4 * n; i++)
		{
			material = IRON;
			//Склейка конца с началом
			if (i == 4 * n - 1) {
				a = i;
				b = 0;
				c = 4 * n + i;
				d = 4*n;
				NetType A(c, a, d, b, material);
				nv.push_back(A);
			}
			else {
				a =  i;
				b = i + 1;
				c = 4*n + i;
				d = 4*n + i + 1;
				NetType A(a,b,c,d, material); nv.push_back(A);
			}
		}
		
		for (i = 0; i < n; i++) {

			//внутренние КЭ
			for (j = 0; j < n; j++) {
				a = start_ind + (n + 1)*i + j;
				b = start_ind + (n + 1)*i + j + 1;
				c = start_ind + (n + 1)*(i + 1) + j;
				d = start_ind + (n + 1)*(i + 1) + j + 1;
				NetType A(a, c, b, d, OIL);
				nv.push_back(A);
			}

			//Верхняя полоса внутренней сетки
			a = 4 * n + i;
			b = 4 * n + i + 1;
			c = start_ind + i;
			d = start_ind + i + 1;
			NetType A(a, c, b, d, OIL);
			nv.push_back(A);

			// по левой стенке
			a = n * 4 + n + i;
			b = start_ind + i*(n+1)+n;
			c = n * 4 + n + i + 1;
			d = start_ind + (i + 1)*(n+1) + n;
			NetType A4(b, d, a, c, OIL);
			nv.push_back(A4);

			//по низу
			a = n * 4 + 2 * n + i;
			b = start_ind + (n + 1)*(n + 1) - 1 - i;
			c = n * 4 + 2*n + i + 1;
			d = start_ind + (n + 1)*(n + 1) - 1 - (i + 1);
			NetType A5(d, c, b, a, OIL);
			nv.push_back(A5);

			// По правой стенке
			a = start_ind + (n+1) * i;
			b = (i) ? 8 * n - i: 4 * n;
			c = start_ind + (n+1)*(i + 1);
			d = 8 * n - (i + 1);
			NetType A6(b, d, a, c, OIL);
			nv.push_back(A6);
		}
			
		return nv;
	};

	//Нахождение координат трубы в сечении
	vector<PointType> coordTubeOnly() {
		int i, j;
		PointType Temp(0, 0, 0, 0);
		vector<PointType> points;

		vector<PointType> unitCircle = findPointsOfTheUnitCircle();

		int n = RoundeSection::n;

		size_t untin_size = unitCircle.size();
		real layer_R = outter_R;
		j = 0;
		for (int k = 0; k < RoundeSection::number_of_inner_layers; k++) {
			for (int l = 0; l < RoundeSection::section_layers[k].splits; l++) {
				for (i = 0; i < untin_size; i++) {
					Temp.x = unitCircle[i].x *layer_R;
					Temp.z = unitCircle[i].z *layer_R;
					Temp.id = unitCircle[i].id + untin_size * j;
					points.push_back(Temp);
				}
				layer_R -= RoundeSection::section_layers[k].thickness / RoundeSection::section_layers[k].splits;
				j++;
			}
			if (k == (RoundeSection::number_of_inner_layers - 1))
				for (i = 0; i < untin_size; i++) {
					Temp.x = unitCircle[i].x *layer_R;
					Temp.z = unitCircle[i].z *layer_R;
					Temp.id = unitCircle[i].id + untin_size * j;
					points.push_back(Temp);
				}
		}

		real b = layer_R * 0.9;
		real b_step = 2 * layer_R / (int)n;
		start_ind = points.size();
		//Добавляем внутренюю сетку
		for (i = 0; i <= n; i++)
			for (j = 0; j <= n; j++) {
				Temp.x = b - j * b_step;
				Temp.z = b - i * b_step;
				Temp.id = start_ind + (n + 1)*i + j;
				points.push_back(Temp);
			}
		RoundeSection::coor_on_layer = points.size();
		return points;
	}

public:
	RoundeSection() {
	
	};
	RoundeSection(json cut_configs) {
		outter_R = cut_configs["R"];
		RoundeSection::n = cut_configs["splits"];
		RoundeSection::innner_material_id = cut_configs["inner-material-id"];
		RoundeSection::number_of_inner_layers = cut_configs["layers-count"];
		RoundeSection::section_layers.resize(RoundeSection::number_of_inner_layers);
		for (int i = 0; i < RoundeSection::number_of_inner_layers; i++) {
			RoundeSection::section_layers[i].material = cut_configs["layers"][i]["material-id"];
			RoundeSection::section_layers[i].splits = cut_configs["layers"][i]["splits"];
			RoundeSection::section_layers[i].thickness = cut_configs["layers"][i]["d"];
		}
	};
	~RoundeSection() {};
};

#endif //_ROUND_SECTION_HPP_