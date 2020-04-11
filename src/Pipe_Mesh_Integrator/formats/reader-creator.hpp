#ifndef _READER_CREATOR_HPP_
#define _READER_CREATOR_HPP_


#include "glass-format.hpp"
#include  "stl-format.hpp"

#include "../first-boundaries-saver.hpp"
#include "../t-matrix-saver.hpp"


template <class PointType, class NetType>
class ReaderCreator {
public:
	ReaderCreator() {};
	virtual ~ReaderCreator() {};

	FileFormater <PointType, NetType>* createFormater(const json& meshParams, IMesh<PointType, NetType>* mesh)
	{

		cout << "Reading Outter Mesh from files: " << endl;
		FileFormater<PointType, NetType>* reader;

		if (meshParams["format"] == "Glass") {

			cout << meshParams["paths"]["inftry"] << endl << meshParams["paths"]["nver"] << endl
				<< meshParams["paths"]["nvkat"] << endl << meshParams["paths"]["xyz"] << endl;

			const vector<string> paths{ meshParams["paths"]["inftry"] ,meshParams["paths"]["nvkat"],meshParams["paths"]["xyz"],meshParams["paths"]["nver"] };

			reader = new GlassFormat <PointType, NetType>(mesh);

			reader->setFilePathes(paths);

		}
		else
			if (meshParams["format"] == "stl")
			{
				cout << meshParams["paths"]["path"] << endl;
				const vector<string> paths{ meshParams["paths"]["path"] };
				reader = new StlFormat <PointType, NetType>(mesh);
				reader->setFilePathes(paths);
			}
			else
				throw runtime_error("Unknown input file format");

		return reader;

	};

};

#endif //_READER_CREATOR_HPP_