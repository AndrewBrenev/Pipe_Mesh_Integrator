#ifndef _INTERSECTION_REMOVER_HPP_
#define _INTERSECTION_REMOVER_HPP_

#include "../meshes/interface.h"

#define OUTTER_SPHERE_RADIUS_COEFFICIENT  0.3
#define INSCRIBED_SPHERE_RADIUS_COEFFICIENT  0.7

template <class PointType, class NetType>
class IntersectionRemover {
private:
	IMesh<PointType, NetType>* souceMesh;
	IMesh<PointType, NetType>* integrableMesh;

	using SphereCollider = pair<PointType, real>;

	bool checkIntersectionOfElements(const SphereCollider& inner, const SphereCollider& outter) {

		real distanceBetweenСenters = sqrt((inner.first.x - outter.first.x) * (inner.first.x - outter.first.x) +
			(inner.first.y - outter.first.y) * (inner.first.y - outter.first.y) +
			(inner.first.z - outter.first.z) * (inner.first.z - outter.first.z)
		);

		if (distanceBetweenСenters > (inner.second + outter.second))
			// точно не пересекаются
			return false;
		else
			//но это не точно
			// если ==, то нужна доп обработка, но об этом позже.
			return true;
	}
	SphereCollider countFeCenterPoint(const NetType& meshFE, bool sourceMesh) {

		real Ox{ 0 }, Oy{ 0 }, Oz{ 0 };

		PointType A;
		for (int k = 0; k < 8; k++) {
			A = (sourceMesh) ? souceMesh->getNode(meshFE.n[k] - 1)
				: integrableMesh->getNode(meshFE.n[k] - 1);
			Ox += A.x;
			Oy += A.y;
			Oz += A.z;
		}
		Ox /= 8.; Oy /= 8.; Oz /= 8.;
		PointType O{ Ox,Oy,Oz };

		Ox = 0; Oy = 0; Oz = 0;

		PointType TmR;
		Plane curPlane = meshFE.planes[0];

		//Радиус вписанной окружности
		for (int i = 0; i < 4; i++)
		{
			TmR = (sourceMesh) ? souceMesh->getNode(curPlane.getNode(i) - 1)
				: integrableMesh->getNode(curPlane.getNode(i) - 1);
			Ox += TmR.x;
			Oy += TmR.y;
			Oz += TmR.z;
		}
		A.x = Ox / 4.;
		A.y = Oy / 4.;
		A.z = Oz / 4;

		real r{ sqrt(
			(O.x - A.x) * (O.x - A.x) +
			(O.y - A.y) * (O.y - A.y) +
			(O.z - A.z) * (O.z - A.z)
		) };

		// Радиус описывающей окружности
		A = (sourceMesh) ? souceMesh->getNode(meshFE.n[0] - 1)
			: integrableMesh->getNode(meshFE.n[0] - 1);


		real R{ sqrt(
		   (O.x - A.x) * (O.x - A.x) +
		   (O.y - A.y) * (O.y - A.y) +
		   (O.z - A.z) * (O.z - A.z)
	   ) };

		return SphereCollider(O, OUTTER_SPHERE_RADIUS_COEFFICIENT * R + INSCRIBED_SPHERE_RADIUS_COEFFICIENT * r);
	}
public:
	IntersectionRemover( IMesh<PointType, NetType>* _souceMesh, IMesh<PointType, NetType>* _integrableMesh) : souceMesh(_souceMesh), integrableMesh(_integrableMesh)
	{

	};
	IntersectionRemover() {

	};
	virtual ~IntersectionRemover() {
		souceMesh = nullptr;
		integrableMesh = nullptr;
	};

	void setSourseMesh( IMesh<PointType, NetType>* _souceMesh)
	{
		this->souceMesh = _souceMesh;
	};
	void setIntegrableMesh( IMesh<PointType, NetType>* _integrableMesh)
	{
		this->integrableMesh = _integrableMesh;
	};

	void removeOverlappingFE() 
	{		   
		for (int j = 0; j < souceMesh->getElemsSize();) {
			pair<PointType, real> outter = countFeCenterPoint(souceMesh->getElem(j), true);

			auto innerElSize{ integrableMesh->getElemsSize() };
			bool deleteFlag = false;
			int k = 0;
			for (; k < innerElSize && !deleteFlag; k++) {
				pair<PointType, real> inner = countFeCenterPoint(integrableMesh->getElem(k), false);
				deleteFlag = checkIntersectionOfElements(inner, outter);
			}

			if (deleteFlag)
				souceMesh->deleteElem(j);
			else j++;
		}
		cout << "Done!" << endl;
	};
};

#endif