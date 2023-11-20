#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;

void overlay(unsigned char* src, unsigned char* dst, int posX, int posY, int w, int h, int DW) {
	dst += posX + posY * DW;
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			dst[j * DW + i] = src[j * w + i];
		}
	}
}

void print_Histogram(Mat src, Mat dst) {
	imshow("img_Ori", src);
	imshow("Histogram", dst);
}

void Histogram(Mat src, Mat dst) {
	Mat color_histo;
	if (src.type() == CV_8UC1) { // GRAY일 경우
		int hst[256] = { 0, };
		double his_rate = 0;
		color_histo = Mat(256, 256, CV_8UC1);
//		color_equal = Mat(256, 256, CV_8UC1);
		memset(color_histo.data, 0, color_histo.cols * color_histo.rows);

		for (int h = 0; h < src.cols; h++) {
			for (int w = 0; w < src.rows; w++) {
				hst[src.at<uchar>(h, w)]++;
			}
		}
		int max_his = 0, sum = 0;
		for (int i = 0; i < 256; i++) {
			max_his = max(max_his, hst[i]);
		}

		his_rate = (double)256 / max_his;
		memset(dst.data, 0, dst.cols * dst.rows);

		for (int h = 0; h < 256; h++) {
			for (int w = 0; w < his_rate * hst[h]; w++) {
				dst.at<uchar>((256 - w - 1), h) = 255;
			}
		}
		overlay(dst.data, color_histo.data, 0, 0, dst.cols, dst.rows, color_histo.cols);
	}

	else if (src.type() == CV_8UC3) { // RGB, YUV일 경우
		int hst[3][256] = { 0, };
		int scale[3][256] = { 0, };
		double his_rate = 0;
		color_histo = Mat(256, 256 * 3, CV_8UC1);
		memset(color_histo.data, 0, color_histo.cols * color_histo.rows);

		for (int h = 0; h < src.rows; h++) {
			for (int w = 0; w < src.cols; w++) {
				hst[2][src.at<Vec3b>(h, w)[2]]++;
				hst[1][src.at<Vec3b>(h, w)[1]]++;
				hst[0][src.at<Vec3b>(h, w)[0]]++;
			}
		}
		int max_his[3] = { 0, }, sum[3] = { 0, };
		for (int i = 0; i < 256; i++) {
			max_his[0] = max(max_his[0], hst[0][i]);
			max_his[1] = max(max_his[1], hst[1][i]);
			max_his[2] = max(max_his[2], hst[2][i]);
		}
		for (int i = 0; i < 3; i++) {
			his_rate = (double)256 / max_his[i];
			memset(dst.data, 0, dst.cols * dst.rows);

			for (int h = 0; h < 256; h++) {
				for (int w = 0; w < his_rate * hst[i][h]; w++) {
					dst.at<uchar>((256 - w - 1), h) = 255;
				}
			}
			// R, G, B 히스토그램 하나로 모아서 출력
			overlay(dst.data, color_histo.data, i * 256, 0, dst.cols, dst.rows, color_histo.cols);
		}
	}
	print_Histogram(src,color_histo);
}

void Equal(Mat src, Mat dst, Mat his) { // histogram equalization
	Mat color_equal;
	if (src.type() == CV_8UC1) { // GRAY일 경우
		int scale[256] = { 0, };
		int hst[256] = { 0, };
		double his_rate = 0;
		color_equal = Mat(256, 256, CV_8UC1);
		memset(color_equal.data, 0, color_equal.cols * color_equal.rows);

		for (int h = 0; h < src.cols; h++) {
			for (int w = 0; w < src.rows; w++) {
				hst[src.at<uchar>(h, w)]++;
			}
		}
		int sum = 0;
		for (int i = 0; i < 256; i++) {
			sum += hst[i];
			scale[i] = ((float)sum / (src.cols * src.rows)) * 255;
		}

		for (int i = 0; i < dst.rows; i++) {
			for (int j = 0; j < dst.cols; j++) {
				dst.data[i * dst.cols + j] = scale[dst.data[i * dst.cols + j]];
			}
		}
	}
	else if (src.type() == CV_8UC3) { // RGB, YUV일 경우
		int hst[256][3] = { 0, };
		int scale[256][3] = { 0, };
		double his_rate = 0;
		color_equal = Mat(256, 256 * 3, CV_8UC1);
		memset(color_equal.data, 0, color_equal.cols * color_equal.rows);

		for (int h = 0; h < src.rows; h++) {
			for (int w = 0; w < src.cols; w++) {
				hst[src.at<Vec3b>(h, w)[2]][2]++;
				hst[src.at<Vec3b>(h, w)[1]][1]++;
				hst[src.at<Vec3b>(h, w)[0]][0]++;
			}
		}
		int sum[3] = { 0, };
		for (int i = 0; i < 256; i++) {
			for (int j = 0; j < 3; j++) {
				sum[j] += hst[i][j];
				scale[i][j] = ((float)sum[j] / (src.cols * src.rows)) * 255;
			}
		}
		for (int i = 0; i < dst.rows; i++) {
			for (int j = 0; j < dst.cols; j++) {
				dst.at<Vec3b>(i, j)[0] = scale[dst.at<Vec3b>(i, j)[0]][0];
				dst.at<Vec3b>(i, j)[1] = scale[dst.at<Vec3b>(i, j)[1]][1];
				dst.at<Vec3b>(i, j)[2] = scale[dst.at<Vec3b>(i, j)[2]][2];
			}
		}
	}
	Histogram(dst, his);
}

int main()
{
	Mat img = imread("Lena.jpg");
	Mat equal_RGB = img.clone();
	Mat img_gray = img.clone();
	cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
	Mat equal_gray = img_gray.clone();
	Mat Histo = Mat(img.rows, img.cols, CV_8UC1);
	memset(Histo.data, 0, Histo.rows * Histo.cols);

	Histogram(img_gray, Histo);
//	Histogram(img, Histo);

//	Equal(img_gray, equal_gray, Histo);
//	Equal(img, equal_RGB, Histo);

	waitKey(0);
	return 0;
}