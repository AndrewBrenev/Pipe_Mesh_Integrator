#ifndef _PIPE_SECTION_HPP_
#define _PIPE_SECTION_HPP_

#include "interface.h"

template <class PointType, class NetType, class SectionType>
class PipeSection : public IMesh<PointType, NetType> {
protected:
	
	SectionType face;
	// „исло разбиений по стороне квадрата, кол-во внутренних окружностей
	int n, l;

	//колличество точек в одном сечении
	int coor_on_layer;
	//¬ложенность труб
	int insert;

	//«адание конечных элементов на слое
	virtual vector<NetType> nvtrTubeOnly() = 0;
	//Ќахождение координат трубы в сечении
	virtual vector<PointType> coordTubeOnly() = 0;

public:
	
	virtual ~PipeSection() {};
	
	vector<NetType> getSectionNVTR(){
		return PipeSection::nvtr;
	};
	
	void buildNet() {
		try {
				PipeSection::coord = coordTubeOnly();
				PipeSection::nvtr = nvtrTubeOnly();
				PipeSection::setNodesSize(coor_on_layer);
				PipeSection::setElemsSize(PipeSection::nvtr.size());
		}
		catch (exception& e)
		{
			throw e;
			cout << "Failed! Found an exeption: " << e.what() << endl;
		}
		
	};
};

#endif //_PIPE-SECTION_HPP_