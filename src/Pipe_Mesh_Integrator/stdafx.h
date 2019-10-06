// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

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

#define IRON		1
#define OIL			2
#define GROUND		3
#define AIR			4

using namespace std;
using json = nlohmann::json;


typedef double real;

struct Point {
	//координаты, глобальный номер вершины
	real x, y, z;
	int id;
	//конструктор
	Point() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
		id = 0;
	};
	Point(real p1, real p2, real p3, int k) {
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
	void setValue(real p1, real p2, real p3) {
		x = p1;
		y = p2;
		z = p3;
	};
};

class NVTR {
public:
	//массив глобальных номаров вершин  Ё
	std::vector<int> n;
	//√лобальный номер  Ё
	int id;
	//Ќомер материала
	int material;
	NVTR() {
		n.resize(8);
		n[0] = 0; n[1] = 0; n[2] = 0; n[3] = 0;
		n[4] = 0; n[5] = 0; n[6] = 0; n[7] = 0;
		id = 0; material = 0;
	};
	NVTR(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int i, int ma) {
		n.resize(8);
		n[0] = p0; n[1] = p1; n[2] = p2; n[3] = p3;
		n[4] = p4; n[5] = p5; n[6] = p6; n[7] = p7;
		id = i; material = ma;
	};
};

struct Plane {
	//координаты, глобальный номер вершины
	real A, B, C, D;
	//конструктор
	Plane() {
		A = 0.0;
		B = 0.0;
		C = 0.0;
		D = 0;
	};
	Plane(real a, real b, real c, real d) {
		A = a;
		B = b;
		C = c;
		D = d;
	};

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
public:
	int material;
	int n[4];
	NVTR_2D() {
		n[0] = 0;
		n[1] = 0;
		n[2] = 0;
		n[3] = 0;
		material = 0;
	};
	NVTR_2D(const int a,const int b,const int c,const int d,const int m) {
		n[0] = a;
		n[1] = b;
		n[2] = c;
		n[3] = d;
		material = m;
	};
};
