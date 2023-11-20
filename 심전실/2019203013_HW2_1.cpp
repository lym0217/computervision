#define _CRT_SECURE_NO_WARNINGS
#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void main()
{
	Mat img = imread("Lena.jpg"); // size´Â 512 X 512
	unsigned char** GrayImg;
	FILE* fp = fopen("lena.y", "wb");
	
	GrayImg = new unsigned char* [img.rows];

	for (int i = 0; i < img.rows; i++) {
		GrayImg[i] = new unsigned char[img.cols];
	}
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			GrayImg[i][j] = (0.299 * img.at<Vec3b>(i, j)[2]) + (0.587 * img.at<Vec3b>(i, j)[1]) + (0.114 * img.at<Vec3b>(i, j)[0]);
		}
	}

	for (int i = 0; i < img.rows; i++) fwrite(GrayImg[i], sizeof(unsigned char), img.cols, fp);
	fclose(fp);
	waitKey(0);
}