#ifndef _PIPE_SECTION_HPP_
#define _PIPE_SECTION_HPP_

#include "interface.h"

template <class PointType, class NetType, class SectionType>
class PipeSection : public IMesh<PointType, NetType> {

private:
	// Число разбиений по стороне квадрата, кол-во внутренних окружностей
	int n, l;
	//Нахождение координат трубы в сечении
	void coordTubeOnly(SectionType c) {};
	//Задание конечных элементов на слое
	void nvtrTubeOnly() {};

	bool readFromFiles(const char *path) {
		FILE* file = fopen(path, "r");
		if (file == NULL) return false;

		fscanf(file, "n = %d l = %d\n", &n, &l);

		return true;
	};

public:
	PipeSection() {};
	~PipeSection() {};
	void buildNet() {
		if (readFromFiles("./input-info/input_cut.txt")) {

		}
	};
};

#endif //_PIPE-SECTION_HPP_