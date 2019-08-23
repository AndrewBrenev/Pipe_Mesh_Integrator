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

	void deleteOverlappingFE() {
		//Процедура удаления пересекающихся КЭ в  объектах
	}

	//перенумерация объектов, находящихся в objectsMeshes
	void buildCombinedMesh() {
		if (objectsMeshes.size())
		{
			cout << "Renumbering objects meshes, while building the resulting combined mesh..." << endl;
			int coord_size(0), mesh_size(0);
			for (int i = 0; i < objectsMeshes.size(); i++) {
				mesh_size += objectsMeshes[i]->getElemsSize();
				coord_size += objectsMeshes[i]->getNodesSize();
			}
			CombinedMesh::coord.resize(coord_size);
			CombinedMesh::nvtr.resize(mesh_size);

			//Полностью сохраним первый объект
			for (int i = 0; i < objectsMeshes[0]->getElemsSize(); i++) CombinedMesh::nvtr[i] = objectsMeshes[0]->getElem(i);
			for (int i = 0; i < objectsMeshes[0]->getNodesSize(); i++) CombinedMesh::coord[i] = objectsMeshes[0]->getNode(i);

			int start_id = objectsMeshes[0]->getNodesSize();
			int start_elem = objectsMeshes[0]->getElemsSize();
			for (int i = 1; i < objectsMeshes.size(); i++) {
				
				vector<NetType> curMesh;
				for (int j = 0; j < objectsMeshes[i]->getElemsSize(); j++) curMesh.push_back(objectsMeshes[i]->getElem(j));
			
				for (int k = objectsMeshes[i]->getNodesSize() -1; k >= 0; k--) {
				
					PointType cur = objectsMeshes[i]->getNode(k);
					cur.id = k + start_id;
					CombinedMesh::coord[k + start_id] = cur;

					for (int j = 0; j < objectsMeshes[i]->getElemsSize(); j++) {
						for (int l = 0; l < 8; l++) {
							if (curMesh[j].n[l] == k + 1) curMesh[j].n[l] = cur.id + 1;
						}
					}
				}
				//Добавим элементы
				for (int k = 0; k < objectsMeshes[i]->getElemsSize(); k++) CombinedMesh::nvtr[k + start_elem] = curMesh[k];

				start_elem += objectsMeshes[i]->getElemsSize();
				start_id += objectsMeshes[i]->getNodesSize();

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
