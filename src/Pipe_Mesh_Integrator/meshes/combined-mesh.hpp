#ifndef _COMBINED_MESH_HPP_
#define _COMBINED_MESH_HPP_


#include "interface.h"

#include "tridimensional-mesh.hpp"
#include "pipe-mesh.hpp"

#include "../integration/mesh-integrator.hpp"
#include "../t-matrix.hpp"
#include "../t-matrix-saver.hpp"

template <class PointType, class NetType>
class CombinedMesh : public TridimensionalMesh<PointType, NetType> {
private:

	json outputInfo;
	T_Matrix<PointType, NetType> t_matrix;
	vector<TridimensionalMesh<PointType, NetType>*> objectsMeshes;

	void combineObjects() {
		MeshIntegrator<PointType, NetType> integrator((*objectsMeshes[0]), *objectsMeshes[1]);
		auto integratedMesh = integrator.integrateMeshes();
		addObjectToCombinedMesh(integratedMesh);
		this->locateMeshPlanes();
		if (integrator.meshHasTnodes())
			processTerminalNodes(integrator);
	}

	void processTerminalNodes(const MeshIntegrator<PointType, NetType>& integrator) {
		auto tNodes = integrator.getTerminalNodes();
		t_matrix.formTerminalMatrix(this,IMesh< PointType, NetType>::getNodesSize() - tNodes.size(),tNodes);
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
	
	void saveMesh() {
		ReaderCreator<PointType, NetType> readerCreator;
		auto  meshReader = readerCreator.createFormater(outputInfo["mesh"], this);
		meshReader->saveMeshToFiles();
	};

	void saveBoundary() {
		FirstBoundariesSaver firstBoundariesSaver(this->firstBoundaryNodes);
		firstBoundariesSaver.saveFirstBoundaryNodesToFile(outputInfo["boundary"]["first"]);
	};

	void saveTmatrix() {
		T_MatrixSaver<PointType, NetType> tMatrixSaver(&t_matrix);
		vector<string> files =
		{ outputInfo["t-matrix"]["paths"]["params"],outputInfo["t-matrix"]["paths"]["ig"],
			outputInfo["t-matrix"]["paths"]["jg"],outputInfo["t-matrix"]["paths"]["gg"] };
		tMatrixSaver.setFilePathes(files);
		tMatrixSaver.saveMatrixToFiles();
	};

	void saveResults()
	{
		cout << "Saving Combined Mesh into files: " << endl;
		saveMesh();
		saveBoundary();
		if (t_matrix.meshHasTnodes())
			saveTmatrix();

		cout << "Done! " << endl;
	}

public:
	CombinedMesh(json input) {

		outputInfo = input["output"];
		int declaredObjectsCount  = input["incoming"]["objects_count"];
		auto actualObjectsCount = input["incoming"]["objects"].size();
		int workObjectsCount = declaredObjectsCount;

		// Objects count checks
		if (!actualObjectsCount) throw std::range_error("Empty list of objects!");
		if (declaredObjectsCount > actualObjectsCount) {
			cout <<endl <<"Warning: Declared objects count is grater than actual. Programm will work with actual count? but it might lead to incorrect resault!" << endl<<endl;
			workObjectsCount = actualObjectsCount;
		}
		if ( actualObjectsCount > declaredObjectsCount ) 
			cout << endl<<"Warning: Actual objects count is grater than declared. Programm will work with DECLARED objects count. It might lead to incorrect resault!" << endl;
		
		
		
		objectsMeshes.reserve(workObjectsCount);
		objectsMeshes.resize(workObjectsCount);

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
		for (int i = 0; i < objectsMeshes.size(); i++) {
			objectsMeshes[i]->buildNet();
			objectsMeshes[i]->locateMeshPlanes();
		}
		
		
 		if (objectsMeshes.size() > 1) 
			combineObjects();
		else
			addObjectToCombinedMesh(*(objectsMeshes[0]));
		
		this->getFirstBoundaryNodes();
		
		saveResults(); 
	};
};
#endif