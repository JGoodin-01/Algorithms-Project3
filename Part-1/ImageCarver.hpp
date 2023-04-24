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

private:
    std::vector<std::vector<int>> image;
    // int imageWidth; // columns
    // int imageHeight; // rows
};

#endif
