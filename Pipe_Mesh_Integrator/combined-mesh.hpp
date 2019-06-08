#ifndef _COMBINED_MESH_HPP_
#define _COMBINED_MESH_HPP_


#include "interface.h"

#include "tridimensional-mesh.hpp"
#include "pipe-mesh.hpp"

template <class PointType, class NetType, class SectionType>
class CombinedMesh : public TridimensionalMesh<PointType, NetType> {
protected:
	vector<TridimensionalMesh<PointType, NetType> *> objectsMeshes;
	void renumberingMeshes() {

		if (objectsMeshes.size())
		{
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

		}
	};
	bool readFromFiles() {};
public:
	CombinedMesh() {
		

		objectsMeshes.resize(2);
				
		objectsMeshes[0] = new PipeMesh<PointType, NetType, SectionType>;
		objectsMeshes[1] = new TridimensionalMesh<PointType, NetType>;

	};
	~CombinedMesh() {};
	void buildNet() {

		omp_set_num_threads(4);
		#pragma omp parallel for
		for (int i = 0; i < objectsMeshes.size(); i++) 
			objectsMeshes[i]->buildNet();
		//renumberingMeshes();
		IMesh<PointType, NetType>::writeToFile("./Glass/Test/inftry.dat", "./Glass/Test/nvkat.dat",
			"./Glass/Test/xyz.dat", "./Glass/Test//nver.dat");
	};
};
#endif
