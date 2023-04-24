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
    void carve(int horizontalLines, int verticalLines);
    void displayVector();
    void printImage();

private:
    std::string fileName;
    std::vector<std::vector<int>> image;
    int imageWidth, imageHeight, horizontalCarves, verticalCarves;
};

#endif
