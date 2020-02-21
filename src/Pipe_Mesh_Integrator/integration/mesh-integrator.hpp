#ifndef _MESH_INTEGRATOR_HPP
#define _MESH_INTEGRATOR_HPP

#include "../meshes/interface.h"
#include "../meshes/tridimensional-mesh.hpp"
#include "intersection-remover.hpp"

#define NUMBER_OF_PROCESSED_OBJECTS 2
#define NUMBER_OF_PLANES_FORMING_ELEMENT 6

template <class PointType,class NetType>
class MeshIntegrator{
private:

	using ObjectPlanes = unordered_set<Plane>;
	using ObjectPoints =  unordered_set<PointType>;
	using PlanesFrequency = unordered_map<Plane, uint32_t>;

	
	IMesh<PointType, NetType>* souceMesh;
	IMesh<PointType, NetType>* integrableMesh;
	TridimensionalMesh<PointType, NetType> combinedMesh;
	IntersectionRemover<PointType, NetType> intersectionRemover{ souceMesh ,integrableMesh };

	void inserPointMapping(const PointType& fPnt, const PointType& scndPnt, unordered_map<PointType, PointType>& mapCont) {
		mapCont.insert({ fPnt ,scndPnt });
	};


	void buildObjectsMapping(const ObjectPlanes& mappingSource, const ObjectPlanes& mappingReciver, unordered_map<PointType, PointType>& pointsMap)
	{
		ObjectPoints sourcePointsSet = selectAllPointsFormingPlane(mappingSource);
		ObjectPoints reciverPointsSet = selectAllPointsFormingPlane(mappingReciver);

		mapPointsSetToAnotherPointsSet(sourcePointsSet, reciverPointsSet, pointsMap);

	};

	ObjectPoints selectAllPointsFormingPlane(const ObjectPlanes& planesOfObject) {
		ObjectPoints objectPoints;
		for (auto outterPlane : planesOfObject)
			for (int i = 0; i < 4; ++i)
			{
				PointType currentPoint = this->combinedMesh.coord[outterPlane.getNode(i) - 1];

				auto iteratorPointInPlaneMap = objectPoints.find(currentPoint);

				if (iteratorPointInPlaneMap == objectPoints.end())
					objectPoints.insert(currentPoint);
			}
		return objectPoints;
	}

	void mapPointsSetToAnotherPointsSet(const ObjectPoints& mappingSource, const ObjectPoints& mappingReciver, unordered_map<PointType, PointType>& pointsMap)
	{
		double minDistance = DBL_MAX;
		double currentPointsDistanse;
		PointType closestPoint;

		for (auto currentSourcePoint : mappingSource)
		{
			minDistance = DBL_MAX;
			for (auto currentReciverPoint : mappingReciver)
			{
				currentPointsDistanse = currentSourcePoint.calculateDistanceToPoint(currentReciverPoint);
				if (currentPointsDistanse < minDistance)
				{
					minDistance = currentPointsDistanse;
					closestPoint = currentReciverPoint;
				}
			}
			inserPointMapping(closestPoint, currentSourcePoint, pointsMap);
		}
	}

	void identifyBoundaryPlanes() {

		joinablePlanes.resize(NUMBER_OF_PROCESSED_OBJECTS);
		joinablePlanes.reserve(NUMBER_OF_PROCESSED_OBJECTS);

		vector<PlanesFrequency> new_planesFrequency;

		computePlanesFrequency(new_planesFrequency);

		for (int i = 0; i < NUMBER_OF_PROCESSED_OBJECTS; i++) {
			for (auto planeFr : planesFrequency[i])	{
				if (new_planesFrequency[i].find(planeFr.first) != new_planesFrequency[i].end() &&
					planeFr.second != new_planesFrequency[i].at(planeFr.first))
					joinablePlanes[i].insert(planeFr.first);
			}

			//Если не удаляли элементы, то записываем внешние плоскости
			if (!joinablePlanes[i].size()) {
				for (auto planeFr : planesFrequency[i])	{
					if (planeFr.second == 1)
						joinablePlanes[i].insert(planeFr.first);
				}
			}
		}

		for (auto fr : planesFrequency)
			fr.clear();
		planesFrequency.clear();
	}

	//Процедура формирования ассоциативного массива, характерезующего отношение частоты встречаемости плоскости в сетке
	// (либо 2, либо 1)
	void computePlanesFrequency(vector<PlanesFrequency>& frequency) {

		frequency.reserve(NUMBER_OF_PROCESSED_OBJECTS);
		frequency.resize(NUMBER_OF_PROCESSED_OBJECTS);

		camputePlanesFrequencyForMesh(souceMesh, frequency[0]);
		camputePlanesFrequencyForMesh(integrableMesh, frequency[1]);

	};

	void camputePlanesFrequencyForMesh(IMesh<PointType, NetType>* mesh, PlanesFrequency& frequency) {
	
		auto allMeshElements = mesh->getElems();

		for (auto element : allMeshElements)
			for (int j = 0; j < NUMBER_OF_PLANES_FORMING_ELEMENT; ++j) {
				auto iteratorElementPlane = frequency.find(element.planes[j]);
				if (iteratorElementPlane == frequency.end())
					frequency.insert({ element.planes[j],1 });
				else
					iteratorElementPlane->second += 1;
			}

	};
	void renumberAndCombineMeshes() {

		cout << "-- renumbering objects meshes";

		addObjectToMesh( *souceMesh);
		
		// ---- интегрируемый объект перенумеруем и сохраним
		int start_id = souceMesh->getNodesSize();

		// Обработаем плоскости
		ObjectPlanes samePlanesWithNewIds;
		for (auto plane : joinablePlanes[1]) {
			plane.moveIds(start_id);
			samePlanesWithNewIds.insert(plane);
		}
		joinablePlanes[1].clear();
		joinablePlanes[1] = samePlanesWithNewIds;
		samePlanesWithNewIds.clear();
		
		//Перенумеруем интегрируемый объект
		integrableMesh->moveMesh(Point(0, 0, 0, start_id));

		
		addObjectToMesh(*integrableMesh);

		cout << "Done!" << endl;
	};

	void calculatePlanesNormals(const IMesh<PointType, NetType>& combinedMesh)
	{
		Plane tmpPlane;
		uint32_t a, b, c, d;

		for (int i = 0; i < joinablePlanes.size(); ++i)
		{
			ObjectPlanes new_planes;

			for (auto plane : joinablePlanes[i])
			{
				a = plane.getNode(0);
				b = plane.getNode(1);
				c = plane.getNode(2);
				d = plane.getNode(3);
				tmpPlane = calculatePlaneNorm<PointType>(
					combinedMesh.coord[a - 1],
					combinedMesh.coord[b - 1],
					combinedMesh.coord[c - 1]
				);

				tmpPlane.setIds(a, b, c, d);
				new_planes.insert(tmpPlane);
			}

			joinablePlanes[i].clear();
			joinablePlanes[i] = new_planes;
			new_planes.clear();
		}
	};

	bool findProectionPoint(const PointType& pointFromTheOtherSide, const PointType& direction, PointType& pointOfIntersection) {

		for (auto plane : joinablePlanes[0]) {
			auto checkResult = checkPlaneAndRayIntersection(pointFromTheOtherSide, direction, plane);
			if (checkResult.first) {
				pointOfIntersection = checkResult.second;
				return true;
			}
		}
		return false;
	};

	pair<bool, PointType> checkPlaneAndRayIntersection(const PointType& rayBegin, const PointType& rayDirection, const Plane& plane)
	{
		double a, b, c, d;

		plane.getNormal(a, b, c, d);

		double tDenominator = a * rayDirection.x + b * rayDirection.y + c * rayDirection.z;

		if (abs(tDenominator) > 1e-6)
		{

			double tNumerator = rayBegin.x * a + rayBegin.y * b + rayBegin.z * c + d;

			double t = -tNumerator / tDenominator;
			if (t > 0)
			{

				PointType intersectPoint(
					rayBegin.x + t * rayDirection.x,
					rayBegin.y + t * rayDirection.y,
					rayBegin.z + t * rayDirection.z);

				PointType planeNodeA = this->combinedMesh.coord[plane.getNode(0) - 1];
				PointType planeNodeB = this->combinedMesh.coord[plane.getNode(1) - 1];
				PointType planeNodeC = this->combinedMesh.coord[plane.getNode(2) - 1];
				PointType planeNodeD = this->combinedMesh.coord[plane.getNode(3) - 1];

				// Определим принадлежность через сумму углов
				PointType vectA(planeNodeA.x - intersectPoint.x, planeNodeA.y - intersectPoint.y, planeNodeA.z - intersectPoint.z);
				PointType vectB(planeNodeB.x - intersectPoint.x, planeNodeB.y - intersectPoint.y, planeNodeB.z - intersectPoint.z);
				PointType vectC(planeNodeC.x - intersectPoint.x, planeNodeC.y - intersectPoint.y, planeNodeC.z - intersectPoint.z);
				PointType vectD(planeNodeD.x - intersectPoint.x, planeNodeD.y - intersectPoint.y, planeNodeD.z - intersectPoint.z);

				double cosAB = (vectA.x * vectB.x + vectA.y * vectB.y + vectA.z * vectB.z) / (vectA.length() * vectB.length());
				double cosAC = (vectA.x * vectC.x + vectA.y * vectC.y + vectA.z * vectC.z) / (vectA.length() * vectC.length());
				double cosBD = (vectB.x * vectD.x + vectB.y * vectD.y + vectB.z * vectD.z) / (vectB.length() * vectD.length());
				double cosCD = (vectC.x * vectD.x + vectC.y * vectD.y + vectC.z * vectD.z) / (vectC.length() * vectD.length());

				double phiAB = acos(cosAB);
				double phiAC = acos(cosAC);
				double phiBD = acos(cosBD);
				double phiCD = acos(cosCD);

				double eps = abs(2 * M_PI - (phiAB + phiAC + phiBD + phiCD));
				if (eps < 1e-2)
				{
					return pair<bool, PointType>(true, intersectPoint);
				}
				else
					return pair<bool, PointType>(false, intersectPoint);
			}
		}
		PointType nullPoint;
		return pair<bool, PointType>(false, nullPoint);
	};

	void buildNewPointsMap() {
		double a, b, c, d;
		for (auto plane : joinablePlanes[1])
		{
			PointType vertexDirection = plane.getNormal();
			PointType invertVertexDirection = plane.getInvertNormal();

			PointType otherDirection; PointType mapPoint;

			for (int planeNode = 0; planeNode < 4; planeNode++)
			{
				PointType vertexPoint = this->combinedMesh.coord[plane.getNode(planeNode) - 1];

				if (this->terminalNodes.find(vertexPoint) == this->terminalNodes.end()) {
					if (findProectionPoint(vertexPoint, vertexDirection, mapPoint))
					{

						if (findProectionPoint(vertexPoint, invertVertexDirection, otherDirection))
						{

							PointType oldDistance(mapPoint.x - vertexPoint.x, mapPoint.y - vertexPoint.y, mapPoint.z - vertexPoint.z);
							PointType newDistance(otherDirection.x - vertexPoint.x, otherDirection.y - vertexPoint.y, otherDirection.z - vertexPoint.z);

							double firstPath = oldDistance.length();
							double secondPath = newDistance.length();
							if (firstPath < secondPath)
								terminalNodes.insert({ vertexPoint ,mapPoint });
							else

								terminalNodes.insert({ vertexPoint ,otherDirection });
						}
						else

							terminalNodes.insert({ vertexPoint ,mapPoint });
					}
					else
						if (findProectionPoint(vertexPoint, invertVertexDirection, mapPoint))
							terminalNodes.insert({ vertexPoint ,mapPoint });

				}
			}
		}
	};

	void buildDockingElements() {
		uint32_t start_node_id = this->combinedMesh.coord.size();
		uint32_t start_elem_id = this->combinedMesh.nvtr.size();

		size_t planeNodes[4];

		for (auto plane : joinablePlanes[1]) {

			planeNodes[0] = plane.getNode(0);
			planeNodes[1] = plane.getNode(1);
			planeNodes[2] = plane.getNode(2);
			planeNodes[3] = plane.getNode(3);
			auto iteratorFirstPlaneNode = terminalNodes.find(this->combinedMesh.coord[planeNodes[0] - 1]);
			auto iteratorSecondPlaneNode = terminalNodes.find(this->combinedMesh.coord[planeNodes[1] - 1]);
			auto iteratorThirdPlaneNode = terminalNodes.find(this->combinedMesh.coord[planeNodes[2] - 1]);
			auto iteratorFourthPlaneNode = terminalNodes.find(this->combinedMesh.coord[planeNodes[3] - 1]);

			// 
			if (iteratorFirstPlaneNode != terminalNodes.end() &&
				iteratorSecondPlaneNode != terminalNodes.end() &&
				iteratorThirdPlaneNode != terminalNodes.end() &&
				iteratorFourthPlaneNode != terminalNodes.end())
			{
				iteratorFirstPlaneNode->second.setId(start_node_id);
				iteratorSecondPlaneNode->second.setId(start_node_id + 1);
				iteratorThirdPlaneNode->second.setId(start_node_id + 2);
				iteratorFourthPlaneNode->second.setId(start_node_id + 3);

				this->combinedMesh.coord.push_back(iteratorFirstPlaneNode->second);
				this->combinedMesh.coord.push_back(iteratorSecondPlaneNode->second);
				this->combinedMesh.coord.push_back(iteratorThirdPlaneNode->second);
				this->combinedMesh.coord.push_back(iteratorFourthPlaneNode->second);

				NetType N(
					planeNodes[0], planeNodes[1], planeNodes[2], planeNodes[3],
					start_node_id + 1, start_node_id + 2, start_node_id + 3, start_node_id + 4,
					start_elem_id,
					1
				);

				N.buildPlanes();
				this->combinedMesh.nvtr.push_back(N);

				start_elem_id++;
				start_node_id += 4;
			}
		}
		this->combinedMesh.setNodesSize(this->combinedMesh.coord.size());
		this->combinedMesh.setElemsSize(this->combinedMesh.nvtr.size());
	};

	void addObjectToMesh( const IMesh<PointType, NetType>& outterMesh)
	{
		std::vector<PointType> currentPoints = outterMesh.getNodes();
		std::vector<NetType> currentElems = outterMesh.getElems();
		this->combinedMesh.nvtr.insert(this->combinedMesh.nvtr.end(), currentElems.begin(), currentElems.end());
		this->combinedMesh.coord.insert(this->combinedMesh.coord.end(), currentPoints.begin(), currentPoints.end());
		this->combinedMesh.setNodesSize(this->combinedMesh.coord.size());
		this->combinedMesh.setElemsSize(this->combinedMesh.nvtr.size());
	};

public:

	vector<ObjectPlanes> joinablePlanes;
	vector<PlanesFrequency> planesFrequency;

	unordered_map<PointType, PointType> terminalNodes;

	void setSourseMesh(IMesh<PointType, NetType> * _souceMesh)
	{
		this->souceMesh = _souceMesh;
	};
	void setIntegrableMesh(IMesh<PointType, NetType> * _integrableMesh)
	{
		this->integrableMesh = _integrableMesh;
	};

	TridimensionalMesh<PointType, NetType> integrateMeshes() {

		computePlanesFrequency(planesFrequency);

		intersectionRemover.removeOverlappingFE();

		identifyBoundaryPlanes();

		// Перенумеруем полученные объекты и построим выходную сетку.
		renumberAndCombineMeshes();

		//вычислим нормаль для каждой плоскости
		calculatePlanesNormals(this->combinedMesh);

		buildObjectsMapping(joinablePlanes[0], joinablePlanes[1], this->terminalNodes);

		//Для каждой точки построили отображение на плоскость
		buildNewPointsMap();

		// Строимм новые элементы
		buildDockingElements();

		return this->combinedMesh;
	};

	MeshIntegrator(IMesh<PointType, NetType> * _souceMesh, IMesh<PointType, NetType> * _integrableMesh) : souceMesh(_souceMesh), integrableMesh(_integrableMesh)
	{

	};
	MeshIntegrator() {	};
	~MeshIntegrator() {};
	};
#endif 