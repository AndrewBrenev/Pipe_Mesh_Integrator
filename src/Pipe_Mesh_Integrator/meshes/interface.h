#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "../stdafx.h"

template <class A, class B>
class FileFormater;

template <class PointType, class NetType>
class IntersectionRemover;

template <class PointType, class NetType>
class MeshIntegrator;

template <class PointType, class NetType> 
class IMesh {
private:
	friend FileFormater < PointType, NetType>;
	friend IntersectionRemover < PointType, NetType > ;
	friend MeshIntegrator < PointType, NetType > ;
protected:
	std::vector <PointType> coord;
	std::vector <NetType> nvtr;	

	//Кол-во узлов, и конечных элементов в сетке
	size_t nodes_size, el_size;

	bool setNodesSize(const size_t &n) {
		if (coord.size() == n)
		{
			nodes_size = n;
			return true;
		}
		else return false;
	};
	bool setElemsSize(const size_t &n) {
		if (nvtr.size() == n)
		{
			el_size = n;
			return true;
		}
		else return false; };
	
public:
	
	IMesh() 
	{
		nodes_size = 0;
		el_size = 0;
	};
	virtual ~IMesh() 
	{
		coord.clear();
		nvtr.clear();
	};

	size_t getNodesSize() { return nodes_size; };
	size_t getElemsSize() { return el_size; };

	PointType getNode(u_int i) const { return coord[i]; };
	NetType getElem(u_int i) const { return nvtr[i]; };

	void deleteElem(const uint32_t id) {
		if (id < el_size) {
			nvtr.erase(nvtr.begin() + id);
			setElemsSize(nvtr.size());
		}
	else 
			throw runtime_error("Unable to delete elment with id");
	};

	std::vector<PointType> getNodes() const { return coord; };
	std::vector<NetType> getElems() const { return nvtr; }

	void moveMesh(PointType O) {
		for (int i = 0; i < nodes_size; i++) {
			coord[i].x += O.x;
			coord[i].y += O.y;
			coord[i].z += O.z;
			coord[i].id += O.id;
		}
		for (int i = 0; i < el_size; i++) {
			for (int j = 0; j < 8; j++)
				nvtr[i].n[j] += O.id;

			for (int j = 0; j < 6; j++)
				nvtr[i].planes[j].moveIds(O.id);
		}

	}

	virtual void buildNet() = 0;
	virtual unordered_set<Plane> getPlanesFormingMesh() = 0;
};

#endif