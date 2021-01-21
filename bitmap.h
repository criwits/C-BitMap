#ifndef BITMAP_H
#define BITMAP_H

#include "matrix.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
	//unsigned short bfType;
	unsigned long bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
	unsigned long biSize;
	long biWidth;
	long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	long biXPelsPerMeter;
	long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
	unsigned char bdB;
	unsigned char bdG;
	unsigned char bdR;
} BITMAPDATA;

typedef struct
{
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	BITMAPDATA** bitmapData;
} BITMAP;

_Bool bitmapCheck(FILE* pFile);
BITMAP bitmapReadFromFile(FILE* pFile);
void bitmapWriteToFile(BITMAP bitmapBMP, FILE* pFile);
BITMAP bitmapConvolute(BITMAP bitmapBMP, MATRIX convKernel);

#endif
