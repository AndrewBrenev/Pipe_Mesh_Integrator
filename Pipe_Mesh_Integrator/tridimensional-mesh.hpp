#ifndef _TRIDIMENSIONAL_MESH_HPP_
#define _TRIDIMENSIONAL_MESH_HPP_

#include "interface.h"

template <class PointType, class NetType>
class TridimensionalMesh : public IMesh<PointType, NetType> {
protected:
	bool convertToObj() {};
public:
	TridimensionalMesh() {};
	~TridimensionalMesh() {};
	void buildNet() {
		if (!IMesh<PointType, NetType>::readFromFiles("./incoming-net/inftry.dat",
			"./incoming-net/nvkat.dat", "./incoming-net/xyz.dat", "./incoming-net/nver.dat")
			) {
			cout << "Warning : Unable to read the mesh from \". / incoming - net\"... " << endl;
			
			//Границы области и шаг разбиения
			real X0, Xn, Xh;
			real Y0, Yn, Yh;
			real Z0, Zn, Zh;

			//считывание координат
			FILE* file = fopen("./input-info/input_mesh.txt", "r");
			if (file == NULL) cout << "Error : Unable to read the mesh" << endl;;
			fscanf(file, "X : ( %lf ; %lf ) h = %lf\n", &X0, &Xn, &Xh);
			fscanf(file, "Y : ( %lf ; %lf ) h = %lf\n", &Y0, &Yn, &Yh);
			fscanf(file, "Z : ( %lf ; %lf ) h = %lf\n", &Z0, &Zn, &Zh);

			PointType T; NetType N;

			int id = 0;
			int nX = (Xn - X0) / Xh;
			int nY = (Yn - Y0) / Yh;
			int nZ = (Zn - Z0) / Zh;
			int el_layer = (nZ + 1)*(nX + 1);
			int t_el = 0;
			// Движение по OY
			for (int j = 0; j <= nY; j++) {
				// Движени по OZ
				for (int k = 0; k <= nZ; k++) {
					//Движение по OX
					for (int i = 0; i <= nX; i++)
					{
						T.id = id;
						T.x = X0 + i * Xh;
						T.y = Y0 + j * Yh;
						T.z = Z0 + k * Zh;
						TridimensionalMesh<PointType, NetType>::coord.push_back(T);
						id++;

						if (i != nX && j != nY && k != nZ)
						{
							NetType N(
								(k + 1) *(nX + 1) + (i + 1) + j * el_layer + 1,
								k * (nX + 1) + (i + 1) + j * el_layer + 1,
								(k + 1) * (nX + 1) + i + j * el_layer + 1,
								k * (nX + 1) + i + j * el_layer + 1,
								(k + 1) *(nX + 1) + (i + 1) + (j + 1) * el_layer + 1,
								k * (nX + 1) + (i + 1) + (j + 1) * el_layer + 1,
								(k + 1) * (nX + 1) + i + (j + 1) * el_layer + 1,
								k * (nX + 1) + i + (j + 1) * el_layer + 1,
								t_el,
								GROUND
							);
							TridimensionalMesh<PointType, NetType>::nvtr.push_back(N);
							t_el++;
						}
					}
				}
			}
			IMesh<PointType, NetType>::setNodesSize(TridimensionalMesh<PointType, NetType>::coord.size());
			IMesh<PointType, NetType>::setElemsSize(TridimensionalMesh<PointType, NetType>::nvtr.size());
		}

		
	};
};

#endif