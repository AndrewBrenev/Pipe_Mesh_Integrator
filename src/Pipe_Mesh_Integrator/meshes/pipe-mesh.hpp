#ifndef _PIPE_MESH_HPP_
#define _PIPE_MESH_HPP_

#include "interface.h"
#include "vect.hpp"
#include "../parts/bending-part.hpp" 
#include "../parts/straight-part.hpp"


template <class PointType, class NetType>
class PipeMesh : public TridimensionalMesh<PointType, NetType> {
private:
	
	std::vector <TubePart<PointType, NetType> *> tube_parts;

	//перенумерация объектов, находящихся в tube_parts
	void dockingPipeParts() {
		if (tube_parts.size())
		{
			cout << "The process of assembling a pipe from its sections has been launched..." << endl;
			
			//Полностью сохраним первый объект
			std::vector<PointType> currentPoints = tube_parts[0]->getNodes();
			std::vector<NetType> currentElems = tube_parts[0]->getElems();
			PipeMesh::nvtr.insert(PipeMesh::nvtr.end(), currentElems.begin(), currentElems.end());
			PipeMesh::coord.insert(PipeMesh::coord.end(), currentPoints.begin(), currentPoints.end());

			int start_id;
			for (int i = 1; i < tube_parts.size(); i++) {
				currentPoints.clear();
				currentElems.clear();
				start_id = PipeMesh::coord.size();
				PointType bias(0, 0, 0, start_id);

				tube_parts[i]->moveMesh(bias);
				std::vector<PointType> currentPoints = tube_parts[i]->getNodes();
				std::vector<NetType> currentElems = tube_parts[i]->getElems();
				delete tube_parts[i];
				PipeMesh::nvtr.insert(PipeMesh::nvtr.end(), currentElems.begin(), currentElems.end());
				PipeMesh::coord.insert(PipeMesh::coord.end(), currentPoints.begin(), currentPoints.end());

			}
			 PipeMesh::setNodesSize( PipeMesh::coord.size());
			 PipeMesh::setElemsSize( PipeMesh::nvtr.size());
			cout << "Done!" << endl;

		}
	};

public:

	PipeMesh(json input_configs) {
		if (input_configs["action"] == "build") {

			json cut = input_configs["parameters"];

		
			for (int i = 0; i < input_configs["parameters"]["straight"]["count"]; i++) {

				json straight_part_params = input_configs["parameters"]["straight"]["segments"][i];
					
				tube_parts.push_back(new StraightPart< PointType, NetType>(straight_part_params));
			}


			for (int i = 0; i < input_configs["parameters"]["turns"]["count"]; i++) {
				json turn_parameters;

				turn_parameters = input_configs["parameters"]["turns"]["segments"][i];
						int begin_id = input_configs["parameters"]["turns"]["segments"][i]["between"]["start"] - 1;
				int end_id = input_configs["parameters"]["turns"]["segments"][i]["between"]["end"] - 1;
				if (begin_id < 0 || end_id < 0)
					throw runtime_error("Error when calculatting a turn : the numbering of straight sections starts from 1");
				if (begin_id >= input_configs["parameters"]["straight"]["count"] ||
					end_id >= input_configs["parameters"]["straight"]["count"]) 
					throw runtime_error("Error when calculatting a turn : the numbering of straight sections can not be more than the number of straight sections");

				turn_parameters["begin"] = input_configs["parameters"]["straight"]["segments"][begin_id];
				turn_parameters["end"] = input_configs["parameters"]["straight"]["segments"][end_id];

				tube_parts.push_back(new BendingPart< PointType, NetType>(turn_parameters));
			}

		}
		else
			if (input_configs["action"] == "read") {
				ReaderCreator<PointType, NetType> readerCreator;
				auto  meshReader = readerCreator.createFormater(TridimensionalMesh<PointType, NetType>::meshParams["parameters"], this);
				meshReader->readMeshFromFiles();
			}
			else
				throw runtime_error("Unknown action for Pipe mesh");
	};
	~PipeMesh() {};
	void buildNet() {

		#pragma omp parallel for
		for (int i = 0; i < tube_parts.size(); i++) {
			tube_parts[i]->buildNet();
		}

		dockingPipeParts();

	};
	
};

#endif
