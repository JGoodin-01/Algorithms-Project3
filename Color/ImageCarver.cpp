/*
    ImageCarver.cpp

    Implementation file for the class that deals with carving.
*/

#include "ImageCarver.hpp"

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::min;
using std::ofstream;
using std::string;
using std::vector;

ImageCarver::ImageCarver()
{
    pgmData data;
    data.version = "";
    data.comment = "";
    data.columns = 0;
    data.rows = 0;
    data.maxValue = 0;
}

// Carves out vertical and horizontal seams of an image
int ImageCarver::carve(int argc, char *argv[])
{
    // Create 2D Arrays and read PGM file
    std::string filename = argv[1];
    pgmData imageData;
    if (filename.substr(filename.find_last_of(".") + 1) == "pgm")
    {
        int **pgmValues = this->readPGM(argv[1], imageData);
        int **pixelEnergy = this->create2DArray(imageData.columns, imageData.rows);
        int **cumulativeEnergy = this->create2DArray(imageData.columns, imageData.rows);

        // Calculate Energy Matrices
        this->calculateEnergyMatrix(imageData.columns, imageData.rows, pgmValues, pixelEnergy);
        this->vertCumulativeEnergy(imageData.columns, imageData.rows, pixelEnergy, cumulativeEnergy);

        // Remove vert seams
        for (auto i = 0; i < atoi(argv[2]); ++i)
        {
            this->removeVerticalSeam(imageData.columns, imageData.rows, pgmValues, cumulativeEnergy);
            imageData.columns--;

            this->calculateEnergyMatrix(imageData.columns, imageData.rows, pgmValues, pixelEnergy);
            this->vertCumulativeEnergy(imageData.columns, imageData.rows, pixelEnergy, cumulativeEnergy);
        }

        // Remove horiz seams via matrix transpose
        int **transposedPGM = this->transposeMatrix(imageData.columns, imageData.rows, pgmValues);
        int **transposedPixelEnergy = this->transposeMatrix(imageData.columns, imageData.rows, pixelEnergy);
        int **transposedCumulativeEnergy = this->transposeMatrix(imageData.columns, imageData.rows, cumulativeEnergy);

        this->calculateEnergyMatrix(imageData.rows, imageData.columns, transposedPGM, transposedPixelEnergy);
        this->vertCumulativeEnergy(imageData.rows, imageData.columns, transposedPixelEnergy, transposedCumulativeEnergy);

        for (auto j = 0; j < atoi(argv[3]); ++j)
        {
            this->removeVerticalSeam(imageData.rows, imageData.columns, transposedPGM, transposedCumulativeEnergy);
            imageData.rows--;

            this->calculateEnergyMatrix(imageData.rows, imageData.columns, transposedPGM, transposedPixelEnergy);
            this->vertCumulativeEnergy(imageData.rows, imageData.columns, transposedPixelEnergy, transposedCumulativeEnergy);
        }

        // Transpose back to original matrix
        pgmValues = this->transposeMatrix(imageData.rows, imageData.columns, transposedPGM);

        // Create new image
        string newFileName = string(argv[1]).substr(0, string(argv[1]).find(".pgm"));
        newFileName += "_processed_" + string(argv[2]) + "_" + string(argv[3]) + ".pgm";
        this->writePGM(newFileName, imageData, pgmValues);
        cout << "\nNew image generated" << endl;
    }
    else if (filename.substr(filename.find_last_of(".") + 1) == "ppm")
    {
        int ***pgmValues = this->readPPM(argv[1], imageData);
        int **pixelEnergy = this->create2DArray(imageData.columns, imageData.rows);
        int **cumulativeEnergy = this->create2DArray(imageData.columns, imageData.rows);

        // Calculate Energy Matrices
        this->calculateEnergyMatrix(imageData.columns, imageData.rows, pgmValues, pixelEnergy);
        this->vertCumulativeEnergy(imageData.columns, imageData.rows, pixelEnergy, cumulativeEnergy);

        // Remove vert seams
        for (auto i = 0; i < atoi(argv[2]); ++i)
        {
            this->removeVerticalSeam(imageData.columns, imageData.rows, pgmValues, cumulativeEnergy);
            imageData.columns--;

            this->calculateEnergyMatrix(imageData.columns, imageData.rows, pgmValues, pixelEnergy);
            this->vertCumulativeEnergy(imageData.columns, imageData.rows, pixelEnergy, cumulativeEnergy);
        }

        // Remove horiz seams via matrix transpose
        int ***transposedPPM = this->transposeColorMatrix(imageData.columns, imageData.rows, pgmValues);
        int **transposedPixelEnergy = this->transposeMatrix(imageData.columns, imageData.rows, pixelEnergy);
        int **transposedCumulativeEnergy = this->transposeMatrix(imageData.columns, imageData.rows, cumulativeEnergy);

        this->calculateEnergyMatrix(imageData.rows, imageData.columns, transposedPPM, transposedPixelEnergy);
        this->vertCumulativeEnergy(imageData.rows, imageData.columns, transposedPixelEnergy, transposedCumulativeEnergy);

        for (auto j = 0; j < atoi(argv[3]); ++j)
        {
            this->removeVerticalSeam(imageData.rows, imageData.columns, transposedPPM, transposedCumulativeEnergy);
            imageData.rows--;

            this->calculateEnergyMatrix(imageData.rows, imageData.columns, transposedPPM, transposedPixelEnergy);
            this->vertCumulativeEnergy(imageData.rows, imageData.columns, transposedPixelEnergy, transposedCumulativeEnergy);
        }

        // Transpose back to original matrix
        pgmValues = this->transposeColorMatrix(imageData.rows, imageData.columns, transposedPPM);

        // Create new image
        string newFileName = string(argv[1]).substr(0, string(argv[1]).find(".ppm"));
        newFileName += "_processed_" + string(argv[2]) + "_" + string(argv[3]) + ".ppm";
        this->writePPM(newFileName, imageData, pgmValues);
        cout << "\nNew image generated" << endl;
    }

    return 0;
}

// Dynamically creates a 2D array based off of the input image file
int **ImageCarver::create2DArray(const int &numCols, const int &numRows)
{
    // Create parent array
    int **arr = new int *[numRows];

    // Creates subarrays in each image
    for (int j = 0; j < numRows; ++j)
    {
        arr[j] = new int[numCols];
    }

    return arr;
}

// Dynamically creates a 2D array based off of the input image file
int ***ImageCarver::create2DColorArray(const int &numCols, const int &numRows)
{
    // Create parent array
    int ***arr = new int **[numRows];

    // Creates subarrays in each row
    for (int j = 0; j < numRows; ++j)
    {
        arr[j] = new int *[numCols];

        // Create subsection for colors
        for (int i = 0; i < numCols; ++i)
        {
            arr[j][i] = new int[3];
        }
    }

    return arr;
}

// Transposes a 2D array
int **ImageCarver::transposeMatrix(const int &numCols, const int &numRows, int **arr)
{
    // This flips the Rows & Cols. Dont get confused by the parameters
    int **newArr = create2DArray(numRows, numCols);

    for (auto i = 0; i < numCols; ++i)
    {
        for (auto j = 0; j < numRows; ++j)
        {
            newArr[i][j] = arr[j][i];
        }
    }

    return newArr;
}

// Transposes a 2D array
int ***ImageCarver::transposeColorMatrix(const int &numCols, const int &numRows, int ***arr)
{
    // This flips the Rows & Cols. Dont get confused by the parameters
    int ***newArr = create2DColorArray(numRows, numCols);

    for (auto i = 0; i < numCols; ++i)
    {
        for (auto j = 0; j < numRows; ++j)
        {
            newArr[i][j] = arr[j][i];
        }
    }

    return newArr;
}

// Reads in PGM File
int **ImageCarver::readPGM(const string &fileName, pgmData &imageData)
{
    ifstream image;
    image.open(fileName);

    // Read in all PGM Header Data
    getline(image, imageData.version);
    getline(image, imageData.comment);
    image >> imageData.columns >> imageData.rows >> imageData.maxValue;

    int **imageArray = create2DArray(imageData.columns, imageData.rows);

    for (auto i = 0; i < imageData.rows; ++i)
    {
        for (auto j = 0; j < imageData.columns; ++j)
        {
            image >> imageArray[i][j];
        }
    }

    return imageArray;
}

// Output PGM text to a new file
void ImageCarver::writePGM(const string &fileName, pgmData &imageData, int **image)
{
    ofstream imageProcessed;
    imageProcessed.open(fileName);

    // Add header info
    imageProcessed << imageData.version << endl;
    imageProcessed << imageData.comment << endl;
    imageProcessed << imageData.columns << ' ' << imageData.rows << endl;
    imageProcessed << imageData.maxValue << endl;

    // Add pixels
    for (auto i = 0; i < imageData.rows; ++i)
    {
        for (auto j = 0; j < imageData.columns; ++j)
        {
            imageProcessed << image[i][j] << ' ';
        }

        imageProcessed << endl;
    }

    imageProcessed.close();
}

// Reads in PPM File
int ***ImageCarver::readPPM(const string &fileName, pgmData &imageData)
{
    ifstream image;
    image.open(fileName);

    // Read in all PGM Header Data
    getline(image, imageData.version);
    getline(image, imageData.comment);
    image >> imageData.columns >> imageData.rows >> imageData.maxValue;

    int ***imageArray = create2DColorArray(imageData.columns, imageData.rows);

    for (auto i = 0; i < imageData.rows; ++i)
    {
        for (auto j = 0; j < imageData.columns; ++j)
        {
            for (auto k = 0; k < 3; ++k)
                image >> imageArray[i][j][k];
        }
    }

    return imageArray;
}

// Output PPM text to a new file
void ImageCarver::writePPM(const string &fileName, pgmData &imageData, int ***image)
{
    ofstream imageProcessed;
    imageProcessed.open(fileName);

    // Add header info
    imageProcessed << imageData.version << endl;
    imageProcessed << imageData.comment << endl;
    imageProcessed << imageData.columns << ' ' << imageData.rows << endl;
    imageProcessed << imageData.maxValue << endl;

    // Add pixels
    for (auto i = 0; i < imageData.rows; ++i)
    {
        for (auto j = 0; j < imageData.columns; ++j)
        {
            for (auto k = 0; k < 3; ++k)
                imageProcessed << image[i][j][k] << ' ';
        }

        imageProcessed << endl;
    }

    imageProcessed.close();
}

// Calculate the energy matrix of an image
void ImageCarver::calculateEnergyMatrix(const int &numCols, const int &numRows, int **imageMatrix, int **energyMatrix)
{
    int value, above, below, left, right;

    // Loop through & columns
    for (auto i = 0; i < numRows; ++i)
    {
        for (auto j = 0; j < numCols; ++j)
        {
            value = imageMatrix[i][j];

            if (i == 0)
            {
                above = value;
                below = imageMatrix[i + 1][j];
            }
            else if (i == (numRows - 1))
            {
                above = imageMatrix[i - 1][j];
                below = value;
            }
            else
            {
                above = imageMatrix[i - 1][j];
                below = imageMatrix[i + 1][j];
            }

            if (j == 0)
            {
                left = value;
                right = imageMatrix[i][j + 1];
            }
            else if (j == (numCols - 1))
            {
                left = imageMatrix[i][j - 1];
                right = value;
            }
            else
            {
                left = imageMatrix[i][j - 1];
                right = imageMatrix[i][j + 1];
            }

            // Equation to determine the average energy of a pixel from the pixels around it
            energyMatrix[i][j] = abs(value - above) + abs(value - below) + abs(value - left) + abs(value - right);
        }
    }
}

// Determines the vertical cumulative energy of the image
void ImageCarver::vertCumulativeEnergy(const int &numCols, const int &numRows, int **energyMatrix, int **cEnergyMatrix)
{
    int value, first, second, last;

    // Loop through rows & columns
    for (auto i = 0; i < numRows; ++i)
    {
        for (auto j = 0; j < numCols; ++j)
        {
            value = energyMatrix[i][j];

            // If top row
            if (i == 0)
            {
                cEnergyMatrix[i][j] = value;
            }
            else
            {
                // If first column
                if (j == 0)
                {
                    first = 99999999;
                    second = cEnergyMatrix[i - 1][j];
                    last = cEnergyMatrix[i - 1][j + 1];
                }
                // If last column
                else if (j == (numCols - 1))
                {
                    first = cEnergyMatrix[i - 1][j - 1];
                    second = cEnergyMatrix[i - 1][j];
                    last = 99999999;
                }
                else
                {
                    first = cEnergyMatrix[i - 1][j - 1];
                    second = cEnergyMatrix[i - 1][j];
                    last = cEnergyMatrix[i - 1][j + 1];
                }

                cEnergyMatrix[i][j] = value + min(min(first, second), last);
            }
        }
    }
}

// Removes the lowest energy vertical seam
void ImageCarver::removeVerticalSeam(const int &numCols, const int &numRows, int **imageMatrix, int **cEnergyMatrix)
{
    // Start with bottom left pixel as lowest energy seam
    int lowestEnergySeam = cEnergyMatrix[numRows - 1][0];
    int first, second, last;
    int index = 0;

    // Find leftmost lowest energy seam in bottom row
    for (auto j = 0; j < numCols; ++j)
    {
        if (cEnergyMatrix[numRows - 1][j] < lowestEnergySeam)
        {
            lowestEnergySeam = cEnergyMatrix[numRows - 1][j];
            index = j;
        }
    }

    // Loop through rows
    for (auto i = (numRows - 1); i >= 0; --i)
    {
        // Remove lowest cumulative energy pixel by shifting pixels to its right to the left one
        for (auto j = 0; j < numCols - index; ++j)
        {
            // Shift pixels in original pixel array to the left to remove valueent pixel in seam
            if (j < numCols - index - 1)
                imageMatrix[i][index + j] = imageMatrix[i][index + j + 1];
            // Replace pixel with -1 if in rightmost column to avoid out of bounds memory access
            else
                imageMatrix[i][index + j] = -1;
        }

        // If more rows above most recent pixel removed, move up to next leftmost pixel in seam
        if (i > 0)
        {
            // If first column
            if (index == 0)
            {
                first = 99999999;
                second = cEnergyMatrix[i - 1][index];
                last = cEnergyMatrix[i - 1][index + 1];
            }
            // If last column
            else if (index == (numCols - 1))
            {
                first = cEnergyMatrix[i - 1][index - 1];
                second = cEnergyMatrix[i - 1][index];
                last = 99999999;
            }
            else
            {
                first = cEnergyMatrix[i - 1][index - 1];
                second = cEnergyMatrix[i - 1][index];
                last = cEnergyMatrix[i - 1][index + 1];
            }

            // Determine lowest energy pixel
            lowestEnergySeam = min(min(first, second), last);

            if (lowestEnergySeam == first)
                index--;
            else if (lowestEnergySeam == second)
                continue;
            else
                index++;
        }
    }
}

// Overload to calculate the energy matrix of an color image
void ImageCarver::calculateEnergyMatrix(const int &numCols, const int &numRows, int ***imageMatrix, int **energyMatrix)
{
    int value, above, below, left, right;

    // Loop through & columns
    for (auto i = 0; i < numRows; ++i)
    {
        for (auto j = 0; j < numCols; ++j)
        {
            int colorEnergy = 0;
            for (auto k = 0; k < 3; ++k)
            {
                value = imageMatrix[i][j][k];

                if (i == 0)
                {
                    above = value;
                    below = imageMatrix[i + 1][j][k];
                }
                else if (i == (numRows - 1))
                {
                    above = imageMatrix[i - 1][j][k];
                    below = value;
                }
                else
                {
                    above = imageMatrix[i - 1][j][k];
                    below = imageMatrix[i + 1][j][k];
                }

                if (j == 0)
                {
                    left = value;
                    right = imageMatrix[i][j + 1][k];
                }
                else if (j == (numCols - 1))
                {
                    left = imageMatrix[i][j - 1][k];
                    right = value;
                }
                else
                {
                    left = imageMatrix[i][j - 1][k];
                    right = imageMatrix[i][j + 1][k];
                }

                // Equation to determine the average energy of a pixel from the pixels around it
                colorEnergy += std::pow(abs(value - above) + abs(value - below) + abs(value - left) + abs(value - right), 2);
            }
            energyMatrix[i][j] = colorEnergy;
        }
    }
}

// Removes the lowest energy vertical seam in a color image
void ImageCarver::removeVerticalSeam(const int &numCols, const int &numRows, int ***imageMatrix, int **cEnergyMatrix)
{
    // Start with bottom left pixel as lowest energy seam
    int lowestEnergySeam = cEnergyMatrix[numRows - 1][0];
    int first, second, last;
    int index = 0;

    // Find leftmost lowest energy seam in bottom row
    for (auto j = 0; j < numCols; ++j)
    {
        if (cEnergyMatrix[numRows - 1][j] < lowestEnergySeam)
        {
            lowestEnergySeam = cEnergyMatrix[numRows - 1][j];
            index = j;
        }
    }

    // Loop through rows
    for (auto i = (numRows - 1); i >= 0; --i)
    {
        // Remove lowest cumulative energy pixel by shifting pixels to its right to the left one
        for (auto j = 0; j < numCols - index; ++j)
        {
            // Shift pixels in original pixel array to the left to remove valueent pixel in seam
            if (j < numCols - index - 1)
                imageMatrix[i][index + j] = imageMatrix[i][index + j + 1];
            // Replace pixel with -1 if in rightmost column to avoid out of bounds memory access
            else
            {
                int replacement [3] = {-1, -1, -1};
                imageMatrix[i][index + j] = replacement;
            }
        }

        // If more rows above most recent pixel removed, move up to next leftmost pixel in seam
        if (i > 0)
        {
            // If first column
            if (index == 0)
            {
                first = 99999999;
                second = cEnergyMatrix[i - 1][index];
                last = cEnergyMatrix[i - 1][index + 1];
            }
            // If last column
            else if (index == (numCols - 1))
            {
                first = cEnergyMatrix[i - 1][index - 1];
                second = cEnergyMatrix[i - 1][index];
                last = 99999999;
            }
            else
            {
                first = cEnergyMatrix[i - 1][index - 1];
                second = cEnergyMatrix[i - 1][index];
                last = cEnergyMatrix[i - 1][index + 1];
            }

            // Determine lowest energy pixel
            lowestEnergySeam = min(min(first, second), last);

            if (lowestEnergySeam == first)
                index--;
            else if (lowestEnergySeam == second)
                continue;
            else
                index++;
        }
    }
}