/*
    ImageCarver.hpp

    Include file for the class that deals with carving.
*/

#include <string>
#include <vector>

#ifndef INCLUDED_IMAGECARVERCLASS_HPP
#define INCLUDED_IMAGECARVERCLASS_HPP

class ImageCarver
{
private:
    struct pgmData
    {
        std::string version;
        std::string comment;
        int columns;
        int rows;
        int maxValue;
    };

    pgmData data;

    int **create2DArray(const int &numCols, const int &numRows);

    int ***create2DColorArray(const int &numCols, const int &numRows);

    int **transposeMatrix(const int &numCols, const int &numRows, int **arr);

    int ***transposeColorMatrix(const int &numCols, const int &numRows, int ***arr);

    int **readPGM(const std::string &fileName, pgmData &data);

    void writePGM(const std::string &fileName, pgmData &imageData, int **image);

    int ***readPPM(const std::string &fileName, pgmData &imageData);

    void writePPM(const std::string &fileName, pgmData &imageData, int ***image);

    void calculateEnergyMatrix(const int &numCols, const int &numRows, int **imageMatrix, int **energyMatrix);

    void removeVerticalSeam(const int &numCols, const int &numRows, int **imageMatrix, int **cEnergyMatrix);

    void vertCumulativeEnergy(const int &numCols, const int &numRows, int **energyMatrix, int **cEnergyMatrix);

    // For PPM files - for color
    void calculateEnergyMatrix(const int &numCols, const int &numRows, int ***imageMatrix, int **energyMatrix);

    void removeVerticalSeam(const int &numCols, const int &numRows, int ***imageMatrix, int **cEnergyMatrix);

public:
    ImageCarver();

    int carve(int argc, char *argv[]);
};

#endif
