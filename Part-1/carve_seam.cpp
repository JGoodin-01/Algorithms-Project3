/*
    carve_seam.cpp
*/

#include "ImageCarver.hpp"

#include <iostream>
#include <fstream>

int main()
{
    ImageCarver carverClass("bug.pgm");
    carverClass.carve(1, 0);
    carverClass.displayVector();
}
