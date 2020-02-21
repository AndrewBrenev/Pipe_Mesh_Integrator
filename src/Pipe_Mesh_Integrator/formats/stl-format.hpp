#ifndef _STL_FORMAT_HPP_
#define _STL_FORMAT_HPP_

#include "flie-format.hpp"

// STL format Viewer 
// https://www.viewstl.com 

template <class PointType, class NetType>
class StlFormat : public FileFormater<PointType, NetType> {
private:

	string filePath;

public:
	StlFormat() {};

	StlFormat(IMesh<PointType, NetType>* mesh) {
		FileFormater<PointType, NetType>::meshToOperate = mesh;
	};
	

	~StlFormat() {};
	void setFilePathes(const vector<string>& inputFiles) override
	{
		filePath = inputFiles[0];

	};
	bool saveMeshToFiles() override
	{
		 unordered_set<Plane> planes = FileFormater<PointType, NetType>::meshToOperate->getPlanesFormingMesh();
		
		 FILE* fout;
		 if (fopen_s(&fout, filePath.c_str(), "wb")) return false;
		 else {

			 //binary file
			 std::string header_info = "Not solid " + filePath + "-output";
			 char head[80];
			 std::strncpy(head, header_info.c_str(), sizeof(head) - 1);
			 char attribute[2] = "0";
			 unsigned long nTriLong = planes.size() * 2;

			 fwrite(head, sizeof(char), 80, fout);
			 fwrite(&nTriLong, sizeof(unsigned long), 1, fout);

			 //write down every triangle
			 for (std::unordered_set<Plane>::iterator it = planes.begin(); it != planes.end(); it++) {
				 //normal vector coordinates

				 auto nodes = it->getNodesIds();
				 PointType points[4];
				 for (int i = 0; i < 4; ++i)
					 points[i] = FileFormater<PointType, NetType>::meshToOperate->getNode(nodes[i]);

				 PointType normal = it->getNormal();

				 float* data = new float[12];
				 data[0] = normal.x;	data[1] = normal.y;	data[2] = normal.z;
				 data[3] = points[0].x;	data[4] = points[0].y;	data[5] = points[0].z;
				 data[6] = points[1].x;	data[7] = points[1].y;	data[8] = points[1].z;
				 data[9] = points[2].x;	data[10] = points[2].y;	data[11] = points[2].z;
				 fwrite(data, sizeof(float), 12, fout);
				 fwrite(attribute, sizeof(char), 2, fout);

				 // second triengle
				 data[3] = points[1].x;	data[4] = points[1].y;	data[5] = points[1].z;
				 data[6] = points[2].x;	data[7] = points[2].y;	data[8] = points[2].z;
				 data[9] = points[3].x;	data[10] = points[3].y;	data[11] = points[3].z;
				 fwrite(data, sizeof(float), 12, fout);
				 fwrite(attribute, sizeof(char), 2, fout);
			 }

			 fclose(fout);
		 }

		return true;
	};

	bool readMeshFromFiles() override
	{
		// ....
		return true;
	};

};

#endif //_GLASS_FORMAT_HPP_