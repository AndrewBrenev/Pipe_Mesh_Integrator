#ifndef _PIPE_SECTION_HPP_
#define _PIPE_SECTION_HPP_

#include "interface.h"

template <class PointType, class NetType, class SectionType>
class PipeSection : public IMesh<PointType, NetType> {
protected:
	
	SectionType face;
	// ����� ��������� �� ������� ��������, ���-�� ���������� �����������
	int n, l;

	//����������� ����� � ����� �������
	int coor_on_layer;
	//����������� ����
	int insert;

	//������� �������� ��������� �� ����
	virtual vector<NetType> nvtrTubeOnly() = 0;

	bool readFromFiles(const char *path) {
		FILE* file = fopen(path, "r");
		if (file == NULL) return false;

		fscanf(file, "n = %d l = %d inserty = %d\n", &n, &l, &insert);

		return true;
	};

public:
	
	virtual ~PipeSection() {};
	
	vector<NetType> getSectionNVTR(){
		return PipeSection::nvtr;
	};
	//���������� ��������� ����� � �������
	virtual vector<PointType> coordTubeOnly(const int start_id) = 0;
	void buildNet() {
		try {
				PipeSection::coord = coordTubeOnly(0);
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