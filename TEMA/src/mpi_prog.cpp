#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <tuple> 
#include<algorithm>
#include <initializer_list>
#include <sys/time.h>
#include "mpi.h"

#define MASTER 0
using namespace std;

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

void ReadImage(const char *fileName, byte **pixels, int32 *width, int32 *height, int32 *bytesPerPixel)
{
        FILE *imageFile = fopen(fileName, "rb");
        if (imageFile == NULL)
        {
            cout << "File not found\n";
            exit(1);
        }
        int32 dataOffset;
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
        fread(&dataOffset, 4, 1, imageFile);
        fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
        fread(width, 4, 1, imageFile);
        fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
        fread(height, 4, 1, imageFile);
        int16 bitsPerPixel;
        fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
        fread(&bitsPerPixel, 2, 1, imageFile);
        *bytesPerPixel = ((int32)bitsPerPixel) / 8;

        int paddedRowSize = (int)(4 * ceil((float)(*width) / 4.0f))*(*bytesPerPixel);
        int unpaddedRowSize = (*width)*(*bytesPerPixel);
        int totalSize = unpaddedRowSize*(*height);
        *pixels = (byte*)malloc(totalSize);
        int i = 0;
        byte *currentRowPointer = *pixels+((*height-1)*unpaddedRowSize);
        for (i = 0; i < *height; i++)
        {
            fseek(imageFile, dataOffset+(i*paddedRowSize), SEEK_SET);
            fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
            currentRowPointer -= unpaddedRowSize;
        }

        fclose(imageFile);
}

void WriteImage(const char *fileName, byte *pixels, int32 width, int32 height,int32 bytesPerPixel)
{
        FILE *outputFile = fopen(fileName, "wb");
        //*****HEADER************//
        const char *BM = "BM";
        fwrite(&BM[0], 1, 1, outputFile);
        fwrite(&BM[1], 1, 1, outputFile);
        int paddedRowSize = (int)(4 * ceil((float)width/4.0f))*bytesPerPixel;
        int32 fileSize = paddedRowSize*height + HEADER_SIZE + INFO_HEADER_SIZE;
        fwrite(&fileSize, 4, 1, outputFile);
        int32 reserved = 0x0000;
        fwrite(&reserved, 4, 1, outputFile);
        int32 dataOffset = HEADER_SIZE+INFO_HEADER_SIZE;
        fwrite(&dataOffset, 4, 1, outputFile);

        //*******INFO*HEADER******//
        int32 infoHeaderSize = INFO_HEADER_SIZE;
        fwrite(&infoHeaderSize, 4, 1, outputFile);
        fwrite(&width, 4, 1, outputFile);
        fwrite(&height, 4, 1, outputFile);
        int16 planes = 1; //always 1
        fwrite(&planes, 2, 1, outputFile);
        int16 bitsPerPixel = bytesPerPixel * 8;
        fwrite(&bitsPerPixel, 2, 1, outputFile);
        //write compression
        int32 compression = NO_COMPRESION;
        fwrite(&compression, 4, 1, outputFile);
        // write image size (in bytes)
        int32 imageSize = width*height*bytesPerPixel;
        fwrite(&imageSize, 4, 1, outputFile);
        int32 resolutionX = 11811; //300 dpi
        int32 resolutionY = 11811; //300 dpi
        fwrite(&resolutionX, 4, 1, outputFile);
        fwrite(&resolutionY, 4, 1, outputFile);
        int32 colorsUsed = MAX_NUMBER_OF_COLORS;
        fwrite(&colorsUsed, 4, 1, outputFile);
        int32 importantColors = ALL_COLORS_REQUIRED;
        fwrite(&importantColors, 4, 1, outputFile);
        int i = 0;
        int unpaddedRowSize = width*bytesPerPixel;
        for ( i = 0; i < height; i++)
        {
            int pixelOffset = ((height - i) - 1)*unpaddedRowSize;
            fwrite(&pixels[pixelOffset], 1, paddedRowSize, outputFile);
        }
        fclose(outputFile);
}

void RGBtoHSL(byte *src){
	float _min, _max, delta;
	float r, g, b, h, s, l;

	r = src[0] / 255.;
	g = src[1] / 255.;
	b = src[2] / 255.;

	_min = std::min({r, g, b});
    _max = std::max({r, g, b});

    h = s = l = 0.0;
    float d = _max - _min;
    l = (_max + _min) / 2.0;

    if(_max == _min){
        h = 0.0;
    } else {

        if (_max == r) h = int((g - b) / d) % 6;
        else if (_max == g) h = (b - r) / d + 2;
        else if (_max == b) h = (r - g) / d + 4;

        h = floor(h * 60);
    }

    if(h < 0)
        h += 360;

    if(d == 0) s = 0;
    else s = d / (1 - abs(2 * l - 1));

    src[0] = (h / 360.) * 255;
    src[1] = int(s * 255);
    src[2] = int(l * 255);
}

void convert_image(int32 width, int32 height, byte **pixels)
{
    for (int i = 0; i < width * height; i++) {
        RGBtoHSL(*pixels + i * 3);
    }
}

int main(int argc, char *argv[])
{
    byte *pixels, *img;
    int32 width, height;
    int32 bytesPerPixel;
    int chunk_size;
    string filename = "input/" + string(argv[1]) + ".bmp";
    string out_filename = "out/" + string(argv[1]) + "_result.bmp";

    struct timeval start, end;

    int nProcesses, rank;
    
    // MPI init
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

    if(rank == MASTER){
        ReadImage(filename.c_str(), &pixels, &width, &height,&bytesPerPixel);
        chunk_size = width * height * 3 / nProcesses;
        gettimeofday(&start, NULL);
    }

    MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    img = (byte*)malloc(chunk_size*sizeof(byte));

    MPI_Scatter(pixels, chunk_size, MPI_CHAR, img, chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    for (int i = 0; i < chunk_size; i+=3) {
        RGBtoHSL(img + i);
    }

    /* Facem gather la datele calculate */
    MPI_Gather(img, chunk_size, MPI_CHAR, pixels, chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    if(rank == MASTER){
        gettimeofday(&end, NULL);
        printf("Duration: %.4f s\n", end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0);
        //WriteImage(out_filename.c_str(), pixels, width, height, bytesPerPixel); 
        free(pixels);
    }

    MPI_Finalize();

    return 0;
}