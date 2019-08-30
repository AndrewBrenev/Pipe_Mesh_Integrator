#ifndef _COMBINED_MESH_HPP_
#define _COMBINED_MESH_HPP_


#include "interface.h"
#include "tridimensional-mesh.hpp"
#include "pipe-mesh.hpp"

template <class PointType, class NetType>
class CombinedMesh : public TridimensionalMesh<PointType, NetType> {
private:
	json outputInfo;
	vector<TridimensionalMesh<PointType, NetType> *> objectsMeshes;


	//функция возвращает центр КЭ и радиус описывающей окружности
	pair<PointType, real> countFeCenterPoint(const NetType & meshFE, const int& objId) {
		real Ox{ 0 }, Oy{ 0 }, Oz{ 0 };
		PointType A;
		for (int k = 0; k < 8; k++) {
			A = objectsMeshes[objId]->getNode(meshFE.n[k] - 1);
			Ox += A.x;
			Oy += A.y;
			Oz += A.z;
		}
		Ox /= 8.; Oy /= 8.; Oz /= 8.;

		A = objectsMeshes[objId]->getNode(meshFE.n[0] - 1);
		PointType O{ Ox,Oy,Oz };


		// Радиус описывающей окружности
		real R{ sqrt(
			(O.x - A.x)*(O.x - A.x) +
			(O.y - A.y)*(O.y - A.y) +
			(O.z - A.z)*(O.z - A.z)
		) };

		return pair<PointType, real>(O, R);
	}

	bool analyzeFE(const pair<PointType, real>& inner, const pair<PointType, real>& outter) {
		
		real distanceBetweenСenters = sqrt((inner.first.x - outter.first.x)*(inner.first.x - outter.first.x) +
			(inner.first.y - outter.first.y)*(inner.first.y - outter.first.y) +
			(inner.first.z - outter.first.z)*(inner.first.z - outter.first.z)
		);

		if (distanceBetweenСenters > (inner.second + outter.second))
			// точно не пересекаются
			return false;
		else 
			//но это не точно
			// если ==, то нужна доп обработка, но об этом позже.
		return true;
	}

	//Процедура удаления пересекающихся КЭ в  объектах
	void deleteOverlappingFE() {

		cout << "--delete overlapping FE..." ; 

		for (int i = 0; i < objectsMeshes.size() -1; i++) {

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
			cout << "-- renumbering objects meshes" << endl;

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
	void saveResultMesh(){
		cout << "Saving Combined Mesh into files: " << endl;
		if (outputInfo["mesh"]["format"] == "Glass") {
			string inftry = outputInfo["mesh"]["paths"]["inftry"];
			string nver = outputInfo["mesh"]["paths"]["nver"];
			string nvkat = outputInfo["mesh"]["paths"]["nvkat"];
			string xyz = outputInfo["mesh"]["paths"]["xyz"];

			cout << inftry << endl << nver << endl << nvkat << endl << xyz << endl;
			const char *output_inftry = inftry.c_str();
			const char *output_nver = nver.c_str();
			const char *output_nvkat = nvkat.c_str();
			const char *output_xyz = xyz.c_str();

		
		IMesh<PointType, NetType>::writeMeshInGlassFormatIntoFiles(output_inftry, output_nvkat, output_xyz, output_nver);
		}
		cout << "Done! " << endl;
	}
public:
	CombinedMesh(json input) {
		
		outputInfo = input["output"];

		objectsMeshes.reserve(input["incoming"]["objects_coount"]);
		objectsMeshes.resize(input["incoming"]["objects_coount"]);
	
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

		omp_set_num_threads(4);
		#pragma omp parallel for
		for (int i = 0; i < objectsMeshes.size(); i++) 
			objectsMeshes[i]->buildNet();
		
		deleteOverlappingFE();

		// Перенумеруем полученные объекты и построим выходную сетку.
		buildCombinedMesh();

		saveResultMesh();
	};
};
#endif
