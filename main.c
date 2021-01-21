#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//#pragma pack(2)

#include "bitmap.h"
#include "matrix.h"
#include <stdio.h>

int main(void)
{
	FILE* fileInput;
	fileInput = fopen("test.bmp", "rb");
	
	FILE* fileOutput;
	fileOutput = fopen("test_out.bmp", "wb");

	FILE* convKernel;
	convKernel = fopen("kernel.mat", "r");

	unsigned short bfType;

	fread(&bfType, sizeof(bfType), 1, fileInput);
	BITMAP bitmap;
	bitmap.bitmapData = NULL;
	bitmap = bitmapReadFromFile(fileInput);

	int kernelHeight, kernelWidth;
	fscanf(convKernel, "%d%*c%d%*c", &kernelHeight, &kernelWidth);
	MATRIX convKernetMatrix = matCreate(kernelHeight, kernelWidth);
	for (int i = 0; i < kernelHeight; i++)
	{
		for (int j = 0; j < kernelWidth; j++)
		{
			fscanf(convKernel, "%f" , &convKernetMatrix.matData[i][j]);
			fscanf(convKernel, "%*c");
		}
	}

	BITMAP bitmapOutput;
	bitmapOutput.bitmapData = NULL;
	bitmapOutput = bitmapConvolute(bitmap, convKernetMatrix);

	fwrite(&bfType, sizeof(bfType), 1, fileOutput);
	bitmapWriteToFile(bitmapOutput, fileOutput);

	fclose(fileInput);
	fclose(fileOutput);
	fclose(convKernel);

	return 0;
}