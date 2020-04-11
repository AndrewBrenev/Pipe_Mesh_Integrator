#pragma once

#include "stdafx.h"
#include "meshes/tridimensional-mesh.hpp"
#include "analytic-geometry.hpp"

template <class PointType, class NetType>
class TridimensionalMesh;

template<typename PointType,typename NetType>
class T_Matrix {
private:
	size_t nodesSize;
	vector<T_Point> terminalMatrix;
	vector<T_Point> unedgedNodes;

TridimensionalMesh<PointType, NetType>* meshToOperate;
	
	void setMesh(TridimensionalMesh<PointType, NetType>* _meshToOperate) {
		meshToOperate = _meshToOperate;
	};
	void setMeshSize(size_t _nodesSize) { nodesSize = _nodesSize; };
	void setTerminalNodes(const vector<PointType>& tNodes) {
		terminalMatrix.resize(tNodes.size());
		int i = 0;
		for (auto tNode : tNodes)
		{
			terminalMatrix[i].id = tNode.id;
			++i;
		}
	};

	pair<double, double> calculateTvalue(const Edge edge,const PointType& tPoint) const {
		PointType A = meshToOperate->getNode(edge.getFirst() - 1);
		PointType B = meshToOperate->getNode(edge.getSecond() - 1);

		double a_value = A.calculateDistanceToPoint(tPoint) / A.calculateDistanceToPoint(A);
		double b_value = B.calculateDistanceToPoint(tPoint) / A.calculateDistanceToPoint(A);
		return make_pair(a_value, b_value);
	}

	void createTerminaledges() {
	};
	void findEdgeContainingTnodes() {
		auto meshEdges = meshToOperate->getMeshEdges();
		for (auto tNode : terminalMatrix)
		{
			PointType tPoint = meshToOperate->getNode(tNode.id);
			bool edgeFound = false;
			for (auto edge = meshEdges.begin(); edge != meshEdges.end() && !edgeFound; edge++)
				if (checkPointForBelongingToEdge<PointType, NetType>(*meshToOperate, *edge, tPoint)) {
					auto distances = calculateTvalue(*edge, tPoint);
					tNode.terminalRows.push_back(make_pair(edge->getFirst() - 1, distances.first));
					tNode.terminalRows.push_back(make_pair(edge->getSecond() - 1, distances.second));
					edgeFound = true;
				}



			if (!edgeFound)
				unedgedNodes.push_back(tNode);
		}

		if (unedgedNodes.size())
			createTerminaledges();

	};


public:
	T_Matrix() {};
	~T_Matrix() {};

	void formTerminalMatrix(TridimensionalMesh<PointType, NetType>* _meshToOperate,
		size_t _nodesSize, const vector<PointType>& tNodes) {
		setMesh(_meshToOperate);
		setMeshSize(_nodesSize);
		setTerminalNodes(tNodes);

		findEdgeContainingTnodes();

	};
	size_t getMeshNodesCount() { return nodesSize; };
	size_t getTerminalNodesCount() { return terminalMatrix.size(); };
	T_Point getColumn(size_t j) {
		if (j < terminalMatrix.size())
			return terminalMatrix[j];
		else
			throw std::out_of_range("There is no such terminal node");
	};

};