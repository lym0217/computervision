#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void Filter(Mat src, Mat dst, float** num_filter, int f_size) {
	int p_size = f_size / 2;
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < dst.rows; i++) {
			for (int j = 0; j < dst.cols; j++) {
				float tmp1 = 0, tmp2 = 0, tmp3 = 0;
				for (int h = -p_size; h <= p_size; h++) {
					for (int w = -p_size; w <= p_size; w++) {
						if (h + i < 0 || h + i >= dst.rows || w + j < 0 || w + j >= dst.cols) continue;
						tmp1 += num_filter[h + p_size][w + p_size] * src.data[(i + h) * src.cols * 3 + (j + w) * 3];
						tmp2 += num_filter[h + p_size][w + p_size] * src.data[(i + h) * src.cols * 3 + (j + w) * 3 + 1];
						tmp3 += num_filter[h + p_size][w + p_size] * src.data[(i + h) * src.cols * 3 + (j + w) * 3 + 2];
					}
				}
				dst.data[i * dst.cols * 3 + j * 3] = tmp1 / (f_size * f_size);
				dst.data[i * dst.cols * 3 + j * 3 + 1] = tmp2 / (f_size * f_size);
				dst.data[i * dst.cols * 3 + j * 3 + 2] = tmp3 / (f_size * f_size);
			}
		}
	}
}

void main()
{
	Mat img = imread("Lena.jpg");
	int f_size;
	printf("Kernel size => ");
	cin >> f_size;

	Mat img_filter = Mat(img.rows, img.cols, img.type());

	// Moving Average Filter
	float** Mov_filter;
	Mov_filter = (float **)malloc(sizeof(float*) * f_size);
	for (int i = 0; i < f_size; i++) {
		Mov_filter[i] = (float*)malloc(sizeof(float) * f_size);
	}
	for (int i = 0; i < f_size; i++) {
		for (int j = 0; j < f_size; j++) {
			Mov_filter[i][j] = 1;
		}
	}
	Filter(img, img_filter, Mov_filter, f_size);
	
	Mat img_blur;
	imshow("ori", img);
	GaussianBlur(img, img_blur, Size(3, 3), 0.1, 0);
	imshow("0.1 3x3", img_blur);
	GaussianBlur(img, img_blur, Size(7, 7), 0.1, 0);
	imshow("0.1 7x7", img_blur);
	GaussianBlur(img, img_blur, Size(3, 3), 1, 0);
	imshow("1 3x3", img_blur);
	GaussianBlur(img, img_blur, Size(7, 7), 1, 0);
	imshow("1 7x7", img_blur);
	GaussianBlur(img, img_blur, Size(3, 3), 5, 0);
	imshow("5 3x3", img_blur);
	GaussianBlur(img, img_blur, Size(7, 7), 5, 0);
	imshow("5 7x7", img_blur);


	waitKey(0);
}