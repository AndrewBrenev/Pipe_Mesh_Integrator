#ifndef _ROUND_SECTION_HPP_
#define _ROUND_SECTION_HPP_

#include "pipe-section.hpp"

template <class PointType, class NetType, class SectionType>
class RoundeSection :public PipeSection <PointType, NetType, SectionType > {
private:
	size_t start_ind;		//���-�� ������ � �����������
	
	vector<PointType> findPointsOfTheUnitCircle() {
		vector<PointType> points;
		PointType Temp;
		int n = RoundeSection::n;
		int p = n / (int)2;
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
		for (int k = 1; k <= p; k++){
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

		if (!n % 2)
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
		
		return points;
	}

	//������� �������� ��������� �� ����
	vector<NetType> nvtrTubeOnly() {
		//������� ������
		int i, j, material = 0;
		int a, b, c, d;

		int n = RoundeSection::n;
		int l = RoundeSection::l;
		vector<NetType> nv;
		//����� �� �����������
		for (int k = 0; k < 4 * n; k++)
			for (i = 0; i <= l; i++)
			{
				if (i == l) material = OIL; else
					if (  i > 6 || i < 2 )  material = IRON; else material = AIR;
				//������� ����� � �������
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
		//������� ������ ���������� �����
		for (i = 0; i < n - 1; i++) {
			a = (l + 2)*i + l + 1;
			b = (l + 2)*(i + 1) + l + 1;
			c = start_ind + i;
			d = start_ind + i + 1;
			NetType A(a, c, b, d, OIL);
			nv.push_back(A);
		}
		//������� ����� � ��������
		a = (l + 2)*n - 1;
		b = (l + 2)*n + l + 1;
		c = start_ind + n - 1;
		d = (l + 2)*(n + 1) + l + 1;
		NetType A1(a, c, b, d, OIL);
		nv.push_back(A1);
		//������ �����
		a = (l + 2)*(2 * n) - 1;
		b = RoundeSection::coor_on_layer - 1;
		c = (l + 2)*(2 * n) + l + 1;
		d = (l + 2)*(2 * n + 2) - 1;
		NetType A2(b, d, a, c, OIL);
		nv.push_back(A2);
		//������ ������
		a = (l + 2)*(3 * n) - 1;
		b = RoundeSection::coor_on_layer - n + 1;
		c = (l + 2)*(3 * n + 1) - 1;
		d = (l + 2)*(3 * n + 2) - 1;
		NetType A3(d, c, b, a, OIL);
		nv.push_back(A3);

		for (i = 1; i < n - 1; i++)
		{
			// �� ����� ������
			a = (l + 2)*(n + i) + l + 1;
			b = start_ind + i * (n - 1);
			c = (l + 2)*(n + i + 1) + l + 1;
			d = start_ind + (i + 1)*(n - 1);
			NetType A4(b, d, a, c, OIL);
			nv.push_back(A4);
			//�� ����
			a = (l + 2)*(2 * n + 1 + i) - 1;
			b = RoundeSection::coor_on_layer - i;
			c = (l + 2)*(2 * n + 2 + i) - 1;
			d = RoundeSection::coor_on_layer - i - 1;
			NetType A5(d, c, b, a, OIL);
			nv.push_back(A5);
			// �� ������ ������
			a = start_ind + (n - 1)*(i - 1) + 1;
			b = start_ind - (i - 1)*(l + 2);
			c = start_ind + (n - 1)*i + 1;
			d = start_ind - (i)*(l + 2);
			NetType A6(b, d, a, c, OIL);
			nv.push_back(A6);
		}
		//���������� ��
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

	//���������� ��������� ����� � �������
	vector<PointType> coordTubeOnly() {

		int n = RoundeSection::n;
		int l = RoundeSection::l;

		PointType Temp(0, 0, 0, 0);
		vector<PointType> unitCircle = findPointsOfTheUnitCircle();
		vector<PointType> points;

		int i, j;
		real b = (RoundeSection::face.R - RoundeSection::face.d)*0.6;
		real b_step = 2 * b / (int)n;

		int s = unitCircle.size();
		for (i = 0; i < s; i++) {
			Temp.x = unitCircle[i].x * (RoundeSection::face.R - RoundeSection::face.d);
			Temp.z = unitCircle[i].z * (RoundeSection::face.R - RoundeSection::face.d);
			Temp.id = unitCircle[i].id + s;

			points.push_back(Temp);
			points.push_back(unitCircle[i]);
		}

		start_ind = points.size();
		//��������� ��������� �����
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++) {
				Temp.x = b - j * b_step;
				Temp.z = b - i * b_step;
				Temp.id = start_ind + n*i + j;
				points.push_back(Temp);
			}
		RoundeSection::coor_on_layer = points.size();
		return points;
	}

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
};

#endif //_ROUND_SECTION_HPP_