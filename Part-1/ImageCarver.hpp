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
        int maxGrayLevel;
    };

    pgmData data;

    void quit(const int &code, const std::string &message);

    int smallestOfThree(const int &x, const int &y, const int &z);

    int **allocateDynamic2DArray(const int &n, const int &m);

    int **transposeMatrix(const int &n, const int &m, int **arr);

    int **readPGM(const std::string &fileName, pgmData &data);

    void writePGM(const std::string &fileName, pgmData &data, int **arr);

    void calculateEnergyMatrix(const int &n, const int &m, int **arr, int **newArr);

    void removeVerticalSeam(const int &n, const int &m, int **imageArr, int **cEnergyArr);

    void verticalCumulativeEnergyMatrix(const int &n, const int &m, int **arr, int **newArr);

public:
    ImageCarver();

    int carve(int argc, char *argv[]);
};

#endif
