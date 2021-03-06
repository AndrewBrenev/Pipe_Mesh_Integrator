
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#pragma once

#include <exception>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <math.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <iomanip>

#include <array>
#include "../../external-libs/nlohmann/json.hpp"


using namespace std;
using json = nlohmann::json;
using real = double;



struct Point {
	//���������� ����� � ��������� ������������
	real x, y, z;

	//���������� ����� �������
	size_t id;

	//�����������
	Point() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
		id = 0;
	};
	Point(real p1, real p2, real p3, size_t k) {
		x = p1;
		y = p2;
		z = p3;
		id = k;
	};
	Point(real p1, real p2, real p3) {
		x = p1;
		y = p2;
		z = p3;
		id = 0;
	};
	// �������� ����������
	void setValue(real p1, real p2, real p3) {
		x = p1;
		y = p2;
		z = p3;
	};
};

class Plane {
private:
	// ���������� ������ ������, ������� � ���������
	size_t node[4];
public:

	Plane() {
		node[0] = 0;
		node[1] = 0;
		node[2] = 0;
		node[3] = 0;
	};
	Plane(size_t a, size_t b, size_t c, size_t d) {
		node[0] = a;
		node[1] = b;
		node[2] = c;
		node[3] = d;
		sort(&node[0], &node[4]);
	};
	void update(const int& id, const size_t& value) {
		if (id >= 0 && id < 4) {
			node[id] = value;
		}
	};

	void moveIds(const int& id) {
		for (int i = 0; i < 4; i++)
			node[i] += id;
	};

	void update() {
		sort(&node[0], &node[4]);
	};
};

class NVTR {
public:
	//������ ���������� ������� ������ ��
	vector<size_t> n;

	Plane planes[6];
	//���������� ����� ��
	int id;
	//����� ���������
	int material;
	NVTR() {
		n.resize(8);
		n[0] = 0; n[1] = 0; n[2] = 0; n[3] = 0;
		n[4] = 0; n[5] = 0; n[6] = 0; n[7] = 0;
		id = 0; material = 0;
	};
	NVTR(size_t p0, size_t p1, size_t p2, size_t p3,
		 size_t p4, size_t p5, size_t p6, size_t p7, int i, int ma) {
		n.resize(8);
		n[0] = p0; n[1] = p1; n[2] = p2; n[3] = p3;
		n[4] = p4; n[5] = p5; n[6] = p6; n[7] = p7;
		id = i; material = ma;
	};
	void buildPlanes() {
		planes[0] = Plane(n[0], n[1], n[2], n[3]);
		planes[1] = Plane(n[4], n[5], n[6], n[7]);
		planes[2] = Plane(n[0], n[1], n[4], n[5]);
		planes[3] = Plane(n[1], n[3], n[5], n[7]);
		planes[4] = Plane(n[2], n[3], n[6], n[7]);
		planes[5] = Plane(n[0], n[2], n[4], n[6]);
	}
};


struct Layer {
	int material;
	int splits;
	real thickness;

	Layer() {
		material = 0;
		splits = 2;
		thickness = 0.5;
	};
	Layer(int _splits, real d, int _material_id) {
		material = _material_id;
		splits = _splits;
		thickness = d;
	};
};

struct NVTR_2D
{
	int material;
	size_t n[4];

	NVTR_2D() {
		n[0] = 0;
		n[1] = 0;
		n[2] = 0;
		n[3] = 0;
		material = 0;
	};
	NVTR_2D(const size_t a, const size_t b, const size_t c, const size_t d, const int m) {
		n[0] = a;
		n[1] = b;
		n[2] = c;
		n[3] = d;
		material = m;
	};
};
