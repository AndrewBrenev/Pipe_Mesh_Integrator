
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
#include <memory>
#include <unordered_set>
#include <functional>

#include <array>
#include "../../external-libs/nlohmann/json.hpp"


using namespace std;
using json = nlohmann::json;
using real = double;



struct Point {
	//Координаты точки в трёхмерном пространстве
	real x, y, z;

	//Глобальный номер вершины
	size_t id;

	//конструктор
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
	// Изменить координату
	void setValue(real p1, real p2, real p3) {
		x = p1;
		y = p2;
		z = p3;
	};

	// Изменить координату
	void setId(const size_t _id) {
		this->id = _id;
	};
	double length() {
		return sqrt(x * x + y * y + z * z);
	}

	double calculateDistanceToPoint(const Point& otherPoint) const
	{
		return sqrt((this->x - otherPoint.x) * (this->x - otherPoint.x)
			+ (this->y - otherPoint.y) * (this->y - otherPoint.y)
			+ (this->z - otherPoint.z) * (this->z - otherPoint.z));
	}

	bool operator==(const Point& other) const
	{
		return (this->id == other.id);
	}
};

class Plane {
private:
	// Глобальные номера вершин, лежащих в плоскости
	size_t node[4];

	double A, B, C, D;
public:

	Plane() {
		node[0] = 0;
		node[1] = 0;
		node[2] = 0;
		node[3] = 0;
		setNormal(1, 1, 1, 0);
	};
	Plane(size_t a, size_t b, size_t c, size_t d) {
		node[0] = a;
		node[1] = b;
		node[2] = c;
		node[3] = d;
		sort(&node[0], &node[4]);
		setNormal(1, 1, 1, 0);
	};
	void update(const int& id, const size_t& value) {
		if (id >= 0 && id < 4) {
			node[id] = value;
		}
	};
	void setNormal(double _A, double _B, double _C, double _D) {
		this->A = _A;	this->B = _B; this->C = _C; this->D = _D;
	}
	void setIds(size_t a, size_t b, size_t c, size_t d) {
		node[0] = a;
		node[1] = b;
		node[2] = c;
		node[3] = d;
		sort(&node[0], &node[4]);
	}
	void getNormal(double& _A, double& _B, double& _C, double& _D)  const
	{
		_A = this->A;	_B = this->B; _C = this->C; _D = this->D;
	}
	Point getNormal() {
		return Point(this->A, this->B, this->C);
	}
	Point getInvertNormal() const
	{
		return Point(-this->A, -this->B, -this->C);
	}
	void invert() {
		this->A *= -1;
		this->B *= -1;
		this->C *= -1;
	}

	constexpr const size_t* getNodesIds() const
	{ 
		return node;
	}
	 void moveIds(const int& id)
	 {
		for (int i = 0; i < 4; i++)
			this->node[i] += id;
	};
	size_t getNode(const int id) const
	{
		if (id >= 0 && id <= 3)
			return node[id];
		else return -1;
	}
	void update() {
		sort(&node[0], &node[4]);
	};
};


namespace std {
	template <>
	struct hash<Plane>
	{
		size_t operator()(const Plane& plane) const
		{
			auto points = plane.getNodesIds();
			string number_string = to_string(points[0]) + to_string(points[1]) + to_string(points[2]) + to_string(points[3]);
			hash<string> value;
			return value(number_string);
			
		};
	};

	template <>
	struct equal_to <Plane> { // functor for operator==
		constexpr bool operator()(const Plane& _Left, const Plane& _Right) const {
			auto l_points = _Left.getNodesIds();
			auto r_points = _Right.getNodesIds();

			return l_points[0] == r_points[0] &&
				l_points[1] == r_points[1] &&
				l_points[2] == r_points[2] &&
				l_points[3] == r_points[3];
		}
	};

	template <>
	struct hash<Point>
	{
		std::size_t operator()(const Point& k) const
		{
			return k.id;
		}
	};


}

class NVTR {
public:
	//массив глобальных номаров вершин КЭ
	vector<size_t> n;

	Plane planes[6];
	//Глобальный номер КЭ
	int id;
	//Номер материала
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
