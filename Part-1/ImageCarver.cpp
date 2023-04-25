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
            while (iss >> num)
            {
                row.push_back(num);
            }
            image.push_back(row);
        }

        ++indexer;
    }
}

void ImageCarver::carve(int horizontalLines, int verticalLines)
{
    int combinedCutLines = horizontalLines + verticalLines;
    horizontalCarves += horizontalLines;
    verticalCarves += verticalLines;

    
    std::vector<std::vector<int>> imageEnergy;
    findImageEnergy(imageEnergy);

    // Output Image Contents
    for (const auto &row : imageEnergy)
    {
        for (int element : row)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
}

void ImageCarver::displayVector()
{
    for (const auto &row : image)
    {
        for (int num : row)
        {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
}

void ImageCarver::printImage()
{
    std::string outputFileName = "./" + fileName + "_processed_v_h_.pgm";
    std::ofstream outputFile(outputFileName);
    if (!outputFile)
    {
        std::cerr << "Error opening file " << outputFileName << " for writing\n";
        return;
    }

    // Output File Header
    outputFile << "P2\n"
               << "# Created by IrfanView\n"
               << imageWidth << " " << imageHeight << '\n'
               << "255\n";

    // Output Image Contents
    for (const auto &row : image)
    {
        for (int element : row)
        {
            outputFile << element << " ";
        }
        outputFile << std::endl;
    }
}

int ImageCarver::energyValue(int left, int top, int right, int bottom, int base)
{
    return abs(base - left) + abs(base - top) + abs(base - right) + abs(base - bottom);
}

void ImageCarver::findImageEnergy(std::vector<std::vector<int>>& energyVector)
{
    int value;
    for (int i = 0; i < image.size(); ++i)
    {
        std::vector<int> row;
        for (int j = 0; j < image[i].size(); ++j)
        {
            if (i == 0 && j == 0)
                value = energyValue(image[i][j], image[i][j], image[i][j + 1], image[i + 1][j], image[i][j]);
            else if (i == (image.size() - 1) && (j == (image[i].size() - 1)))
                value = energyValue(image[i][j - 1], image[i - 1][j], image[i][j], image[i][j], image[i][j]);
            else if (i == 0 && (j == (image[i].size() - 1)))
                value = energyValue(image[i][j - 1], image[i][j], image[i][j], image[i + 1][j], image[i][j]);
            else if (j == 0 && (i == (image.size() - 1)))
                value = energyValue(image[i][j], image[i - 1][j], image[i][j + 1], image[i][j], image[i][j]);
            else if (i == 0)
                value = energyValue(image[i][j - 1], image[i][j], image[i][j + 1], image[i + 1][j], image[i][j]);
            else if (j == 0)
                value = energyValue(image[i][j], image[i - 1][j], image[i][j + 1], image[i + 1][j], image[i][j]);
            else if (i == image.size() - 1)
                value = energyValue(image[i][j - 1], image[i - 1][j], image[i][j + 1], image[i][j], image[i][j]);
            else if (j == image[i].size() - 1)
                value = energyValue(image[i][j - 1], image[i - 1][j], image[i][j], image[i + 1][j], image[i][j]);
            else
                value = energyValue(image[i][j - 1], image[i - 1][j], image[i][j + 1], image[i + 1][j], image[i][j]);
            row.push_back(value);
        }
        energyVector.push_back(row);
    }
}
