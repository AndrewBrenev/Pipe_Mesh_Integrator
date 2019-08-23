#include "./meshes/combined-mesh.hpp"
#include "json-deserializer.hpp"

int main()
{
	try {
		TaskConfig currentConfigs;
		auto taskParametrs = currentConfigs.deserializeJsonFile("config.json");
		CombinedMesh <Point, NVTR> A(taskParametrs);
		A.buildNet();
	}
	catch (exception& e)
	{
		cout << "Failed! Found an exeption: " << e.what() << endl;
	}
	system("pause");
 }
