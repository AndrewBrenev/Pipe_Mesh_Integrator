#pragma once
#include "stdafx.h"

template <typename PointType>
Plane calculatePlaneNorm(const PointType& A, const PointType& B, const PointType& C) {

	real Nx, Ny, Nz, D;
	real a21, a22, a23;
	real a31, a32, a33;

	a21 = B.x - C.x; a22 = B.y - C.y; a23 = B.z - C.z;
	a31 = A.x - C.x; a32 = A.y - C.y; a33 = A.z - C.z;

	// Вектор нормали к плосткости, в которой происходит поворот
	Nx = a22 * a33 - a32 * a23;
	Ny = a23 * a31 - a21 * a33;
	Nz = a21 * a32 - a22 * a31;
	double norm = sqrt(Nx * Nx + Ny * Ny + Nz * Nz);
	Nx /= norm; Ny /= norm; Nz /= norm;
	D = -C.x * Nx - C.y * Ny - C.z * Nz;

	Plane normal;
	normal.setNormal(Nx, Ny, Nz, D);

	return normal;
};

bool checPointBelongingToSegment(real p, real a, real b) {
	if (a - b) {
		if (a < b) {
			return (p >= a && p <= b) ? true : false;
		}
		else
			return (p >= b && p <= a) ? true : false;
	}
	else
		return (a == p) ? true : false;
}

template <class PointType,class NetType>
bool checkPointForBelongingToEdge(const TridimensionalMesh<PointType, NetType>& meshToOperate, const Edge edge, PointType x) {

	PointType A = meshToOperate.getNode(edge.getFirst() - 1);
	PointType B = meshToOperate.getNode(edge.getSecond() - 1);

	return  (x != A && x != B) ?
		checPointBelongingToSegment(x.x, A.x, B.x)
		&& checPointBelongingToSegment(x.y, A.y, B.y)
		&& checPointBelongingToSegment(x.z, A.z, B.z) :
		false;
}

template <class PointType>
bool checkPointBelongToPlane(const Plane& plane, const PointType* planeNodes, const PointType& point) {

	// Определим принадлежность через сумму углов
	PointType vectA(planeNodes[0].x - point.x, planeNodes[0].y - point.y, planeNodes[0].z - point.z);
	PointType vectB(planeNodes[1].x - point.x, planeNodes[1].y - point.y, planeNodes[1].z - point.z);
	PointType vectC(planeNodes[2].x - point.x, planeNodes[2].y - point.y, planeNodes[2].z - point.z);
	PointType vectD(planeNodes[3].x - point.x, planeNodes[3].y - point.y, planeNodes[3].z - point.z);

	double cosAB = (vectA.x * vectB.x + vectA.y * vectB.y + vectA.z * vectB.z) / (vectA.length() * vectB.length());
	double cosAC = (vectA.x * vectC.x + vectA.y * vectC.y + vectA.z * vectC.z) / (vectA.length() * vectC.length());
	double cosBD = (vectB.x * vectD.x + vectB.y * vectD.y + vectB.z * vectD.z) / (vectB.length() * vectD.length());
	double cosCD = (vectC.x * vectD.x + vectC.y * vectD.y + vectC.z * vectD.z) / (vectC.length() * vectD.length());

	double phiAB = acos(cosAB);
	double phiAC = acos(cosAC);
	double phiBD = acos(cosBD);
	double phiCD = acos(cosCD);

	double eps = abs(2 * M_PI - (phiAB + phiAC + phiBD + phiCD));

	return (eps < 1e-2) ? true : false;
}

template <class PointType, class NetType>
pair<bool, PointType> checkPlaneAndRayIntersection(const TridimensionalMesh<PointType, NetType>& mesh, const PointType& rayBegin, const PointType& rayDirection, const Plane& plane)
{
	double a, b, c, d;
	plane.getNormal(a, b, c, d);

	double tDenominator = a * rayDirection.x + b * rayDirection.y + c * rayDirection.z;

	if (abs(tDenominator) > 1e-6)
	{
		double tNumerator = rayBegin.x * a + rayBegin.y * b + rayBegin.z * c + d;

		double t = -tNumerator / tDenominator;
		if (t >= 0)
		{

			PointType intersectPoint(
				rayBegin.x + t * rayDirection.x,
				rayBegin.y + t * rayDirection.y,
				rayBegin.z + t * rayDirection.z);

			PointType planeNodes[4];

			planeNodes[0] = mesh.getNode(plane.getNode(0) - 1);
			planeNodes[1] = mesh.getNode(plane.getNode(1) - 1);
			planeNodes[2] = mesh.getNode(plane.getNode(2) - 1);
			planeNodes[3] = mesh.getNode(plane.getNode(3) - 1);

			return  (checkPointBelongToPlane(plane, planeNodes, intersectPoint)) ?
				pair<bool, PointType>(true, intersectPoint) :
				pair<bool, PointType>(false, intersectPoint);
		}
	}
	PointType nullPoint;
	return pair<bool, PointType>(false, nullPoint);
};

template <class PointType>
bool isPlaneAndRayIntersection(const PointType& rayBegin, const PointType& rayDirection, const Plane& plane)
{
	double a, b, c, d;
	plane.getNormal(a, b, c, d);

	double tDenominator = a * rayDirection.x + b * rayDirection.y + c * rayDirection.z;

	if (abs(tDenominator) > 1e-6)
	{
		double tNumerator = rayBegin.x * a + rayBegin.y * b + rayBegin.z * c + d;

		double t = -tNumerator / tDenominator;
		return (t >= 0) ? true : false;
	}
	return false;
}