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
			cout << "Renumbering objects meshes, while building the resulting combined mesh..." << endl;
			int coord_size(0), mesh_size(0);
			for (int i = 0; i < tube_parts.size(); i++) {
				mesh_size += tube_parts[i]->getElemsSize();
				coord_size += tube_parts[i]->getNodesSize();
			}
			 PipeMesh::coord.resize(coord_size);
			 PipeMesh::nvtr.resize(mesh_size);

			//Полностью сохраним первый объект
			for (int i = 0; i < tube_parts[0]->getElemsSize(); i++)  PipeMesh::nvtr[i] = tube_parts[0]->getElem(i);
			for (int i = 0; i < tube_parts[0]->getNodesSize(); i++)  PipeMesh::coord[i] = tube_parts[0]->getNode(i);

			int start_id = tube_parts[0]->getNodesSize();
			int start_elem = tube_parts[0]->getElemsSize();
			for (int i = 1; i < tube_parts.size(); i++) {

				vector<NetType> curMesh;
				for (int j = 0; j < tube_parts[i]->getElemsSize(); j++) curMesh.push_back(tube_parts[i]->getElem(j));

				for (int k = tube_parts[i]->getNodesSize() - 1; k >= 0; k--) {

					PointType cur = tube_parts[i]->getNode(k);
					cur.id = k + start_id;
					 PipeMesh::coord[k + start_id] = cur;

					for (int j = 0; j < tube_parts[i]->getElemsSize(); j++) {
						for (int l = 0; l < 8; l++) {
							if (curMesh[j].n[l] == k + 1) curMesh[j].n[l] = cur.id + 1;
						}
					}
				}
				//Добавим элементы
				for (int k = 0; k < tube_parts[i]->getElemsSize(); k++)  PipeMesh::nvtr[k + start_elem] = curMesh[k];

				start_elem += tube_parts[i]->getElemsSize();
				start_id += tube_parts[i]->getNodesSize();

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

		for (int i = 0; i < tube_parts.size(); i++)
			tube_parts[i]->buildNet();
		dockingPipeParts();
		IMesh<PointType, NetType>::writeMeshInGlassFormatIntoFiles("../../Glass/Test/inftry.dat", "../../Glass/Test/nvkat.dat",
					"../../Glass/Test/xyz.dat", "../../Glass/Test/nver.dat");
	};
	
};

#endif
