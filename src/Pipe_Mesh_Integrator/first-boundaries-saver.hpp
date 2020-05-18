#pragma once

#include "stdafx.h"


class FirstBoundariesSaver  {

private:
	const vector<size_t>& nodesIds;
public:
	FirstBoundariesSaver(const vector<size_t>& _nodesIds) :nodesIds(_nodesIds) {};

	~FirstBoundariesSaver() {};

	void saveFirstBoundaryNodesToFile(const string& filePath)
	{
		FILE* b = fopen(filePath.c_str(), "w");
		fprintf(b, "%d\n", nodesIds.size());
		for (size_t node : nodesIds)
			fprintf(b, "%d ", node);
		fclose(b);

	}
};

