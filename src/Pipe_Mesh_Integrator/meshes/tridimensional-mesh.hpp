#ifndef _TRIDIMENSIONAL_MESH_HPP_
#define _TRIDIMENSIONAL_MESH_HPP_

#include "interface.h"

template <class PointType, class NetType>
class TridimensionalMesh : public IMesh<PointType, NetType> {
private:

	json meshParams;
	void buildThreeDimensionalParapipedalMesh() {

		cout << "Building Tridementional Outter Mesh" << endl;

		//Границы области и шаг разбиения
		real X0(meshParams["parameters"]["X"]["min"]), Xn(meshParams["parameters"]["X"]["max"]), Xh(meshParams["parameters"]["X"]["h"]);
		real Y0(meshParams["parameters"]["Y"]["min"]), Yn((meshParams["parameters"]["Y"]["max"])), Yh(meshParams["parameters"]["Y"]["h"]);
		real Z0(meshParams["parameters"]["Z"]["min"]), Zn(meshParams["parameters"]["Z"]["max"]), Zh(meshParams["parameters"]["Z"]["h"]);

		PointType T; NetType N;

		int id = 0;
		int nX = (Xn - X0) / Xh;
		int nY = (Yn - Y0) / Yh;
		int nZ = (Zn - Z0) / Zh;
		int el_layer = (nZ + 1)*(nX + 1);
		int t_el = 0;

		// Движение по OY
		for (int j = 0; j <= nY; j++) {
			// Движени по OZ
			for (int k = 0; k <= nZ; k++) {
				//Движение по OX
				for (int i = 0; i <= nX; i++)
				{
					T.id = id;
					T.x = X0 + i * Xh;
					T.y = Y0 + j * Yh;
					T.z = Z0 + k * Zh;
					TridimensionalMesh<PointType, NetType>::coord.push_back(T);
					id++;

					if (i != nX && j != nY && k != nZ)
					{
						NetType N(
							(k + 1) *(nX + 1) + (i + 1) + j * el_layer + 1,
							k * (nX + 1) + (i + 1) + j * el_layer + 1,
							(k + 1) * (nX + 1) + i + j * el_layer + 1,
							k * (nX + 1) + i + j * el_layer + 1,
							(k + 1) *(nX + 1) + (i + 1) + (j + 1) * el_layer + 1,
							k * (nX + 1) + (i + 1) + (j + 1) * el_layer + 1,
							(k + 1) * (nX + 1) + i + (j + 1) * el_layer + 1,
							k * (nX + 1) + i + (j + 1) * el_layer + 1,
							t_el,
							meshParams["parameters"]["material-id"]
						);
						N.buildPlanes();
						TridimensionalMesh<PointType, NetType>::nvtr.push_back(N);
						t_el++;
					}
				}
			}
		}
		IMesh<PointType, NetType>::setNodesSize(TridimensionalMesh<PointType, NetType>::coord.size());
		IMesh<PointType, NetType>::setElemsSize(TridimensionalMesh<PointType, NetType>::nvtr.size());

		cout << "Done!" << endl;
	}
public:
	TridimensionalMesh(json inputParams) {
		meshParams = inputParams;
	};
	TridimensionalMesh() {};
	virtual ~TridimensionalMesh() {};
	void buildNet() {
		if (meshParams["action"] == "build")
			buildThreeDimensionalParapipedalMesh();
		else
			if (meshParams["action"] == "read") {
				cout << "Reading Outter Mesh from files: " << endl;
				if (meshParams["parameters"]["format"] == "Glass") {
					
					string inftry = meshParams["parameters"]["paths"]["infitry"];
					string nver = meshParams["parameters"]["paths"]["nver"];
					string nvkat = meshParams["parameters"]["paths"]["nvkat"];
					string xyz = meshParams["parameters"]["paths"]["xyz"];
					cout << inftry << endl <<nver << endl <<nvkat << endl <<xyz << endl;
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
};

#endif