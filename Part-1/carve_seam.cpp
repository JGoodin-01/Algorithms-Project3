/*
    carve_seam.cpp
*/

#include "ImageCarver.hpp"

#include <iostream>
#include <fstream>

int main()
{
    ImageCarver carverClass("bug.pgm");
    carverClass.carve(10, 0);
    // carverClass.printImage();
}
