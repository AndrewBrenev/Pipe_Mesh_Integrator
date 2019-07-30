#ifndef _PIPE_MESH_HPP_
#define _PIPE_MESH_HPP_



#include "interface.h"
#include "vect.hpp"
#include "bending-part.hpp"
#include "straight-part.hpp"


typedef Circle SectionType;

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

protected:


public:
	PipeMesh(json input_configs) {
		if (input_configs["action"] == "build") {

			json cut = input_configs["parameters"];

		
			for (int i = 0; i < input_configs["parameters"]["straight"]["count"]; i++) {

				json straight_part_params = input_configs["parameters"]["straight"]["segments"][i];
				straight_part_params["cut"] = input_configs["parameters"]["cut"];
				
				tube_parts.push_back(new StraightPart< PointType, NetType>(straight_part_params));
			}


			for (int i = 0; i < input_configs["parameters"]["turns"]["count"]; i++) {
				json turn_parameters;

				turn_parameters = input_configs["parameters"]["turns"]["segments"][i];
				turn_parameters["cut"] = input_configs["parameters"]["cut"];

				int begin_id = input_configs["parameters"]["turns"]["segments"][i]["between"]["start"] - 1;
				int end_id = input_configs["parameters"]["turns"]["segments"][i]["between"]["end"] - 1;
				if (begin_id < 0 || end_id < 0) throw runtime_error("Error when calculatting a turn : the numbering of straight sections starts from 1");
				if (begin_id >= input_configs["parameters"]["straight"]["count"] ||
					end_id >= input_configs["parameters"]["straight"]["count"]) throw runtime_error("Error when calculatting a turn : the numbering of straight sections can not be more than the number of straight sections");

				turn_parameters["begin"] = input_configs["parameters"]["straight"]["segments"][begin_id];
				turn_parameters["end"] = input_configs["parameters"]["straight"]["segments"][end_id];

				tube_parts.push_back(new BendingPart< PointType, NetType>(turn_parameters));
			}

		}
		else
			if (input_configs["action"] == "read") {
				cout << "Reading Tube Mesh from files: " << endl;
				if (input_configs["parameters"]["format"] == "Glass") {

					string inftry = input_configs["parameters"]["paths"]["infitry"];
					string nver = input_configs["parameters"]["paths"]["nver"];
					string nvkat = input_configs["parameters"]["paths"]["nvkat"];
					string xyz = input_configs["parameters"]["paths"]["xyz"];
					cout << inftry << endl << nver << endl << nvkat << endl << xyz << endl;
					const char *input_infitry = inftry.c_str();
					const char *input_nver = nver.c_str();
					const char *input_nvkat = nvkat.c_str();
					const char *input_xyz = xyz.c_str();

					if (!IMesh<PointType, NetType>::readMeshInGlassFormatFromFiles(input_infitry, input_nvkat, input_xyz, input_nver))
						throw runtime_error("Error while reading files: " + inftry + " ," + nvkat + " ," + xyz + " ," + nver);
					cout << "Done!" << endl;
				}
				else
					throw runtime_error("Unknown input file format in \"config.json\"");
			}
			else
				throw runtime_error("Unknown action for Outer mesh");
	};
	~PipeMesh() {};
	void buildNet() {


		omp_set_num_threads(4);
		#pragma omp parallel for
		for (int i = 0; i < tube_parts.size(); i++)
			tube_parts[i]->buildNet();

		dockingPipeParts();

		//Debug only!
		//IMesh<PointType, NetType>::writeMeshInGlassFormatIntoFiles("../../external-libs/glass/test/inftry.dat",
		//	"../../external-libs/glass/test/nvkat.dat",
		//			"../../external-libs/glass/test/xyz.dat", "../../external-libs/glass/test/nver.dat");
	};
	
};

#endif
