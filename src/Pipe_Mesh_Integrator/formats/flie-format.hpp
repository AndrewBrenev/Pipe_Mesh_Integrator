#ifndef _FILE_FORMAT_HPP_
#define _FILE_FORMAT_HPP_

#include "../meshes/interface.h"


class FileFormat  {

public:
	FileFormat() {};
	virtual ~FileFormat() {};

	virtual void saveMeshToFiles() = 0;
	virtual void readMeshFromFiles() = 0;

};

#endif //_FILE_FORMAT_HPP_