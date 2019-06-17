#include "stdafx.h"

class TaskConfig {
public:
	TaskConfig() {};
	~TaskConfig() {};
	json deserializeJsonFile(const char *filePath) {
		std::ifstream inputStream(filePath);
		json configs;
		inputStream >> configs;
		return configs;
	}
};

