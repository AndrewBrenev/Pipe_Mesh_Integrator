#ifndef _COMBINED_MESH_HPP_
#define _COMBINED_MESH_HPP_

#include "tridimensional-mesh.hpp"
#include "pipe-mesh.hpp"

template <class PointType, class NetType, class SectionType>
class CombinedMesh : public TridimensionalMesh<PointType, NetType> {
protected:
	TridimensionalMesh<PointType, NetType> incomingNet;
	PipeMesh<PointType, NVTR_2D, SectionType> tube;
	bool readFromFiles() {};
public:
	CombinedMesh() {};
	~CombinedMesh() {};
	void buildNet() {
		incomingNet.buildNet();
		tube.buildNet();
		
	};
};
#endif
