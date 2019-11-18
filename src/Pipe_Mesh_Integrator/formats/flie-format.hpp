#ifndef _FILE_FORMAT_HPP_
#define _FILE_FORMAT_HPP_

#include "../meshes/interface.h"


template <class PT, class NT>
class FileFormater {
protected:
	IMesh<PT, NT>* meshToOperate;
	virtual void setElement(const size_t id, NT& value) final {
		meshToOperate->nvtr[id] = value;
	} ;
	virtual void setPoint(const size_t id, PT& value) final {
		meshToOperate->coord[id] = value;
	};
	virtual void setElementsSize(const size_t count) final {
		meshToOperate->setElemsSize(count);
	};
	virtual void setPointsSize(const size_t count) final {
		meshToOperate->setNodesSize(count);
	};
	virtual void pushElement( NT& value) final {
		meshToOperate->nvtr.push_back(value);
	};
	virtual void pushPoint( PT& value) final {
		meshToOperate->coord.push_back(value);

	};
	virtual NT getElement(const size_t id) final {
		return meshToOperate->getElem(id);
	}  ;
	virtual PT getPoint(const size_t id) final {
		return meshToOperate->getNode(id);
	} ;


public:

	FileFormater(IMesh<PT, NT>* mesh) : meshToOperate(mesh) {};
	FileFormater() {};
	
	virtual ~FileFormater() {};

	void  virtual setFilePathes(const vector<string>& ) = 0;
	bool virtual saveMeshToFiles() = 0;
	bool virtual readMeshFromFiles() = 0;

};

#endif //_FILE_FORMAT_HPP_