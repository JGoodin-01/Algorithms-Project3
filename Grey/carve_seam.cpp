/*
    carve_seam.cpp
*/

#include "ImageCarver.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    ImageCarver carverClass;
    carverClass.carve(argc, argv);
    return 0;
}
