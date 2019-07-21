#ifndef _PIPE_SECTION_HPP_
#define _PIPE_SECTION_HPP_

#include "interface.h"

template <class PointType, class NetType, class SectionType>
class PipeSection : public IMesh<PointType, NetType> {
protected:

	SectionType face;
	// Число разбиений по стороне квадрата, кол-во внутренних окружностей
	int n=6, l = 3;

	int coor_on_layer;
	//Вложенность труб
	int insert;

	//Задание конечных элементов на слое
	virtual vector<NetType> nvtrTubeOnly() = 0;

	bool readFromFiles(const char *path) {
		FILE* file = fopen(path, "r");
		if (file == NULL) return false;

		fscanf(file, "n = %d l = %d inserty = %d\n", &n, &l, &insert);

		return true;
	};

public:
	
	virtual ~PipeSection() {};
	
	void getParam(int &frst, int &sec) { frst = n; sec = l; };
	vector<NetType> getSectionNVTR(){
		return PipeSection::nvtr;
	};
	//Нахождение координат трубы в сечении
	virtual vector<PointType> coordTubeOnly(const int start_id) = 0;
	void buildNet() {
		if (readFromFiles("./input-info/input_cut.txt")) {
				PipeSection::coord = coordTubeOnly(0);
				PipeSection::nvtr = nvtrTubeOnly();
				PipeSection::setNodesSize(coor_on_layer);
				PipeSection::setElemsSize(PipeSection::nvtr.size());
		}
		else throw("Cut Input File Not Found in : ./input-info/input_cut.txt");
	};
};

#endif //_PIPE-SECTION_HPP_