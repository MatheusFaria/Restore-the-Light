#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GLIncludes.h"


Image::Image(){}
Image::Image(std::string _path): path(_path){}

/* BMP file loader loads a 24-bit bmp file only */

/*
* getint and getshort are help functions to load the bitmap byte by byte
*/
static unsigned int getint(FILE *fp) {
    int c, c1, c2, c3;

    /*  get 4 bytes */
    c = getc(fp);
    c1 = getc(fp);
    c2 = getc(fp);
    c3 = getc(fp);

    return ((unsigned int)c) +
        (((unsigned int)c1) << 8) +
        (((unsigned int)c2) << 16) +
        (((unsigned int)c3) << 24);
}

static unsigned int getshort(FILE *fp){
    int c, c1;

    /* get 2 bytes*/
    c = getc(fp);
    c1 = getc(fp);

    return ((unsigned int)c) + (((unsigned int)c1) << 8);
}

/*  quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  */

int Image::loadBMP() {
    FILE *file;
    unsigned long size;                 /*  size of the image in bytes. */
    unsigned long i;                    /*  standard counter. */
    unsigned short int planes;          /*  number of planes in image (must be 1)  */
    unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
    char temp;                          /*  used to convert bgr to rgb color. */

    /*  make sure the file is there. */
    if ((file = fopen(path.c_str(), "rb")) == NULL) {
        printf("File Not Found : %s\n", path.c_str());
        return 0;
    }

    /*  seek through the bmp header, up to the width height: */
    fseek(file, 18, SEEK_CUR);

    /*  No 100% errorchecking anymore!!! */

    /*  read the width */    width = getint(file);

    /*  read the height */
    height = getint(file);

    /*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
    size = width * height * 3;

    /*  read the planes */
    planes = getshort(file);
    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", path.c_str(), planes);
        return 0;
    }

    /*  read the bpp */
    bpp = getshort(file);
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", path.c_str(), bpp);
        return 0;
    }

    /*  seek past the rest of the bitmap header. */
    fseek(file, 24, SEEK_CUR);

    /*  read the data.  */
    pixels = (char *)malloc(size);
    if (pixels == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }

    if ((i = fread(pixels, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", path.c_str());
        return 0;
    }

    for (i = 0; i<size; i += 3) { /*  reverse all of the colors. (bgr -> rgb) */
        temp = pixels[i];
        pixels[i] = pixels[i + 2];
        pixels[i + 2] = temp;
    }

    fclose(file); /* Close the file and release the filedes */

    /*  we're done. */
    return 1;
}
