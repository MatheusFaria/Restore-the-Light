#include "image.h"

#include <fstream>
#include <iostream>

Image::Image(){}
Image::Image(std::string _path): path(_path){}

int Image::loadBMP(){

    Uint8* datBuff[2] = { nullptr, nullptr }; // Header buffers

    pixels = nullptr; // Pixels

    BITMAPFILEHEADER* bmpHeader = nullptr; // Header
    BITMAPINFOHEADER* bmpInfo = nullptr; // Info 

    // The file... We open it with it's constructor
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file)
    {
        std::cout << "Failure to open bitmap file.\n";

        return 1;
    }

    // Allocate byte memory that will hold the two headers
    datBuff[0] = new Uint8[sizeof(BITMAPFILEHEADER)];
    datBuff[1] = new Uint8[sizeof(BITMAPINFOHEADER)];

    file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
    file.read((char*)datBuff[1], sizeof(BITMAPINFOHEADER));

    // Construct the values from the buffers
    bmpHeader = (BITMAPFILEHEADER*)datBuff[0];
    bmpInfo = (BITMAPINFOHEADER*)datBuff[1];

    // Check if the file is an actual BMP file
    if (bmpHeader->bfType != 0x4D42)
    {
        std::cout << "File \"" << path << "\" isn't a bitmap file\n";
        return 2;
    }

    // First allocate pixel memory
    pixels = new Uint8[bmpInfo->biSizeImage];

    // Go to where image data starts, then read in image data
    file.seekg(bmpHeader->bfOffBits);
    file.read((char*)pixels, bmpInfo->biSizeImage);

    // We're almost done. We have our image loaded, however it's not in the right format.
    // .bmp files store image data in the BGR format, and we have to convert it to RGB.
    // Since we have the value in bytes, this shouldn't be to hard to accomplish
    Uint8 tmpRGB = 0; // Swap buffer
    for (unsigned long i = 0; i < bmpInfo->biSizeImage; i += 3)
    {
        tmpRGB = pixels[i];
        pixels[i] = pixels[i + 2];
        pixels[i + 2] = tmpRGB;
    }

    // Set width and height to the values loaded from the file
    width = bmpInfo->biWidth;
    height = bmpInfo->biHeight;

    // Delete the two buffers.
    delete[] datBuff[0];
    delete[] datBuff[1];

    return 0; // Return success code
}
