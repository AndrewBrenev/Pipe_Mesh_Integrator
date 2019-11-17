#ifndef _ROUND_SECTION_HPP_
#define _ROUND_SECTION_HPP_

#include "pipe-section.hpp"

template <class PointType, class NetType>
class RoundeSection :public PipeSection <PointType, NetType> {
private:

	real outter_R = 1;		// ������ ������� ����������
	size_t n;					// ����� ��������� ����� �������� 
	size_t start_ind = 0;		// ���-�� ������ � ����������� = 4*n*number_of_layers ��� ��������� ������ ������� �������� ���� ���������� �����
	
	vector<PointType> findPointsOfTheUnitCircle() {
		vector<PointType> points;
		PointType Temp;

		size_t p = n / (int)2;
		real alfa_step = 90 / n;

		{
			real sn = sin(45 * M_PI / 180.0);
			//������ ��������
			Temp.x = sn;
			Temp.z = sn;
			Temp.id = 0;
			points.push_back(Temp);

			//������ ��������
			Temp.x = -sn;
			Temp.z = sn;
			Temp.id = n;
			points.push_back(Temp);

			//������ ��������
			Temp.x = -sn;
			Temp.z = -sn;
			Temp.id = 2 * n;
			points.push_back(Temp);

			//��������� ��������
			Temp.x = sn;
			Temp.z = -sn;
			Temp.id = 3 * n;
			points.push_back(Temp);
		}

		//����� �� �������
		for (size_t k = 1; k <= p; k++)
			if ((n % 2) == 0 && k == p)
			{
				//������ ��������
				Temp.x = 0;
				Temp.z = 1;
				Temp.id = p;
				points.push_back(Temp);

				//������ ��������
				Temp.x = -1;
				Temp.z = 0;
				Temp.id = n + p;
				points.push_back(Temp);

				//������ ��������
				Temp.x = 0;
				Temp.z = -1;
				Temp.id = 2 * n + p;
				points.push_back(Temp);

				//��������� ��������
				Temp.x = 1;
				Temp.z = 0;
				Temp.id = 3 * n + p;
				points.push_back(Temp);
			}
			else {
				real s_alfa = sin((45 + k * alfa_step) * M_PI / 180.0);
				real c_alfa = cos((45 + k * alfa_step) * M_PI / 180.0);

				//������ ��������
				Temp.x = c_alfa;
				Temp.z = s_alfa;
				Temp.id = k;
				points.push_back(Temp);

				//������ ��������
				Temp.x = -c_alfa;
				Temp.z = s_alfa;
				Temp.id = n - k;
				points.push_back(Temp);

				//��������� ��������
				Temp.x = c_alfa;
				Temp.z = -s_alfa;
				Temp.id = 3 * n - k;
				points.push_back(Temp);

				//������ ��������
				Temp.x = -c_alfa;
				Temp.z = -s_alfa;
				Temp.id = 2 * n + k;
				points.push_back(Temp);

				//___________________________________________
				//������ ��������
				Temp.x = s_alfa;
				Temp.z = c_alfa;
				Temp.id = 4 * n - k;
				points.push_back(Temp);

				//������ ��������
				Temp.x = -s_alfa;
				Temp.z = c_alfa;
				Temp.id = n + k;
				points.push_back(Temp);

				//��������� ��������
				Temp.x = s_alfa;
				Temp.z = -c_alfa;
				Temp.id = 3 * n + k;
				points.push_back(Temp);

				//������ ��������
				Temp.x = -s_alfa;
				Temp.z = -c_alfa;
				Temp.id = 2 * n - k;
				points.push_back(Temp);
			}

		points.shrink_to_fit();
		return points;
	}

	//������� �������� ��������� �� ����
	vector<NetType> nvtrTubeOnly() override
	{

		//������� ������
		size_t i(0), j(0);
		int material = 0;

		size_t a, b, c, d;

		vector<NetType> nv;

		int number_of_layers = 0;
		for (int k = 0; k < RoundeSection::number_of_inner_layers; k++)
			for (j = 0; j < RoundeSection::section_layers[k].splits; j++)
			{
				//����� �� �����������
				for (i = 0; i < 4 * n; i++)
				{
					//������� ����� � �������
					if (i == 4 * n - 1) {
						a = i + 4 * n * number_of_layers;
						b = 4 * n * number_of_layers;
						c = i + 4 * n * (number_of_layers + 1);
						d = 4 * n * (number_of_layers + 1);
						NetType A(a, b, c, d, RoundeSection::section_layers[k].material);
						nv.push_back(A);
					}
					else {
						a = i + number_of_layers * 4 * n;
						b = i + 1 + number_of_layers * 4 * n;
						c = i + 4 * n * (number_of_layers + 1);
						d = i + 1 + 4 * n * (number_of_layers + 1);
						NetType A(a, b, c, d, RoundeSection::section_layers[k].material);
						nv.push_back(A);
					}
				}
				number_of_layers++;
			}

		//���������� ��������
		for (i = 0; i < n; i++) {

			//���������� ��
			for (j = 0; j < n; j++) {
				a = start_ind + (n + 1) * i + j;
				b = start_ind + (n + 1) * i + j + 1;
				c = start_ind + (n + 1) * (i + 1) + j;
				d = start_ind + (n + 1) * (i + 1) + j + 1;
				NetType A(a, b, c, d, RoundeSection::innner_material_id);
				nv.push_back(A);
			}

			//������� ������ ���������� �����
			a = number_of_layers * 4 * n + i;
			b = number_of_layers * 4 * n + i + 1;
			c = start_ind + i;
			d = start_ind + i + 1;
			NetType A(a, b, c, d, RoundeSection::innner_material_id);
			nv.push_back(A);

			// �� ����� ������
			a = number_of_layers * n * 4 + n + i;
			b = start_ind + i * (n + 1) + n;
			c = number_of_layers * n * 4 + n + i + 1;
			d = start_ind + (i + 1) * (n + 1) + n;
			NetType A4(b, a, d, c, RoundeSection::innner_material_id);
			nv.push_back(A4);

			//�� ����
			a = number_of_layers * n * 4 + 2 * n + i;
			b = start_ind + (n + 1) * (n + 1) - 1 - i;
			c = number_of_layers * n * 4 + 2 * n + i + 1;
			d = start_ind + (n + 1) * (n + 1) - 1 - (i + 1);
			NetType A5(d, b, c, a, RoundeSection::innner_material_id);
			nv.push_back(A5);

			// �� ������ ������
			a = start_ind + (n + 1) * i;
			b = (i) ? (number_of_layers + 1) * 4 * n - i :
				4 * n * number_of_layers;
			c = start_ind + (n + 1) * (i + 1);
			d = (number_of_layers + 1) * 4 * n - (i + 1);
			NetType A6(b, a, d, c, RoundeSection::innner_material_id);
			nv.push_back(A6);
		}
		nv.shrink_to_fit();
		return nv;
	};

	//���������� ��������� ����� � �������
	vector<PointType> coordTubeOnly() override 
	{

		size_t i, j;
		PointType Temp(0, 0, 0, 0);
		vector<PointType> points;

		vector<PointType> unitCircle = findPointsOfTheUnitCircle();

		size_t untin_size = unitCircle.size();

		real layer_R;
		if (outter_R > 0)
			layer_R = outter_R;
		else
			throw runtime_error("The outer radius of the pipe cannot be less than or equal to 0.");

		j = 0;
		for (int k = 0; k < RoundeSection::number_of_inner_layers; k++)
		{
			for (int l = 0; l < RoundeSection::section_layers[k].splits; l++)
			{
				for (i = 0; i < untin_size; i++)
				{
					Temp.x = unitCircle[i].x * layer_R;
					Temp.z = unitCircle[i].z * layer_R;
					Temp.id = unitCircle[i].id + untin_size * j;
					points.push_back(Temp);
				}
				layer_R -= RoundeSection::section_layers[k].thickness / RoundeSection::section_layers[k].splits;
				if (layer_R <= 0)
					throw runtime_error("The sum of the thicknesses of the layers is greater than or equal to the radius of the pipe! It is incorrect! The program is stopped. Edit the config file and try again!");

				j++;
			}
			if (k == (RoundeSection::number_of_inner_layers - 1))
				for (i = 0; i < untin_size; i++) {
					Temp.x = unitCircle[i].x * layer_R;
					Temp.z = unitCircle[i].z * layer_R;
					Temp.id = unitCircle[i].id + untin_size * j;
					points.push_back(Temp);
				}
		}

		// ��������� ����� �� ���������. 
		//  D = sqrt(2)a = 2r => a = sqrt(2) => ����. ��������� = 1/sqrt(2)
		real b = layer_R * 0.65;
		real b_step = 2 * b / (int)n;
		start_ind = points.size();
		//��������� ��������� �����
		for (i = 0; i <= n; i++)
			for (j = 0; j <= n; j++)
			{
				Temp.x = b - j * b_step;
				Temp.z = b - i * b_step;
				Temp.id = start_ind + (n + 1) * i + j;
				points.push_back(Temp);
			}
		RoundeSection::coor_on_layer = points.size();

		points.shrink_to_fit();
		return points;
	}

public:
	RoundeSection() {
		outter_R = 1;
		n = 6;
		RoundeSection::innner_material_id = 0;
		RoundeSection::number_of_inner_layers = 1;
		Layer def{ 4,0.2,1 };
		RoundeSection::section_layers.push_back(def);
	};
	
	RoundeSection(json cut_configs) 
	{
		outter_R = cut_configs["R"];
		n = cut_configs["splits"];
		RoundeSection::innner_material_id = cut_configs["inner-material-id"];
		RoundeSection::number_of_inner_layers = cut_configs["layers-count"];
		RoundeSection::section_layers.resize(RoundeSection::number_of_inner_layers);
		for (int i = 0; i < RoundeSection::number_of_inner_layers; i++)
		{
			RoundeSection::section_layers[i].material = cut_configs["layers"][i]["material-id"];
			RoundeSection::section_layers[i].splits = cut_configs["layers"][i]["splits"];
			RoundeSection::section_layers[i].thickness = cut_configs["layers"][i]["d"];
		}
	};
	~RoundeSection() {};

};

#endif //_ROUND_SECTION_HPP_