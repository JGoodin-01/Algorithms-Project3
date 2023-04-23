/*
    ImageCarver.hpp

    Include file for the class that deals with carving.
*/

#include <string>

#ifndef INCLUDED_IMAGECARVERCLASS_HPP
#define INCLUDED_IMAGECARVERCLASS_HPP

class ImageCarver {
public:
    ImageCarver();

private:
    int imageWidth; // columns
    int imageHeight; // rows
    bool isColor;
};

#endif
