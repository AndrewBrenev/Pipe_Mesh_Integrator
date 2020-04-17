#pragma once
 
#include "t-matrix.hpp"
#include"stdafx.h"

template<typename PointType,typename NetType>
class T_MatrixSaver {
private:
    T_Matrix<PointType,NetType>* t_matrix;

    string params, ig, gg, jg;

public:
    T_MatrixSaver(T_Matrix<PointType, NetType> * _t_matrix) : t_matrix(_t_matrix) {};
    ~T_MatrixSaver() {};
    // 0 - params, 1 - ig, 2- jg, 3 - gg
    void setFilePathes(const vector<string>& inputFiles)
    {
        params = inputFiles[0];
        ig = inputFiles[1];
        jg = inputFiles[2];
        gg = inputFiles[3];

    };
    void saveMatrixToFiles() {
        size_t columsCount = t_matrix->getTerminalNodesCount();
        //params
        FILE* file = fopen(params.c_str(), "w");
        fprintf(file, "%d %d\n", t_matrix->getMeshNodesCount(), columsCount);
        fclose(file);

        FILE* ig_file = fopen(ig.c_str(), "w");
        FILE* jg_file = fopen(jg.c_str(), "w");
        FILE* gg_file = fopen(gg.c_str(), "w");

        int ig_counter = 0;
        for (int i = 0; i < columsCount; ++i) {
            fprintf(ig_file, "%d ", ig_counter);
            auto column = t_matrix->getColumn(i);

            ig_counter += column.terminalRows.size();
            for (auto value : column.terminalRows) {
                fprintf(jg_file, "%d ", value.first);
                fprintf(gg_file, "%lf ", value.second);
            }
        }
        fprintf(ig_file, "%d ", ig_counter);
        fclose(ig_file);
        fclose(jg_file);
        fclose(gg_file);
    };
};