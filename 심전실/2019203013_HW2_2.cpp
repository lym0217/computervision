#define _CRT_SECURE_NO_WARNINGS
#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void main()
{
	Mat img = imread("Lena.jpg");
	unsigned char** GrayImg;
	unsigned char** U;
	unsigned char** V;
	FILE* fp1 = fopen("lena.yuv", "wb");

	GrayImg = new unsigned char* [img.rows];
	U = new unsigned char* [img.rows];
	V = new unsigned char* [img.rows];

	for (int i = 0; i < img.rows; i++) {
		GrayImg[i] = new unsigned char[img.cols];
		U[i] = new unsigned char[img.cols];
		V[i] = new unsigned char[img.cols];
	}
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			GrayImg[i][j] = (0.299 * img.at<Vec3b>(i, j)[2]) + (0.587 * img.at<Vec3b>(i, j)[1]) + (0.114 * img.at<Vec3b>(i, j)[0]);
			U[i][j] = (0.499 * img.at<Vec3b>(i, j)[0]) - (0.169 * img.at<Vec3b>(i, j)[2]) + (0.331 * img.at<Vec3b>(i, j)[1]);
			V[i][j] = (0.499 * img.at<Vec3b>(i, j)[2]) - (0.418 * img.at<Vec3b>(i, j)[1]) - (0.081 * img.at<Vec3b>(i, j)[0]);
		}
	}
	for (int i = 0; i < img.rows; i++) fwrite(GrayImg[i], sizeof(unsigned char), img.cols, fp1);
	for (int i = 0; i < img.rows; i++) fwrite(U[i], sizeof(unsigned char), img.cols, fp1);
	for (int i = 0; i < img.rows; i++) fwrite(V[i], sizeof(unsigned char), img.cols, fp1);

	fclose(fp1);

	FILE* fp3 = fopen("lena.rgb", "wb");
	unsigned char** R;
	unsigned char** G;
	unsigned char** B;

	R = new unsigned char* [img.rows];
	G = new unsigned char* [img.rows];
	B = new unsigned char* [img.rows];

	for (int i = 0; i < img.rows; i++) {
		R[i] = new unsigned char[img.cols];
		G[i] = new unsigned char[img.cols];
		B[i] = new unsigned char[img.cols];
	}
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			R[i][j] = GrayImg[i][j] + (1.402 * V[i][j]);
			G[i][j] = GrayImg[i][j] - (0.344 * U[i][j]) - (0.714 * V[i][j]);
			B[i][j] = GrayImg[i][j] + (1.772 * U[i][j]);
		}
	}
	for (int i = 0; i < img.rows; i++) fwrite(R[i], sizeof(unsigned char), img.cols, fp3);
	for (int i = 0; i < img.rows; i++) fwrite(G[i], sizeof(unsigned char), img.cols, fp3);
	for (int i = 0; i < img.rows; i++) fwrite(B[i], sizeof(unsigned char), img.cols, fp3);

	fclose(fp3);
	waitKey(0);
}