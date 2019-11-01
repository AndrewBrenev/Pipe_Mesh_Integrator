#ifndef _GLASS_FORMAT_HPP_
#define _GLASS_FORMAT_HPP_

#include "flie-format.hpp"


class GLassFormat : public FileFormat {

public:
	GLassFormat() {};
	virtual ~GLassFormat() {};

	virtual void saveMeshToFiles() {};
	virtual void readMeshFromFiles() {};

};

#endif //_GLASS_FORMAT_HPP_