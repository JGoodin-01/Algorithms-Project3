/*
    ImageCarver.cpp

    Implementation file for the class that deals with carving.
*/

#include "ImageCarver.hpp"

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>

ImageCarver::ImageCarver(std::string filename)
{
    std::string fileEXT = filename.substr(filename.find_last_of(".") + 1);
    if (fileEXT != "pgm")
    {
        std::cerr << "File type is not supported." << std::endl;
        exit(1);
    }

    std::ifstream infile(filename);
    std::string line;

    int indexer = 0;
    while (std::getline(infile, line))
    {
        if (indexer == 2)
        {
            // int spaceIndex = line.find(" ");
            // imageWidth = std::stoi(line.substr(0, spaceIndex));
            // imageHeight = std::stoi(line.substr(spaceIndex, line.size()));
        }
        else if (indexer > 3)
        {
            std::vector<int> row;
            std::istringstream iss(line);
            int num;
            while (iss >> num) {
                row.push_back(num);
            }
            image.push_back(row);
        }

        ++indexer;
    }

    // for (const auto& row : image) {
    //     for (int num : row) {
    //         std::cout << num << " ";
    //     }
    //     std::cout << std::endl;
    // }
}
