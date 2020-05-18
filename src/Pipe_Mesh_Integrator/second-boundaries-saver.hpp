#pragma once

#include "stdafx.h"


class SecondBoundariesSaver {

private:
	const unordered_set<Plane>& planes;
public:
	SecondBoundariesSaver(const unordered_set<Plane>& _planes) : planes(_planes) {};

	~SecondBoundariesSaver() {};

	void saveSecondBoundaryNodesToFile(const string& filePath)
	{
		FILE* b = fopen(filePath.c_str(), "w");
		fprintf(b, "%d\n", planes.size());
		for (auto plane : planes)
			fprintf(b, "%d %d %d %d  ", plane.getNode(0), plane.getNode(1), plane.getNode(2), plane.getNode(3));
		fclose(b);
	}
};

