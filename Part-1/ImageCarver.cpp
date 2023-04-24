/*
    ImageCarver.cpp

    Implementation file for the class that deals with carving.
*/

#include "ImageCarver.hpp"

#include <iostream>
#include <iterator>
#include <fstream>

ImageCarver::ImageCarver(std::string filename)
{
    std::string fileEXT = filename.substr(filename.find_last_of(".") + 1);
    if (fileEXT != "ppm" && fileEXT != "pgm")
    {
        std::cerr << "File type is not supported." << std::endl;
        exit(1);
    }

    std::ifstream infile(filename);
    std::string line;

    int indexer = 0;
    isColor = false;
    while (std::getline(infile, line))
    {
        if (indexer == 0 && line == "P3")
            isColor = true;
        else if (indexer == 2)
        {
            int spaceIndex = line.find(" ");
            int endLineIndex = line.find("\n");
            imageWidth = std::stoi(line.substr(0, spaceIndex));
            imageHeight = std::stoi(line.substr(spaceIndex, endLineIndex));
            std::cout << imageWidth << "/" << imageHeight;
        }

        ++indexer;
    }
}
