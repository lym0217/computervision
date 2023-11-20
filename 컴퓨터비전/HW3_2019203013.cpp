#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void overlay(unsigned char* src, unsigned char* dst, int posX, int posY, int w, int h, int DW) {
	dst += posX + posY * DW;
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			dst[j * DW + i] = src[j * w + i];
		}
	}
}

void zero_padding(Mat img, unsigned char* src, unsigned char* dst, int posX, int posY, int w, int h, int DW) {
	if (img.type() == CV_8UC1) {
		dst += posX + posY * DW;
		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				dst[j * DW + i] = src[j * w + i];
			}
		}
	}
	else if (img.type() == CV_8UC3) {
		dst += posX * 3 + posY * DW * 3;
		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				dst[j * DW * 3 + i * 3] = src[j * w * 3 + i * 3];
				dst[j * DW * 3 + i * 3 + 1] = src[j * w * 3 + i * 3 + 1];
				dst[j * DW * 3 + i * 3 + 2] = src[j * w * 3 + i * 3 + 2];
			}
		}
	}
}

void Filter(Mat src, Mat dst, float filter[][3], int f_size) {
	int p_size = f_size / 2;
	int i = p_size, j = p_size;
	for (int i = p_size; i < p_size + dst.rows; i++) {
		for (int j = p_size; j < p_size + dst.cols; j++) {
			float tmp = 0;
			for (int h = 0; h < f_size; h++) {
				for (int w = 0; w < f_size; w++) {
					tmp += filter[h][w] * src.data[(i + h - p_size) * src.cols + (j + w - p_size)];
				}
			}
			dst.data[(i - p_size) * dst.cols + (j - p_size)] = tmp / (f_size * f_size);
		}
	}
}

void Sharpening_Filter(unsigned char* src, unsigned char* dst, int w, int h) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			dst[i * w + j] += src[i * w + j];
		}
	}
}

void main()
{
	Mat img = imread("image.jpg");
	Mat img_gray;
	cvtColor(img, img_gray, cv::COLOR_RGB2GRAY);
	int n;
	printf("n => ");
	cin >> n;
	Mat img_total = Mat(img_gray.rows * 2, img_gray.cols * 3, CV_8UC1);

	// gray image Padding
	Mat img_gray_p = Mat(img_gray.rows + 2 * n, img_gray.cols + 2 * n, CV_8UC1);
	memset(img_gray_p.data, 0, img_gray_p.cols * img_gray_p.rows);
	zero_padding(img_gray, img_gray.data, img_gray_p.data, n, n, img_gray.cols, img_gray.rows, img_gray_p.cols);
//	imshow("gray_padding", img_gray_p);
	
	// RGB image Padding
	Mat img_rgb_p = Mat(img.rows + 2 * n, img.cols + 2 * n, CV_8UC3);
	memset(img_rgb_p.data, 0, img_rgb_p.cols * img_rgb_p.rows * 3);
	zero_padding(img, img.data, img_rgb_p.data, n, n, img.cols, img.rows, img_rgb_p.cols);
//	imshow("RGB_padding", img_rgb_p);
	
	int f_size = 2 * n + 1;
	Mat filter = Mat(f_size, f_size, CV_32F);

	// Moving Average Filter
	float Mov_filter[][3] = { {1,1,1},{1,1,1},{1,1,1} };
	Mat img_mov = Mat(img_gray.rows, img_gray.cols, CV_8UC1);
	memset(img_mov.data, 0, img_mov.rows * img_mov.cols);
	Filter(img_gray_p, img_mov, Mov_filter, f_size);
	overlay(img_mov.data, img_total.data, 0, 0, img_mov.cols, img_mov.rows, img_total.cols);

	// Laplacian Filter
	float Lap_filter_a[][3] = { {0,1,0},{1,-4,1},{0,1,0} };
	float Lap_filter_b[][3] = { {1,1,1},{1,-8,1},{1,1,1} };
	float Lap_filter_c[][3] = { {0,-1,0},{-1,4,-1},{0,-1,0} };
	float Lap_filter_d[][3] = { {-1,-1,-1},{-1,8,-1},{-1,-1,-1} };
	Mat img_lap = Mat(img_gray.rows, img_gray.cols, CV_8UC1);
	memset(img_lap.data, 0, img_lap.rows * img_lap.cols);

	Mat img_clone = img_gray.clone();
	//Laplacian filter a
	Filter(img_gray_p, img_lap, Lap_filter_a, f_size);
	overlay(img_lap.data, img_total.data, img_gray.cols, 0, img_lap.cols, img_lap.rows, img_total.cols);
	//Laplacian filter b
	Filter(img_gray_p, img_lap, Lap_filter_b, f_size);
	overlay(img_lap.data, img_total.data, img_gray.cols*2, 0, img_lap.cols, img_lap.rows, img_total.cols);
	//Laplacian filter c
	Filter(img_gray_p, img_lap, Lap_filter_c, f_size);
	overlay(img_lap.data, img_total.data, img_gray.cols, img_gray.rows, img_lap.cols, img_lap.rows, img_total.cols);
	//Laplacian filter d
	Filter(img_gray_p, img_lap, Lap_filter_d, f_size);
	overlay(img_lap.data, img_total.data, img_gray.cols*2, img_gray.rows, img_lap.cols, img_lap.rows, img_total.cols);

	//Sharpening Filter
	Sharpening_Filter(img_lap.data, img_clone.data, img_lap.cols, img_lap.rows);
	overlay(img_clone.data, img_total.data, 0, img_gray.rows, img_clone.cols, img_clone.rows, img_total.cols);

	imshow("result", img_total);
	
	waitKey(0);
}