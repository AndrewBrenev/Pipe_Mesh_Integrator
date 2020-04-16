#ifndef _INTERSECTION_REMOVER_HPP_
#define _INTERSECTION_REMOVER_HPP_

#include "../meshes/interface.h"

#define OUTTER_SPHERE_RADIUS_COEFFICIENT  1
#define INSCRIBED_SPHERE_RADIUS_COEFFICIENT  0

template <class PointType, class NetType>
class IntersectionRemover {
private:
	TridimensionalMesh<PointType, NetType>& souceMesh;
	TridimensionalMesh<PointType, NetType>& integrableMesh;

	using SphereCollider = pair<PointType, real>;

	bool isCollided = false;


	bool checkBoxCollider( NetType  sourceFE,  NetType  integrableFE) const {
		bool colliderFlag = false;
		int intersectionCount = 0;
		PointType rayDirection(1,0,0);
		PointType tmpNode;
		for (int i = 0; i < NUMBER_OF_NODES_FORMING_ELEMENT && !colliderFlag; i++) {
			intersectionCount = 0;
			tmpNode = integrableMesh.getNode(integrableFE.n[i] - 1);
			for (int j = 0; j < NUMBER_OF_PLANES_FORMING_ELEMENT; j++) {
				auto intersection = checkPlaneAndRayIntersection<PointType, NetType>(souceMesh, tmpNode, rayDirection, sourceFE.planes[j]);
				if (intersection.first) intersectionCount++;
			}
			if (intersectionCount % 2 != 0)
				colliderFlag = true;
		}
		return colliderFlag;
	};

	bool checkSphereCollider(const SphereCollider& inner, const SphereCollider& outter) const {

		real distanceBetweenÑenters = sqrt((inner.first.x - outter.first.x) * (inner.first.x - outter.first.x) +
			(inner.first.y - outter.first.y) * (inner.first.y - outter.first.y) +
			(inner.first.z - outter.first.z) * (inner.first.z - outter.first.z)
		);

		 return (distanceBetweenÑenters > (inner.second + outter.second))? 
			// òî÷íî íå ïåðåñåêàþòñÿ
			false:
			true;
	}
	SphereCollider countFeCenterPoint(const NetType& meshFE, bool sourceMesh) {

		real Ox{ 0 }, Oy{ 0 }, Oz{ 0 };

		PointType A;
		for (int k = 0; k < 8; k++) {
			A = (sourceMesh) ? souceMesh.getNode(meshFE.n[k] - 1)
				: integrableMesh.getNode(meshFE.n[k] - 1);
			Ox += A.x;
			Oy += A.y;
			Oz += A.z;
		}
		Ox /= 8.; Oy /= 8.; Oz /= 8.;
		PointType O{ Ox,Oy,Oz };

		Ox = 0; Oy = 0; Oz = 0;

		PointType TmR;
		Plane curPlane = meshFE.planes[0];

		//Ðàäèóñ âïèñàííîé îêðóæíîñòè
		for (int i = 0; i < 4; i++)
		{
			TmR = (sourceMesh) ? souceMesh.getNode(curPlane.getNode(i) - 1)
				: integrableMesh.getNode(curPlane.getNode(i) - 1);
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

		// Ðàäèóñ îïèñûâàþùåé îêðóæíîñòè
		A = (sourceMesh) ? souceMesh.getNode(meshFE.n[0] - 1)
			: integrableMesh.getNode(meshFE.n[0] - 1);


		real R{ sqrt(
		   (O.x - A.x) * (O.x - A.x) +
		   (O.y - A.y) * (O.y - A.y) +
		   (O.z - A.z) * (O.z - A.z)
	   ) };

		return SphereCollider(O, OUTTER_SPHERE_RADIUS_COEFFICIENT * R + INSCRIBED_SPHERE_RADIUS_COEFFICIENT * r);
	}
public:
	IntersectionRemover(TridimensionalMesh<PointType, NetType>& _souceMesh, TridimensionalMesh<PointType, NetType>& _integrableMesh) : souceMesh(_souceMesh), integrableMesh(_integrableMesh)
	{

	};
	IntersectionRemover() {

	};
	virtual ~IntersectionRemover() {};

	void setSourseMesh(TridimensionalMesh<PointType, NetType>& _souceMesh)
	{
		this->souceMesh = _souceMesh;
	};
	void setIntegrableMesh(TridimensionalMesh<PointType, NetType>& _integrableMesh)
	{
		this->integrableMesh = _integrableMesh;
	};

	bool isMeshesCollided() { return isCollided; }
	void removeOverlappingFE() 
	{		   
		for (int j = 0; j < souceMesh.getElemsSize();) {
			pair<PointType, real> outter = countFeCenterPoint(souceMesh.getElem(j), true);

			auto innerElSize{ integrableMesh.getElemsSize() };
			bool deleteFlag = false;
			int k = 0;
			for (; k < innerElSize && !deleteFlag; k++) {
				pair<PointType, real> inner = countFeCenterPoint(integrableMesh.getElem(k), false);
				deleteFlag = checkSphereCollider(inner, outter) && checkBoxCollider(souceMesh.getElem(j), integrableMesh.getElem(k));
				//deleteFlag = checkSphereCollider(inner, outter);
			}

			if (deleteFlag) {
				isCollided = true;
				souceMesh.deleteElem(j);
			}
			else j++;
		}
		cout << "Done!" << endl;
	};
};

#endif