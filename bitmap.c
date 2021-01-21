#include "BitMap.h"
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

_Bool bitmapCheck(FILE* pFile)
{
	unsigned short bitmapType;
	fread(&bitmapType, sizeof(bitmapType), 1, pFile);
	return bitmapType == 0x4D42 ? 1 : 0;
}

void bitmapFree(BITMAP* bitmapBMP)
{
	for (int i = 0; i < bitmapBMP->bitmapInfoHeader.biHeight; i++)
		free(bitmapBMP->bitmapData[i]);
	free(bitmapBMP->bitmapData);
	bitmapBMP->bitmapData = NULL;
}

void bitmapInit(BITMAP* bitmapBMP)
{
	bitmapBMP->bitmapData = (BITMAPDATA**)malloc(bitmapBMP->bitmapInfoHeader.biHeight * sizeof(BITMAPDATA*));
	for (int i = 0; i < bitmapBMP->bitmapInfoHeader.biHeight; i++)
		bitmapBMP->bitmapData[i] = (BITMAPDATA*)malloc(bitmapBMP->bitmapInfoHeader.biWidth * sizeof(BITMAPDATA));
}

BITMAP bitmapReadFromFile(FILE* pFile)
{
	BITMAP bitmapTempBMP;
	unsigned char bitmapOffsetBuffer;
	fread(&bitmapTempBMP.bitmapFileHeader, sizeof(bitmapTempBMP.bitmapFileHeader), 1, pFile);
	fread(&bitmapTempBMP.bitmapInfoHeader, sizeof(bitmapTempBMP.bitmapInfoHeader), 1, pFile);

	bitmapInit(&bitmapTempBMP);

	int bitmapChannels = bitmapTempBMP.bitmapInfoHeader.biBitCount / 8;
	int bitmapLineOffset = bitmapChannels == 4 ? 0 : (bitmapChannels * bitmapTempBMP.bitmapInfoHeader.biWidth) % 4;
	bitmapLineOffset = bitmapLineOffset ? 4 - bitmapLineOffset : 0;
	for (int j = 0; j < bitmapTempBMP.bitmapInfoHeader.biHeight; j++)
	{
		for (int i = 0; i < bitmapTempBMP.bitmapInfoHeader.biWidth; i++)
		{
			fread(&bitmapTempBMP.bitmapData[j][i].bdB, sizeof(unsigned char), 1, pFile);
			fread(&bitmapTempBMP.bitmapData[j][i].bdG, sizeof(unsigned char), 1, pFile);
			fread(&bitmapTempBMP.bitmapData[j][i].bdR, sizeof(unsigned char), 1, pFile);
			for (int k = 0; k < bitmapChannels - 3; k++)
				fread(&bitmapOffsetBuffer, sizeof(bitmapOffsetBuffer), 1, pFile);
		}
		for (int i = 0; i < bitmapLineOffset; i++)
			fread(&bitmapOffsetBuffer, sizeof(bitmapOffsetBuffer), 1, pFile);
	}
	return bitmapTempBMP;
}

void bitmapWriteToFile(const BITMAP bitmapBMP, FILE* pFile)
{
	fwrite(&bitmapBMP.bitmapFileHeader, sizeof(bitmapBMP.bitmapFileHeader), 1, pFile);
	fwrite(&bitmapBMP.bitmapInfoHeader, sizeof(bitmapBMP.bitmapInfoHeader), 1, pFile);
	int bitmapChannels = bitmapBMP.bitmapInfoHeader.biBitCount / 8;
	int bitmapLineOffset = bitmapChannels == 4 ? 0 : (bitmapChannels * bitmapBMP.bitmapInfoHeader.biWidth) % 4;
	bitmapLineOffset = bitmapLineOffset ? 4 - bitmapLineOffset : 0;
	unsigned char bitmapPixelOffset = 0xFF;
	unsigned char bitmapLinesOffset = 0x00;
	for (int j = 0; j < bitmapBMP.bitmapInfoHeader.biHeight; j++)
	{
		for (int i = 0; i < bitmapBMP.bitmapInfoHeader.biWidth; i++)
		{
			fwrite(&bitmapBMP.bitmapData[j][i].bdB, sizeof(unsigned char), 1, pFile);
			fwrite(&bitmapBMP.bitmapData[j][i].bdG, sizeof(unsigned char), 1, pFile);
			fwrite(&bitmapBMP.bitmapData[j][i].bdR, sizeof(unsigned char), 1, pFile);
			for (int k = 0; k < bitmapChannels - 3; k++)
				fwrite(&bitmapPixelOffset, sizeof(bitmapPixelOffset), 1, pFile);
		}
		for (int i = 0; i < bitmapLineOffset; i++)
			fwrite(&bitmapLinesOffset, sizeof(bitmapLinesOffset), 1, pFile);
	}
}

BITMAP bitmapConvolute(BITMAP bitmapBMP, MATRIX convKernel)
{
	BITMAP bitmapConvluted;
	bitmapConvluted.bitmapFileHeader = bitmapBMP.bitmapFileHeader;
	bitmapConvluted.bitmapInfoHeader = bitmapBMP.bitmapInfoHeader;
	bitmapInit(&bitmapConvluted);

	MATRIX matB = matCreate(bitmapBMP.bitmapInfoHeader.biHeight, bitmapBMP.bitmapInfoHeader.biWidth);
	MATRIX matG = matCreate(bitmapBMP.bitmapInfoHeader.biHeight, bitmapBMP.bitmapInfoHeader.biWidth);
	MATRIX matR = matCreate(bitmapBMP.bitmapInfoHeader.biHeight, bitmapBMP.bitmapInfoHeader.biWidth);

	for (int i = 0; i < bitmapBMP.bitmapInfoHeader.biHeight; i++)
		for (int j = 0; j < bitmapBMP.bitmapInfoHeader.biWidth; j++)
		{
			matB.matData[i][j] = bitmapBMP.bitmapData[i][j].bdB;
			matG.matData[i][j] = bitmapBMP.bitmapData[i][j].bdG;
			matR.matData[i][j] = bitmapBMP.bitmapData[i][j].bdR;
		}

	MATRIX matConvB = matConvolute(matB, convKernel);
	MATRIX matConvG = matConvolute(matG, convKernel);
	MATRIX matConvR = matConvolute(matR, convKernel);
	assert(!errno);

	for (int i = 0; i < bitmapBMP.bitmapInfoHeader.biHeight; i++)
		for (int j = 0; j < bitmapBMP.bitmapInfoHeader.biWidth; j++)
		{
			bitmapConvluted.bitmapData[i][j].bdB = (uint8_t)matConvB.matData[i][j];
			bitmapConvluted.bitmapData[i][j].bdG = (uint8_t)matConvG.matData[i][j];
			bitmapConvluted.bitmapData[i][j].bdR = (uint8_t)matConvR.matData[i][j];
		}

	matFree(&matB);
	matFree(&matG);
	matFree(&matR);
	matFree(&matConvB);
	matFree(&matConvG);
	matFree(&matConvR);

	return bitmapConvluted;
}