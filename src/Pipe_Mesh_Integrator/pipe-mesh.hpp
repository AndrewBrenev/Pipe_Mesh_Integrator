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
	
	std::vector <BendingPart<PointType, NetType>> turns;
	std::vector <StraightPart<PointType, NetType>> straights;


protected:


public:
	PipeMesh(json input_configs) {
		if (input_configs["action"] == "build") {
			json cut = input_configs["parameters"];
			for (int i = 0; i < input_configs["parameters"]["straight"]["count"]; i++) {
				json cut_params = input_configs["parameters"]["straight"]["segments"][i];
				cut_params.insert(cut.begin(), cut.end());
				straights.push_back(StraightPart< PointType, NetType>(cut_params));
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

		for (int i = 0; i < straights.size(); i++)
			straights[i].buildNet();
		for (int i = 0; i < turns.size(); i++)
			turns[i].buildNet();
	};
	
};

#endif
