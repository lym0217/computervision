#include<opencv2/opencv.hpp>

using namespace cv;

void filter(Mat src, Mat dst, float* ft, int f_size) {
	int p_size = f_size / 2;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			int tmp = 0;
			for (int h = 0; h < f_size; h++) {
				for (int w = 0; w < f_size; w++) {
					if (i + h - p_size < 0 || i + h - p_size >= src.rows || j + w - p_size < 0 || j + w - p_size >= src.cols) continue;
					tmp += ft[h*f_size+w] * src.data[(i + h - p_size) * src.cols + (j + w - p_size)];
				}
			}
//			tmp /= (f_size * f_size);
			dst.data[i * dst.cols + j] = (tmp > 0 ? tmp : -tmp);
		}
	}
}

void Edge(Mat src, Mat dst, float* gx, float* gy, int f_size) {
	int p_size = f_size / 2;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			int sumx = 0, sumy = 0;
			for (int h = 0; h < f_size; h++) {
				for (int w = 0; w < f_size; w++) {
					if (i + h - p_size < 0 || i + h - p_size >= src.rows || j + w - p_size < 0 || j + w - p_size >= src.cols) continue;
					sumx += gx[h * f_size + w] * src.data[(i + h - p_size) * src.cols + (j + w - p_size)];
					sumy += gy[h * f_size + w] * src.data[(i + h - p_size) * src.cols + (j + w - p_size)];
				}
			}
//			sumx /= (f_size * f_size);
//			sumy /= (f_size * f_size);
			dst.data[i * dst.cols + j] = (sumx > 0 ? sumx : -sumx) + (sumy > 0 ? sumy : -sumy);
		}
	}
}

void thresholding(Mat src, Mat dst, int T){
	for (int i = 0; i < src.rows * src.cols; i++) {
		if (src.data[i] < T) dst.data[i] = 0;
		else dst.data[i] = 255;
	}
}

void main()
{
	Mat img = imread("Lena512.jpg");
	cvtColor(img, img, COLOR_RGB2GRAY);
	
//	imshow("gray", img);

	Mat rst_img = img.clone();
	
	float gx[9] = {-1,0,1,-2,0,2,-1,0,1};
	float gy[9] = {-1,-2,-1,0,0,0,1,2,1};
	
	// gx display
	filter(img, rst_img, gx, 3);
//	imshow("gx", rst_img);
	
	// gy display
	filter(img, rst_img, gy, 3);
//	imshow("gy", rst_img);

	Edge(img, rst_img, gx, gy, 3);
	imshow("result", rst_img);

	// Thresholding 값 찾기
	int h[256] = { 0, };
	Mat hist = Mat(256, 256, CV_8UC1);
	memset(hist.data, 0, hist.rows * hist.cols);
	int max = 0;

	for (int i = 0; i < rst_img.rows * rst_img.cols; i++) h[rst_img.data[i]]++;
	for (int i = 0; i < 256; i++) {
		if (max < h[i]) max = h[i];
	}
	for (int i = 0; i < 256; i++) {
		h[i] = h[i] * 256 / max;
		for (int j = 0; j < h[i]; j++) {
			hist.data[256 * (255 - j) + i] = 255;
		}
	}
//	imshow("hist", hist);

	int T = 50;
	thresholding(rst_img, rst_img, T);
//	imshow("thresholding", rst_img);

	//smoothing filter 적용 & Edge 추출
	Mat G_img1, G_img2;
	GaussianBlur(img, G_img1, Size(3, 3), 1, 0);
	Edge(G_img1, rst_img, gx, gy, 3);
	thresholding(rst_img, rst_img, T);
	imshow("G_Edge_1", rst_img);
	GaussianBlur(img, G_img2, Size(7, 7), 1, 0);
	Edge(G_img2, rst_img, gx, gy, 3);
	thresholding(rst_img, rst_img, T);
	imshow("G_Edge_2", rst_img);
	
	waitKey(0);
}