#include "./meshes/combined-mesh.hpp"
#include "json-deserializer.hpp"

int main()
{
	try {

		auto numberOfThreads = omp_get_max_threads();
		omp_set_num_threads(numberOfThreads);

		cout << "Program is running using " << numberOfThreads <<" parallel threads."<< endl;
				
		string configPath = "../../examples/one-straight.json";
		cout << "Reading the system configuration file : " << configPath << endl;
		TaskConfig currentConfigs;
		auto taskParametrs = currentConfigs.deserializeJsonFile(configPath.c_str());


		cout << "The construction of the finite element mesh was launched." << endl;
		CombinedMesh <Point, NVTR> A(taskParametrs);
		A.buildNet();
	}
	catch (exception& e)
	{
		cout << "Failed! Found an exeption: " << e.what() << endl;
	}
	system("pause");
 }
