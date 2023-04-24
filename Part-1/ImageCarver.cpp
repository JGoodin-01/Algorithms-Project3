/*
    ImageCarver.cpp

    Implementation file for the class that deals with carving.
*/

#include "ImageCarver.hpp"

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>

ImageCarver::ImageCarver(std::string filename) : horizontalCarves(0), verticalCarves(0)
{
    std::string fileEXT = filename.substr(filename.find_last_of(".") + 1);
    if (fileEXT != "pgm")
    {
        std::cerr << "File type is not supported." << std::endl;
        exit(1);
    }

    fileName = filename.substr(0, filename.find_last_of("."));
    std::ifstream infile(filename);
    std::string line;

    int indexer = 0;
    while (std::getline(infile, line))
    {   
        if (indexer == 2)
        {
            int spaceIndex = line.find(" ");
            imageWidth = std::stoi(line.substr(0, spaceIndex));
            imageHeight = std::stoi(line.substr(spaceIndex, line.size()));
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
}

void ImageCarver::carve(int horizontalLines, int verticalLines)
{
    horizontalCarves += horizontalLines;
    verticalCarves += verticalLines;
}

void ImageCarver::displayVector()
{
    for (const auto& row : image) {
        for (int num : row) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
}

void ImageCarver::printImage()
{
    std::string outputFileName = "./" + fileName + "_processed_v_h_.pgm";
    std::ofstream outputFile(outputFileName);
    if (!outputFile) {
        std::cerr << "Error opening file " << outputFileName << " for writing\n";
        return;
    }

    // Output File Header
    outputFile << "P2\n"
        << "# Created by IrfanView\n"
        << imageWidth << " " << imageHeight << '\n'
        << "255\n";
    
    // Output Image Contents
    for (const auto& row : image) {
        for (int element : row) {
            outputFile << element << " ";
        }
        outputFile << std::endl;
    }
}
