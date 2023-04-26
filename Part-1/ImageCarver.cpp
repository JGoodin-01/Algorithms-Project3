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
    data.maxGrayLevel = 0;
}

/**
 * @brief Carves out vertical and horizontal seams from a pgm file 
 *
 * This function checks for valid arguments, prints argument information, creates 2D arrays and reads the PGM file. 
 * It then calculates the energy/cumulative matrices, removes vertical and horizontal seams, transposes final matrix, and writes to the PGM file. 
 * Finally, it gracefully exits the program. 
 * 
 * @param argc The number of arguments passed in
 * @param argv An array of strings containing the arguments passed in
 * @return int Returns 0 upon successful completion 
 */
int ImageCarver::carve(int argc, char *argv[])
{
    // Check for valid arguments
    if (argc != 4)
        this->quit(1, "\nIncorrect Usage, Arguments Expected are ./program [filename].pgm [# of vertical seams to be removed] [# of horizontal seams to be removed]\n");

    // Print out argument information
    cout << endl
         << "File Name is: " << argv[1] << endl;
    cout << "Vertical Seams to Carve out: " << atoi(argv[2]) << endl;
    cout << "Horizontal Seams to Carve out: " << atoi(argv[3]) << endl;

    // Create 2D Arrays and read PGM file
    pgmData imageData;
    int **pgmValues = this->readPGM(argv[1], imageData);
    int **pixelEnergy = this->allocateDynamic2DArray(imageData.columns, imageData.rows);
    int **cumulativeEnergy = this->allocateDynamic2DArray(imageData.columns, imageData.rows);

    // Calculate Energy/Cumulative Matrices
    this->calculateEnergyMatrix(imageData.columns, imageData.rows, pgmValues, pixelEnergy);
    this->verticalCumulativeEnergyMatrix(imageData.columns, imageData.rows, pixelEnergy, cumulativeEnergy);

    // Remove vertical seams
    for (auto i = 0; i < atoi(argv[2]); ++i)
    {
        this->removeVerticalSeam(imageData.columns, imageData.rows, pgmValues, cumulativeEnergy);
        imageData.columns--;

        this->calculateEnergyMatrix(imageData.columns, imageData.rows, pgmValues, pixelEnergy);
        this->verticalCumulativeEnergyMatrix(imageData.columns, imageData.rows, pixelEnergy, cumulativeEnergy);
    }

    // Remove horizontal seams
    // Transpose matricies
    int **transposedPGM = this->transposeMatrix(imageData.columns, imageData.rows, pgmValues);
    int **transposedPixelEnergy = this->transposeMatrix(imageData.columns, imageData.rows, pixelEnergy);
    int **transposedCumulativeEnergy = this->transposeMatrix(imageData.columns, imageData.rows, cumulativeEnergy);

    this->calculateEnergyMatrix(imageData.rows, imageData.columns, transposedPGM, transposedPixelEnergy);
    this->verticalCumulativeEnergyMatrix(imageData.rows, imageData.columns, transposedPixelEnergy, transposedCumulativeEnergy);

    for (auto j = 0; j < atoi(argv[3]); ++j)
    {
        this->removeVerticalSeam(imageData.rows, imageData.columns, transposedPGM, transposedCumulativeEnergy);
        imageData.rows--;

        this->calculateEnergyMatrix(imageData.rows, imageData.columns, transposedPGM, transposedPixelEnergy);
        this->verticalCumulativeEnergyMatrix(imageData.rows, imageData.columns, transposedPixelEnergy, transposedCumulativeEnergy);
    }

    // Transpose final matrix
    pgmValues = this->transposeMatrix(imageData.rows, imageData.columns, transposedPGM);

    // Write PGM file
    string newFileName = string(argv[1]).substr(0, string(argv[1]).find(".pgm"));
    newFileName += "_processed_" + string(argv[2]) + "_" + string(argv[3]) + ".pgm";
    cout << "Writing to file: " << newFileName << endl;
    this->writePGM(newFileName, imageData, pgmValues);
    cout << "\nSeams Carved!" << endl;

    // End the program gracefully
    this->quit(0, "\nExiting\n");
    return 0;
}

/**
 * @brief Exits the program with an exit code and message 
 *
 * @param code The exit code
 * @param message The message to be printed
 */
void ImageCarver::quit(const int &code, const string &message)
{
    if (message != "")
    {
        cout << message << endl;
    }

    exit(code);
}

/**
 * @brief Return the smallest of three integer values
 *
 * @param x The first integer value
 * @param y The second integer value
 * @param z The third integer value
 *
 * @return The smallest of the three integer values
 */
int ImageCarver::smallestOfThree(const int &x, const int &y, const int &z)
{
    return min(min(x, y), z);
}

/**
 * @brief Dynamically Allocate a 2D n x m array
 *
 * @param n The number of columns in the array
 * @param m The number of rows in the array
 *
 * @return A dynamically allocated 2D array of size n x m
 */
int **ImageCarver::allocateDynamic2DArray(const int &n, const int &m)
{
    // Initialize the first dimension of m rows
    int **arr = new int *[m];

    // In each row, initialize n columns
    for (int j = 0; j < m; ++j)
    {
        arr[j] = new int[n];
    }

    return arr;
}

/**
 * @brief Transpose a matrix by returning a new dynamically allocated m x n matrix
 *
 * @param n The number of columns in the original array
 * @param m The number of rows in the original array
 * @param arr The original array to transpose
 *
 * @return A dynamically allocated m x n array which is the transpose of the original array
 */
int **ImageCarver::transposeMatrix(const int &n, const int &m, int **arr)
{
    int **newArr = allocateDynamic2DArray(m, n);

    for (auto i = 0; i < n; ++i)
    {
        for (auto j = 0; j < m; ++j)
        {
            newArr[i][j] = arr[j][i];
        }
    }

    return newArr;
}

/**
 * @brief Reads a PGM file into a pgmData struct and a dynamically allocated 2D array
 *
 * @param fileName The name of the PGM file to read in
 * @param data A reference to a pgmData struct that will contain all the header data from the PGM file
 *
 * @return A dynamically allocated 2D array containing the pixel values from the PGM file
 */
int **ImageCarver::readPGM(const string &fileName, pgmData &data)
{
    ifstream image;
    image.open(fileName);

    if (!image)
    {
        quit(1, "Image file could not be opened.");
    }

    // Read in all PGM Header Data
    getline(image, data.version);
    getline(image, data.comment);
    image >> data.columns >> data.rows >> data.maxGrayLevel;

    int **arr = allocateDynamic2DArray(data.columns, data.rows);

    for (auto i = 0; i < data.rows; ++i)
    {
        for (auto j = 0; j < data.columns; ++j)
        {
            image >> arr[i][j];
        }
    }

    return arr;
}

/**
 * @brief Write header data and an array of pixel values to a PGM file
 *
 * @param fileName The name of the PGM file to write to
 * @param data A reference to a pgmData struct that contains all the header data to write to the PGM file
 * @param arr A 2D array containing the pixel values to write to the PGM file
 */
void ImageCarver::writePGM(const string &fileName, pgmData &data, int **arr)
{
    ofstream imageProcessed;
    imageProcessed.open(fileName);

    // Insert all header data at the top of the file
    imageProcessed << data.version << endl;
    imageProcessed << data.comment << endl;
    imageProcessed << data.columns << ' ' << data.rows << endl;
    imageProcessed << data.maxGrayLevel << endl;

    // Insert pixel values
    for (auto i = 0; i < data.rows; ++i)
    {
        for (auto j = 0; j < data.columns; ++j)
        {
            imageProcessed << arr[i][j] << ' ';
        }

        imageProcessed << endl;
    }

    imageProcessed.close();
}

/**
 * Calculate the energy matrix of an n x m pixel array
 *
 * @param n The number of columns in the pixel array
 * @param m The number of rows in the pixel array
 * @param arr The original pixel array
 * @param newArr The energy matrix to be calculated
 */
void ImageCarver::calculateEnergyMatrix(const int &n, const int &m, int **arr, int **newArr)
{
    int curr, above, below, left, right;

    // Loop through rows
    for (auto i = 0; i < m; ++i)
    {
        // Loop through columns
        for (auto j = 0; j < n; ++j)
        {
            curr = arr[i][j];

            if (i == 0)
            {
                above = curr;
                below = arr[i + 1][j];
            }
            else if (i == (m - 1))
            {
                above = arr[i - 1][j];
                below = curr;
            }
            else
            {
                above = arr[i - 1][j];
                below = arr[i + 1][j];
            }

            if (j == 0)
            {
                left = curr;
                right = arr[i][j + 1];
            }
            else if (j == (n - 1))
            {
                left = arr[i][j - 1];
                right = curr;
            }
            else
            {
                left = arr[i][j - 1];
                right = arr[i][j + 1];
            }

            // Equation to determine the average energy of a pixel from the pixels around it
            newArr[i][j] = abs(curr - above) + abs(curr - below) + abs(curr - left) + abs(curr - right);
        }
    }
}

/**
 * Calculate the vertical cumulative energy matrix for a given energy matrix of size n x m
 *
 * @param n The number of columns in the energy matrix
 * @param m The number of rows in the energy matrix
 * @param arr The energy matrix
 * @param newArr The cumulative energy matrix to be calculated
 */
void ImageCarver::verticalCumulativeEnergyMatrix(const int &n, const int &m, int **arr, int **newArr)
{
    int curr, upLeft, upMid, upRight;

    // Loop through rows
    for (auto i = 0; i < m; ++i)
    {
        // Loop through columns
        for (auto j = 0; j < n; ++j)
        {
            curr = arr[i][j];

            // Top row of the energy matrix
            if (i == 0)
            {
                newArr[i][j] = curr;
            }
            else
            {
                // Leftmost column of the energy matrix
                if (j == 0)
                {
                    // No value exists up left; largest value ensures it is never chosen
                    upLeft = 99999999;
                    upMid = newArr[i - 1][j];
                    upRight = newArr[i - 1][j + 1];
                }

                // Rightmost column of the energy matrix
                else if (j == (n - 1))
                {
                    // No value exists up right; largest value ensures it is never chosen
                    upLeft = newArr[i - 1][j - 1];
                    upMid = newArr[i - 1][j];
                    upRight = 99999999;
                }
                // Anywhere else in the energy matrix
                else
                {
                    upLeft = newArr[i - 1][j - 1];
                    upMid = newArr[i - 1][j];
                    upRight = newArr[i - 1][j + 1];
                }

                newArr[i][j] = curr + smallestOfThree(upLeft, upMid, upRight);
            }
        }
    }
}

/**
 * Removes the lowest energy vertical seam in the cumulative energy matrix
 * Note: This function prioritizes ALWAYS picking the leftmost lowest energy seam
 *
 * @param n The number of columns in the image array
 * @param m The number of rows in the image array
 * @param imageArr The original image array
 * @param cEnergyArr The cumulative energy matrix
 */
void ImageCarver::removeVerticalSeam(const int &n, const int &m, int **imageArr, int **cEnergyArr)
{
    // Start with bottom left pixel as lowest energy seam
    int lowestEnergySeam = cEnergyArr[m - 1][0];
    int upLeft, upMid, upRight;
    int index = 0;

    // Find leftmost lowest energy seam in bottom row
    for (auto j = 0; j < n; ++j)
    {
        if (cEnergyArr[m - 1][j] < lowestEnergySeam)
        {
            lowestEnergySeam = cEnergyArr[m - 1][j];
            index = j;
        }
    }

    // Loop through rows from bottom up
    for (auto i = (m - 1); i >= 0; --i)
    {
        // Remove lowest cumulative energy pixel by shifting pixels to its right to the left one
        for (auto j = 0; j < n - index; ++j)
        {
            // Shift pixels in original pixel array to the left to remove current pixel in seam
            if (j < n - index - 1)
            {
                imageArr[i][index + j] = imageArr[i][index + j + 1];
            }
            // Replace pixel with -1 if in rightmost column to avoid out of bounds memory access
            else
            {
                imageArr[i][index + j] = -1;
            }
        }

        // If more rows above most recent pixel removed, move up to next leftmost pixel in seam
        if (i > 0)
        {
            // Index is in furthest left column
            if (index == 0)
            {
                upLeft = 99999999;
                upMid = cEnergyArr[i - 1][index];
                upRight = cEnergyArr[i - 1][index + 1];
            }
            // Index is in furthest right column
            else if (index == (n - 1))
            {
                upLeft = cEnergyArr[i - 1][index - 1];
                upMid = cEnergyArr[i - 1][index];
                upRight = 99999999;
            }
            // Index is anywhere else in matrix
            else
            {
                upLeft = cEnergyArr[i - 1][index - 1];
                upMid = cEnergyArr[i - 1][index];
                upRight = cEnergyArr[i - 1][index + 1];
            }

            // Determine lowest next pixel prioritizing leftmost pixel
            lowestEnergySeam = smallestOfThree(upLeft, upMid, upRight);

            if (lowestEnergySeam == upLeft)
            {
                index--;
            }
            else if (lowestEnergySeam == upMid)
            {
                continue;
            }
            else
            {
                index++;
            }
        }
    }
}