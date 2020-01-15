#ifndef _COMBINED_MESH_HPP_
#define _COMBINED_MESH_HPP_


#include "interface.h"

#include "tridimensional-mesh.hpp"
#include "pipe-mesh.hpp"


template <class PointType, class NetType>
class CombinedMesh : public TridimensionalMesh<PointType, NetType> {
private:

	using ObjectPlanes = unordered_set<Plane>;
	using PlanesFrequency = unordered_map<Plane, uint32_t>;

	json outputInfo;

	vector<ObjectPlanes> joinablePlanes;
	vector<PlanesFrequency> planesFrequency;

	unordered_map<PointType, PointType> terminalNodes;

	vector<TridimensionalMesh<PointType, NetType>*> objectsMeshes;

	void inserPointMapping(const PointType& fPnt, const PointType& scndPnt,  unordered_map<PointType, PointType>& mapCont) {
		mapCont.insert({ fPnt ,scndPnt });
	}

	Plane calculatePlaneNorm(const PointType& A, const PointType& B, const PointType& C) {

		// Вычислим плоскость, в которой происходит поворот
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
	}

	//функция возвращает центр КЭ и радиус описывающей окружности
	pair<PointType, real> countFeCenterPoint(const NetType& meshFE, const int& objId) {
		real Ox{ 0 }, Oy{ 0 }, Oz{ 0 };

		PointType A;
		for (int k = 0; k < 8; k++) {
			A = objectsMeshes[objId]->getNode(meshFE.n[k] - 1);
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
			TmR = objectsMeshes[objId]->getNode(curPlane.getNode(i) - 1);
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
		A = objectsMeshes[objId]->getNode(meshFE.n[0] - 1);

		real R{ sqrt(
		   (O.x - A.x) * (O.x - A.x) +
		   (O.y - A.y) * (O.y - A.y) +
		   (O.z - A.z) * (O.z - A.z)
	   ) };



		return pair<PointType, real>(O, 0.3 * R + 0.7 * r);
	}

	bool analyzeFE(const pair<PointType, real>& inner, const pair<PointType, real>& outter) {

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

	//Процедура отображения 
	bool buildObjectsMapping(const ObjectPlanes& mappingSource,const ObjectPlanes& mappingReciver, unordered_map<PointType, PointType>& pointsMap)
	{
		unordered_set<PointType> mappedSourcePoints;
		//Для каждой внешней плоскости
		for (auto outterPlane : mappingSource)
			for (int i = 0; i < 4; ++i)
			{

				PointType currentSourcePoint = CombinedMesh::coord[outterPlane.getNode(i) - 1];

				auto iteratorSourcePointPlaneNode = mappedSourcePoints.find(currentSourcePoint);

				if (iteratorSourcePointPlaneNode == mappedSourcePoints.end())
				{
					PointType currentReciverPoint;
					double minDistance = DBL_MAX;
					double currentPointsDistanse;
					PointType closestPoint;
					for (auto reciverPlane : mappingReciver)
						for (int j = 0; j < 4; ++j)
						{
							currentReciverPoint = CombinedMesh::coord[reciverPlane.getNode(j) - 1];
							currentPointsDistanse = currentSourcePoint.calculateDistanceToPoint(currentReciverPoint);
							if (currentPointsDistanse < minDistance)
							{
								minDistance = currentPointsDistanse;
								closestPoint = currentReciverPoint;
							}
						}
					inserPointMapping(closestPoint, currentSourcePoint, pointsMap);
					mappedSourcePoints.insert(currentSourcePoint);
				}
			}
		
		return true;
	}

	void identifyBoundaryPlanes() {

		joinablePlanes.resize(objectsMeshes.size());
		joinablePlanes.reserve(objectsMeshes.size());

		vector<PlanesFrequency> new_planesFrequency;

		computePlanesFrequency(new_planesFrequency);


		for (int i = 0; i < objectsMeshes.size(); i++) {
			for (auto planeFr : planesFrequency[i])
			{

				if (new_planesFrequency[i].find(planeFr.first) != new_planesFrequency[i].end() &&
					planeFr.second != new_planesFrequency[i].at(planeFr.first))
					joinablePlanes[i].insert(planeFr.first);
			}

			//Если не удаляли элементы, то записываем внешние плоскости
			if (!joinablePlanes[i].size()) {
				for (auto planeFr : planesFrequency[i])
				{
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

		frequency.reserve(objectsMeshes.size());
		frequency.resize(objectsMeshes.size());

		for (int i = 0; i < objectsMeshes.size(); i++) {
			auto currentElems = objectsMeshes[i]->getElems();
			for (auto element : currentElems) {
				for (int j = 0; j < 6; j++) {
					auto iteratorSearchPlane = frequency[i].find(element.planes[j]);
					if (iteratorSearchPlane == frequency[i].end()) {

						frequency[i].insert({ element.planes[j],1 });
					}
					else
						iteratorSearchPlane->second += 1;
				}
			}
		}

	}

	//Процедура удаления пересекающихся КЭ в  объектах
	void deleteOverlappingFE() {

		cout << "--delete overlapping FE...";

		for (int i = 0; i < objectsMeshes.size() - 1; i++) {

			for (int j = 0; j < objectsMeshes[i]->getElemsSize();) {
				pair<PointType, real> outter = countFeCenterPoint(objectsMeshes[i]->getElem(j), i);

				auto innerElSize{ objectsMeshes[i + 1]->getElemsSize() };
				bool deleteFlag = false;
				int k = 0;
				for (; k < innerElSize && !deleteFlag; k++) {
					pair<PointType, real> inner = countFeCenterPoint(objectsMeshes[i + 1]->getElem(k), i + 1);
					deleteFlag = analyzeFE(inner, outter);
				}

				if (deleteFlag)
					objectsMeshes[i]->deleteElem(j);
				else j++;

			}
		}

		cout << "Done!" << endl;
	}

	//перенумерация объектов, находящихся в objectsMeshes
	void renumberObjectsNodes() {
		if (objectsMeshes.size())
		{
			cout << "-- renumbering objects meshes";

			//Полностью сохраним первый объект
			addObjectToCombinedMesh(*(objectsMeshes[0]));

			std::vector<PointType> currentPoints;
			std::vector<NetType> currentElems;
			int start_id;

			for (int i = 1; i < objectsMeshes.size(); ++i)
			{
				currentPoints.clear();
				currentElems.clear();
				start_id = CombinedMesh::coord.size();

				// start planes
				ObjectPlanes new_planes;
				for (auto plane : joinablePlanes[i]) {
					plane.moveIds(start_id);
					new_planes.insert(plane);
				}
				joinablePlanes[i].clear();
				joinablePlanes[i] = new_planes;
				new_planes.clear();
				//finish 

				PointType bias(0, 0, 0, start_id);
				objectsMeshes[i]->moveMesh(bias);

				addObjectToCombinedMesh(*(objectsMeshes[i]));

				delete objectsMeshes[i];
			}

			cout << "Done!" << endl;

		}
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

				PointType planeNodeA = CombinedMesh::coord[plane.getNode(0) - 1];
				PointType planeNodeB = CombinedMesh::coord[plane.getNode(1) - 1];
				PointType planeNodeC = CombinedMesh::coord[plane.getNode(2) - 1];
				PointType planeNodeD = CombinedMesh::coord[plane.getNode(3) - 1];

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
					return pair<bool, PointType> (false, intersectPoint);
			}
		}
		PointType nullPoint;
		return pair<bool, PointType>(false,nullPoint);
	}

	void calculatePlanesNormals()
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
				tmpPlane = calculatePlaneNorm(
					CombinedMesh::coord[a - 1],
					CombinedMesh::coord[b - 1],
					CombinedMesh::coord[c - 1]
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
	}

	void buildNewPointntsMap() {
		double a, b, c, d;
		for (auto plane : joinablePlanes[1])
		{
			PointType vertexDirection = plane.getNormal();
			PointType invertVertexDirection = plane.getInvertNormal();

			PointType otherDirection; PointType mapPoint;

			for (int planeNode = 0; planeNode < 4; planeNode++)
			{
				PointType vertexPoint = CombinedMesh::coord[plane.getNode(planeNode) - 1];

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
	}
	
	void addObjectToCombinedMesh(const TridimensionalMesh<PointType, NetType>& outterMesh)
	{
	
		std::vector<PointType> currentPoints = outterMesh.getNodes();
		std::vector<NetType> currentElems = outterMesh.getElems();
		CombinedMesh::nvtr.insert(CombinedMesh::nvtr.end(), currentElems.begin(), currentElems.end());
		CombinedMesh::coord.insert(CombinedMesh::coord.end(), currentPoints.begin(), currentPoints.end());
		CombinedMesh::setNodesSize(CombinedMesh::coord.size());
		CombinedMesh::setElemsSize(CombinedMesh::nvtr.size());
	}
	
	void buildDockingElements() {
		uint32_t start_node_id = CombinedMesh::coord.size();
		uint32_t start_elem_id = CombinedMesh::nvtr.size();

		size_t planeNodes[4];

		for (auto plane : joinablePlanes[1]) {

			planeNodes[0] = plane.getNode(0);
			planeNodes[1] = plane.getNode(1);
			planeNodes[2] = plane.getNode(2);
			planeNodes[3] = plane.getNode(3);
			auto iteratorFirstPlaneNode = terminalNodes.find(CombinedMesh::coord[planeNodes[0] - 1]);
			auto iteratorSecondPlaneNode = terminalNodes.find(CombinedMesh::coord[planeNodes[1] - 1]);
			auto iteratorThirdPlaneNode = terminalNodes.find(CombinedMesh::coord[planeNodes[2] - 1]);
			auto iteratorFourthPlaneNode = terminalNodes.find(CombinedMesh::coord[planeNodes[3] - 1]);
			
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

				CombinedMesh::coord.push_back(iteratorFirstPlaneNode->second);
				CombinedMesh::coord.push_back(iteratorSecondPlaneNode->second);
				CombinedMesh::coord.push_back(iteratorThirdPlaneNode->second);
				CombinedMesh::coord.push_back(iteratorFourthPlaneNode->second);

				NetType N(
					planeNodes[0], planeNodes[1], planeNodes[2], planeNodes[3],
					start_node_id + 1, start_node_id + 2, start_node_id + 3, start_node_id + 4,
					start_elem_id,
					1
				);

				N.buildPlanes();
				CombinedMesh::nvtr.push_back(N);

				start_elem_id++;
				start_node_id += 4;
			}
		}
		IMesh<PointType, NetType>::setNodesSize(CombinedMesh::coord.size());
		IMesh<PointType, NetType>::setElemsSize(CombinedMesh::nvtr.size());
	}

	//Подпрограмма вывода результата
	void saveResultMesh() {

		cout << "Saving Combined Mesh into files: " << endl;

		ReaderCreator<PointType, NetType> readerCreator;
		auto  meshReader = readerCreator.createFormater(outputInfo["mesh"], this);
		meshReader->saveMeshToFiles();

		cout << "Done! " << endl;
	}
public:
	CombinedMesh(json input) {

		outputInfo = input["output"];

		objectsMeshes.reserve(input["incoming"]["objects_count"]);
		objectsMeshes.resize(input["incoming"]["objects_count"]);

		for (int i = 0; i < objectsMeshes.size(); i++) {
			string current_object_type = input["incoming"]["objects"][i]["type"];
			if (current_object_type == "Tube")
				objectsMeshes[i] = new PipeMesh< PointType, NetType>(input["incoming"]["objects"][i]);
			if (current_object_type == "Outer")
				objectsMeshes[i] = new TridimensionalMesh<PointType, NetType>(input["incoming"]["objects"][i]);
		}

	};
	~CombinedMesh() {};

	void buildNet() {

		cout << "Building the combined mesh: " << endl;

#pragma omp parallel for
		for (int i = 0; i < objectsMeshes.size(); i++)
			objectsMeshes[i]->buildNet();

		if (objectsMeshes.size() > 1) {

			computePlanesFrequency(planesFrequency);
			deleteOverlappingFE();
			identifyBoundaryPlanes();
			// Перенумеруем полученные объекты и построим выходную сетку.
			renumberObjectsNodes();

			//вычислим нормаль для каждой плоскости
			calculatePlanesNormals();

			buildObjectsMapping(joinablePlanes[0], joinablePlanes[1], this->terminalNodes);

			//Для каждой точки построили отображение на плоскость
			buildNewPointntsMap();

			// Строимм новые элементы
			buildDockingElements();
		}
		else
			addObjectToCombinedMesh(*(objectsMeshes[0]));
		

		saveResultMesh();
	};
};
#endif
