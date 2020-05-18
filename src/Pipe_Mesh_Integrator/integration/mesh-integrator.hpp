#ifndef _MESH_INTEGRATOR_HPP
#define _MESH_INTEGRATOR_HPP

#include "../meshes/interface.h"
#include "../meshes/tridimensional-mesh.hpp"
#include "intersection-remover.hpp"
#include "../analytic-geometry.hpp"

template <class PointType,class NetType>
class MeshIntegrator{
private:
	using ObjectPlanes = unordered_set<Plane>;
	using ObjectPoints =  unordered_set<PointType>;
	using PlanesFrequency = unordered_map<Plane, uint32_t>;
		
	TridimensionalMesh<PointType, NetType>& souceMesh;
	TridimensionalMesh<PointType, NetType>& integrableMesh;
	TridimensionalMesh<PointType, NetType> combinedMesh;
	IntersectionRemover<PointType, NetType> intersectionRemover{ souceMesh ,integrableMesh };

	vector<Plane> outterTubePlanes;
	vector<ObjectPlanes> joinablePlanes;
	vector<PlanesFrequency> planesFrequency;

	unordered_map<PointType, PointType> bindingNodesMap;
	vector<PointType> terminalNodes;

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

	void camputePlanesFrequencyForMesh(TridimensionalMesh<PointType, NetType>& mesh, PlanesFrequency& frequency) {
	
		auto allMeshElements = mesh.getElems();

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

		addObjectToMesh(souceMesh);
		
		// ---- интегрируемый объект перенумеруем и сохраним
		int start_id = souceMesh.getNodesSize();

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
		integrableMesh.moveMesh(Point(0, 0, 0, start_id));

		
		addObjectToMesh(integrableMesh);

		cout << "Done!" << endl;
	};

	bool findProectionPoint(const PointType& pointFromTheOtherSide, const PointType& direction, PointType& pointOfIntersection) {

		for (auto plane : joinablePlanes[0]) {
			auto checkResult = checkPlaneAndRayIntersection(this->combinedMesh, pointFromTheOtherSide, direction, plane);
			if (checkResult.first) {
				pointOfIntersection = checkResult.second;
				return true;
			}
		}
		return false;
	};

	void bindObjects() {
		double a, b, c, d;
		for (auto plane : joinablePlanes[1])
		{
			PointType vertexDirection = plane.getNormal();
			PointType mapPoint;

			for (int planeNode = 0; planeNode < NUMBER_OF_NODES_FORMING_PLANE; planeNode++)
			{
				PointType vertexPoint = this->combinedMesh.coord[plane.getNode(planeNode) - 1];

				if (this->bindingNodesMap.find(vertexPoint) == this->bindingNodesMap.end() &&
					findProectionPoint(vertexPoint, vertexDirection, mapPoint))
					bindingNodesMap.insert({ vertexPoint ,mapPoint });
			}
		}
	};

	void renumberTerminalNodes() {
		uint32_t start_node_id = this->combinedMesh.coord.size();
		uint32_t t_nodes_count = 0;

		for (auto mapElem = bindingNodesMap.begin() ; mapElem != bindingNodesMap.end(); mapElem++)
			if ( mapElem->second.id == 0) {
				mapElem->second.setId(start_node_id + t_nodes_count);
				terminalNodes.push_back(mapElem->second);
				t_nodes_count++;
			}

		terminalNodes.shrink_to_fit();

		for (auto node : terminalNodes)
			this->combinedMesh.coord.push_back(node);
		this->combinedMesh.setNodesSize(this->combinedMesh.coord.size());
	};

	void buildDockingElements() {
		uint32_t start_elem_id = this->combinedMesh.nvtr.size();

		size_t planeNodes[4];

		for (auto plane : joinablePlanes[1]) {

			planeNodes[0] = plane.getNode(0);
			planeNodes[1] = plane.getNode(1);
			planeNodes[2] = plane.getNode(2);
			planeNodes[3] = plane.getNode(3);
			auto iteratorFirstPlaneNode = bindingNodesMap.find(this->combinedMesh.coord[planeNodes[0] - 1]);
			auto iteratorSecondPlaneNode = bindingNodesMap.find(this->combinedMesh.coord[planeNodes[1] - 1]);
			auto iteratorThirdPlaneNode = bindingNodesMap.find(this->combinedMesh.coord[planeNodes[2] - 1]);
			auto iteratorFourthPlaneNode = bindingNodesMap.find(this->combinedMesh.coord[planeNodes[3] - 1]);

			// 
			if (iteratorFirstPlaneNode != bindingNodesMap.end() &&
				iteratorSecondPlaneNode != bindingNodesMap.end() &&
				iteratorThirdPlaneNode != bindingNodesMap.end() &&
				iteratorFourthPlaneNode != bindingNodesMap.end())
			{
				
				NetType N(
					planeNodes[0], planeNodes[1], planeNodes[2], planeNodes[3],
					iteratorFirstPlaneNode->second.id + 1, iteratorSecondPlaneNode->second.id + 1,
					iteratorThirdPlaneNode->second.id + 1, iteratorFourthPlaneNode->second.id + 1,
					start_elem_id,
					1
				);

				N.buildPlanes();
				this->combinedMesh.nvtr.push_back(N);

				start_elem_id++;
			}
		}
		this->combinedMesh.setElemsSize(this->combinedMesh.nvtr.size());
	};

	void addObjectToMesh( const TridimensionalMesh<PointType, NetType>& outterMesh)
	{
		std::vector<PointType> currentPoints = outterMesh.getNodes();
		std::vector<NetType> currentElems = outterMesh.getElems();
		this->combinedMesh.nvtr.insert(this->combinedMesh.nvtr.end(), currentElems.begin(), currentElems.end());
		this->combinedMesh.coord.insert(this->combinedMesh.coord.end(), currentPoints.begin(), currentPoints.end());
		this->combinedMesh.setNodesSize(this->combinedMesh.coord.size());
		this->combinedMesh.setElemsSize(this->combinedMesh.nvtr.size());
	};

public:
	vector<PointType> getTerminalNodes() const {return terminalNodes; };
	void setSourseMesh(TridimensionalMesh<PointType, NetType>& _souceMesh)
	{
		this->souceMesh = _souceMesh;
	};
	void setIntegrableMesh(TridimensionalMesh<PointType, NetType> & _integrableMesh)
	{
		this->integrableMesh = _integrableMesh;
	};
	ObjectPlanes getTypeOutterPlanes() {
		return joinablePlanes[1];
	}

	TridimensionalMesh<PointType, NetType> integrateMeshes() {

		computePlanesFrequency(planesFrequency);

		intersectionRemover.removeOverlappingFE();


		identifyBoundaryPlanes();

		// Перенумеруем полученные объекты и построим выходную сетку.
		renumberAndCombineMeshes();
		 
		if (intersectionRemover.isMeshesCollided()) {

			// построим отображение внешних точек интегрируемого объекта на уже существующие вершины
			buildObjectsMapping(joinablePlanes[0], joinablePlanes[1], this->bindingNodesMap);

			//Для каждой точки построили отображение на плоскость
			bindObjects();

			// Перенумеровать терминальные узлы
			renumberTerminalNodes();
			// Строимм новые элементы
			buildDockingElements();
		}
		return this->combinedMesh;
	};

	bool meshHasTnodes() {
		return !terminalNodes.empty();
	}
	MeshIntegrator(TridimensionalMesh<PointType, NetType>& _souceMesh, TridimensionalMesh<PointType, NetType>& _integrableMesh) : souceMesh(_souceMesh), integrableMesh(_integrableMesh) {	};
	MeshIntegrator() {	};
	~MeshIntegrator() {};
	};
#endif 