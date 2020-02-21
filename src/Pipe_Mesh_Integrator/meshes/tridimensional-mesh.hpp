#pragma once

#include "interface.h"
#include "../formats/reader-creator.hpp"

template <class PointType, class NetType>
class TridimensionalMesh : public IMesh<PointType, NetType> {
protected: 
	json meshParams;
private:
	void build3DParapipedalMesh() {

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
		int el_layer = (nZ + 1) * (nX + 1);
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
							(k + 1) * (nX + 1) + (i + 1) + j * el_layer + 1,
							k * (nX + 1) + (i + 1) + j * el_layer + 1,
							(k + 1) * (nX + 1) + i + j * el_layer + 1,
							k * (nX + 1) + i + j * el_layer + 1,
							(k + 1) * (nX + 1) + (i + 1) + (j + 1) * el_layer + 1,
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
	};
public:
	TridimensionalMesh(json inputParams) {
		meshParams = inputParams;
	};
	TridimensionalMesh() {};
	virtual ~TridimensionalMesh() {};

	unordered_set<Plane> getPlanesFormingMesh() override {

		unordered_set<Plane> meshPlanes;

		for (auto element : TridimensionalMesh<PointType, NetType>::nvtr)
			for (int j = 0; j < 6; ++j) {

				auto nodes = element.planes[j].getNodesIds();
				auto newPlane = calculatePlaneNorm<PointType>(
					TridimensionalMesh<PointType, NetType>::coord[nodes[0] - 1],
					TridimensionalMesh<PointType, NetType>::coord[nodes[1] - 1],
					TridimensionalMesh<PointType, NetType>::coord[nodes[2] - 1]);
				newPlane.setIds(nodes[0] - 1, nodes[1] - 1, nodes[2] - 1, nodes[3] - 1);
				meshPlanes.insert(newPlane);
			}
		return  meshPlanes;
	};
	void buildNet() {
		if (meshParams["action"] == "build")
			build3DParapipedalMesh();
		else
			if (meshParams["action"] == "read")
			{
				ReaderCreator<PointType, NetType> readerCreator;
				auto  meshReader = readerCreator.createFormater(meshParams["parameters"], this);
				meshReader->readMeshFromFiles();
			}
			else
				throw runtime_error("Unknown action for Outer mesh");
	};
};

