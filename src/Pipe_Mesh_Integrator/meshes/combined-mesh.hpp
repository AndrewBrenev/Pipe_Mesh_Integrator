#ifndef _COMBINED_MESH_HPP_
#define _COMBINED_MESH_HPP_


#include "interface.h"

#include "tridimensional-mesh.hpp"
#include "pipe-mesh.hpp"


#include "../integration/mesh-integrator.hpp"


template <class PointType, class NetType>
class CombinedMesh : public TridimensionalMesh<PointType, NetType> {
private:

	json outputInfo;

	vector<TridimensionalMesh<PointType, NetType>*> objectsMeshes;

	void combineObjects() {
		MeshIntegrator<PointType, NetType> integrator(objectsMeshes[0], objectsMeshes[1]);
		auto integratedMesh = integrator.integrateMeshes();
		addObjectToCombinedMesh(integratedMesh);
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
	~CombinedMesh() {
		for (int i = 0; i < this->objectsMeshes.size(); ++i)
			delete objectsMeshes[i];
		objectsMeshes.clear();
	};

	void buildNet() {

		cout << "Building the combined mesh: " << endl;

#pragma omp parallel for
		for (int i = 0; i < objectsMeshes.size(); i++)
			objectsMeshes[i]->buildNet();

		if (objectsMeshes.size() > 1) 
			combineObjects();
		else
			addObjectToCombinedMesh(*(objectsMeshes[0]));
		
		saveResultMesh();
	};
};
#endif
