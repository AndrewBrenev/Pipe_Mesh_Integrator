// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <iostream>
#include<fstream>
#pragma once
#include <cmath>
#include <math.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <iomanip>




#define IRON		1
#define OIL			2
#define GROUND		3
#define AIR			4

using namespace std;

typedef double real;


struct Point {
	//����������, ���������� ����� �������
	real x, y, z;
	int id;
	//�����������
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
	};
};

class NVTR {
public:
	//������ ���������� ������� ������ ��
	std::vector<int> n;
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
	NVTR(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int i, int ma) {
		n.resize(8);
		n[0] = p0; n[1] = p1; n[2] = p2; n[3] = p3;
		n[4] = p4; n[5] = p5; n[6] = p6; n[7] = p7;
		id = i; material = ma;
	};
};

struct Circle {
	//���������� ������ ����������
	Point O;
	//������� � ���������� �������
	real R, r;
	Circle() {};
	Circle(real x0, real y0, real z0, real R0, real r0) {
		O.x = x0;
		O.y = y0;
		O.z = z0;
		R = R0;
		r = r0;
	};
	Circle(Point Center, real R0, real r0) {
		O = Center;
		R = R0;
		r = r0;
	};
};

struct Plane {
	//����������, ���������� ����� �������
	real A, B, C, D;
	//�����������
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

struct NVTR_2D
{
public:
	int material;
	int id;
	int n[4];
	NVTR_2D() {};
	NVTR_2D(const int a,const int b,const int c,const int d,const int e) {
		n[0] = a;
		n[1] = b;
		n[2] = c;
		n[3] = d;
		material = e;
	};
};