/*
    ImageCarver.hpp

    Include file for the class that deals with carving.
*/

#include <string>
#include <vector>

#ifndef INCLUDED_IMAGECARVERCLASS_HPP
#define INCLUDED_IMAGECARVERCLASS_HPP

class ImageCarver {
public:
    ImageCarver(std::string filename);
    ~ImageCarver() = default;

    void carve(int horizontalLines, int verticalLines);
    void displayVector();
    void printImage();
    
private:
    std::string fileName;
    std::vector<std::vector<int>> image;
    int imageWidth, imageHeight, horizontalCarves, verticalCarves;

    int energyValue (int left, int top, int right, int bottom, int base);
    void findImageEnergy(std::vector<std::vector<int>>& energyVector);
};

#endif
