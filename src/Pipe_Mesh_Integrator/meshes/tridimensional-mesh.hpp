#pragma once

#include "interface.h"
#include "../formats/reader-creator.hpp"
#define EMPTY_ELEMENT_VALUE  18446744073709551615

template <class PointType, class NetType>
class TridimensionalMesh : public IMesh<PointType, NetType> {
protected: 
	unordered_set<Plane> meshPlanes;
	unordered_set<Edge> meshEdges;
	vector<size_t> firstBoundaryNodes;

	json meshParams;
private:

	template <typename T ,typename ContainerT> 
	void insertValueIntoContainer(T& value, ContainerT& container)
	{
		auto elementIterator = container.find(value);
		if (elementIterator == container.end())
			container.insert(value);
	};

	void locateBoundaryNodes() {
		vector<size_t> nodesFrequency(TridimensionalMesh<PointType, NetType>::coord.size());
		firstBoundaryNodes.resize(TridimensionalMesh<PointType, NetType>::coord.size(), EMPTY_ELEMENT_VALUE);
		
		for (auto element : TridimensionalMesh<PointType, NetType>::nvtr)
			for (int j = 0; j < 8; ++j)
				nodesFrequency[element.n[j] - 1] += 1;

		for (size_t i = 0, j = 0; i < nodesFrequency.size(); ++i)
			if (nodesFrequency[i] <= 4) {
				firstBoundaryNodes[j] = i;
				j++;
			}

		for (size_t i = 0, j = 0; i < firstBoundaryNodes.size();)
			if (firstBoundaryNodes[i] != EMPTY_ELEMENT_VALUE)
				++i;
			else
				firstBoundaryNodes.erase(firstBoundaryNodes.begin() + i);
			

		firstBoundaryNodes.shrink_to_fit();
	};
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
							(k + 1) * (nX + 1) + i + j * el_layer + 1,
							k * (nX + 1) + (i + 1) + j * el_layer + 1,
							k * (nX + 1) + i + j * el_layer + 1,
							(k + 1) * (nX + 1) + (i + 1) + (j + 1) * el_layer + 1,
							(k + 1) * (nX + 1) + i + (j + 1) * el_layer + 1,
							k * (nX + 1) + (i + 1) + (j + 1) * el_layer + 1,
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
	void locateMeshEdges() {
		for (auto plane : meshPlanes) {

			Edge firstEdge(plane.getNode(0), plane.getNode(1)), secondEdge(plane.getNode(0), plane.getNode(2)),
				thirdEdge(plane.getNode(1), plane.getNode(3)), fourthEdge(plane.getNode(2), plane.getNode(3));

			insertValueIntoContainer(firstEdge, meshEdges);
			insertValueIntoContainer(secondEdge, meshEdges);
			insertValueIntoContainer(thirdEdge, meshEdges);
			insertValueIntoContainer(fourthEdge, meshEdges);

		}
	};
public:
	TridimensionalMesh(json inputParams) {
		meshParams = inputParams;
	};
	TridimensionalMesh() {};
	virtual ~TridimensionalMesh() {};

	void locateMeshPlanes() {
		for (int i = 0; i < this->getElemsSize(); i++) {
			this->nvtr[i].calculatePlanesNormals <PointType>(*this);
			for (int j = 0; j < NUMBER_OF_PLANES_FORMING_ELEMENT; ++j)
				meshPlanes.insert(this->nvtr[i].planes[j]);
		}
	};

	virtual unordered_set<Plane> getPlanesFormingMesh() override {
		if (!meshPlanes.size())
			this->locateMeshPlanes();
		return meshPlanes;
	};

	unordered_set<Edge> getMeshEdges() {
		if (meshEdges.empty())
			locateMeshEdges();
		return meshEdges;
	}
	vector<size_t> getFirstBoundaryNodes()
	{
		if (!firstBoundaryNodes.size())
			locateBoundaryNodes();

		return firstBoundaryNodes;
	}

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

