#include "./meshes/combined-mesh.hpp"
#include "json-deserializer.hpp"

int main(int argc, const char** argv)
{
	try {

		auto numberOfThreads = omp_get_max_threads();
		omp_set_num_threads(numberOfThreads);
	
		if (argc == 2) {

			time_t start, end;

			time(&start);

			cout << "Program is running using " << numberOfThreads << " parallel threads." << endl;

			string configPath = argv[1];
			cout << "Reading the system configuration file : " << configPath << endl;
			TaskConfig currentConfigs;
			auto taskParametrs = currentConfigs.deserializeJsonFile(configPath.c_str());

			cout << "The construction of the finite element mesh was launched." << endl;
			CombinedMesh <Point, NVTR> A(taskParametrs);
			A.buildNet();

			time(&end);
			real seconds = difftime(end, start);

			(seconds) ?
				cout << "All calculations took " << seconds << " second(s)." << endl :
				cout << "Wow! All calculations took less then second!" << endl;

		}else
			throw runtime_error("Incorrect number of function arguments! The program takes only one argument: the relative path to the config file. The path and file name must not contain spaces. Check the conditions and try again.");
	}
	catch (exception& e)
	{
		cout << "Failed! Found an exeption: " << e.what() << endl;
	}
	system("pause");
 }
