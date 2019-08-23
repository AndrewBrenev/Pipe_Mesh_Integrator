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
	//���������� ��������� ����� � �������
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