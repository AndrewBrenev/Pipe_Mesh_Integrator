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


	vector<TridimensionalMesh<PointType, NetType>*> objectsMeshes;

	Plane calculanePlanenorm(const Plane& plane,const vector<PointType> points) {
		

		// Вычислим плоскость, в которой происходит поворот
		real Nx, Ny, Nz, D;
		real a21, a22, a23;
		real a31, a32, a33;

	//	a21 = BendingPart::begin.x - rotationPoint.x; a22 = BendingPart::begin.y - rotationPoint.y; a23 = BendingPart::begin.z - rotationPoint.z;
	//	a31 = BendingPart::end.x - rotationPoint.x; a32 = BendingPart::end.y - rotationPoint.y; a33 = BendingPart::end.z - rotationPoint.z;

		// Вектор нормали к плосткости, в которой происходит поворот
		Nx = a22 * a33 - a32 * a23;
		Ny = a23 * a31 - a21 * a33;
		Nz = a21 * a32 - a22 * a31;
		D = -rotationPoint.x * Nx - rotationPoint.y * Ny - rotationPoint.z * Nz;

		Plane norm(Nx, Ny, Nz, D);

		return norm;
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
		 A =  objectsMeshes[objId]->getNode(meshFE.n[0] - 1);
		
		 real R{ sqrt(
			(O.x - A.x) * (O.x - A.x) +
			(O.y - A.y) * (O.y - A.y) +
			(O.z - A.z) * (O.z - A.z)
		) };

		
		
		return pair<PointType, real>(O, 0.6*R + 0.4*r);
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

	//Процедура формирования ассоциативного массива, формирующая отношение частоты встречаемости плоскости в КЭ
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
	void buildCombinedMesh() {
		if (objectsMeshes.size())
		{
			cout << "-- renumbering objects meshes" ;

			//Полностью сохраним первый объект
			std::vector<PointType> currentPoints = objectsMeshes[0]->getNodes();
			std::vector<NetType> currentElems = objectsMeshes[0]->getElems();
			CombinedMesh::nvtr.insert(CombinedMesh::nvtr.end(), currentElems.begin(), currentElems.end());
			CombinedMesh::coord.insert(CombinedMesh::coord.end(), currentPoints.begin(), currentPoints.end());

			int start_id;
			for (int i = 1; i < objectsMeshes.size(); i++) {
				currentPoints.clear();
				currentElems.clear();
				start_id = CombinedMesh::coord.size();
				PointType bias(0, 0, 0, start_id);

				objectsMeshes[i]->moveMesh(bias);
				std::vector<PointType> currentPoints = objectsMeshes[i]->getNodes();
				std::vector<NetType> currentElems = objectsMeshes[i]->getElems();
				delete objectsMeshes[i];
				CombinedMesh::nvtr.insert(CombinedMesh::nvtr.end(), currentElems.begin(), currentElems.end());
				CombinedMesh::coord.insert(CombinedMesh::coord.end(), currentPoints.begin(), currentPoints.end());

			}
			CombinedMesh::setNodesSize(CombinedMesh::coord.size());
			CombinedMesh::setElemsSize(CombinedMesh::nvtr.size());
			cout << "Done!" << endl;

		}
	};

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

			//computePlanesFrequency(planesFrequency);
			deleteOverlappingFE();

			//identifyBoundaryPlanes();
		}

		// Перенумеруем полученные объекты и построим выходную сетку.
		buildCombinedMesh();

		saveResultMesh();
	};
};
#endif
