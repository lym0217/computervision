#include<opencv2/opencv.hpp>
#include<iostream>

#define PI 3.14

using namespace cv;
using namespace std;

void GetGaussianPDF(float* EmptyPDF, int nLength, float fMean, float fStDev) {
	int n;
	int Center = nLength / 2;
	float x;

	for (n = 0; n < nLength; n++) {
		x = (float)(n - Center);
		EmptyPDF[n] = (1 / ((float)sqrt(2 * PI) * fStDev)) * exp(-pow(x - fMean, 2) / (2 * fStDev * fStDev));
	}
}

float GetNoise(float* PDF, int nLength) {
	int n;
	int Center = nLength / 2;
	float fRand, fComp, fTemp = 0;
	float x = 0, fDiff;
	float* CDF = new float[nLength];

	CDF[0] = 0;

	fRand = (float)rand() / (RAND_MAX + 1);

	for (n = 1; n < nLength; n++) {
		CDF[n] = (PDF[n] + PDF[n - 1]) / 2 + CDF[n - 1];
		fDiff = fRand - CDF[n];
		if (fDiff < 0) {
			x = ((float)n - Center);
			break;
		}
	}
	fComp = (fRand - CDF[n - 1]) / (CDF[n] - CDF[n - 1]);
	delete[] CDF;

	return x + fComp;
}

void G(Mat src, Mat dst, float fMean, float fStdev) {
	float fTemp = 0, fPDF[256] = { 0.0f };

	GetGaussianPDF(fPDF, 256, fMean, fStdev);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			fTemp = (float)src.data[i * src.cols + j] + GetNoise(fPDF, 256);
			dst.data[i * dst.cols + j] = static_cast<unsigned char>(fTemp);
		}
	}
}

void SP(Mat src, Mat dst, float fSProb, float fPProb) {
	float Low = fSProb;
	float High = 1.0f - fPProb;
	float fRand;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			fRand = ((float)rand() / RAND_MAX);

			if (fRand < Low) dst.data[i * dst.cols + j] = 255;
			else if (fRand > High) dst.data[i * dst.cols + j] = 0;
			else dst.data[i * dst.cols + j] = src.data[i * src.cols + j];
		}
	}
}

void Geometric_mean_filter(Mat src, Mat dst, int f_size) {
	int p_size = f_size / 2;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			for (int k = 0; k < src.channels(); k++) {
				double tmp = 1.0;
				for (int h = 0; h < f_size; h++) {
					for (int w = 0; w < f_size; w++) {
						if (i + h - p_size < 0 || i + h - p_size >= src.rows || j + w - p_size < 0 || j + w - p_size >= src.cols) continue;
						tmp *= src.data[(i + h - p_size) * src.cols * src.channels() + (j + w - p_size) * src.channels() + k];
					}
				}

				dst.data[i * dst.cols * dst.channels() + j * dst.channels() + k] = pow(tmp, 1.0 / (f_size * f_size));
			}
		}
	}
}

void median_filter(Mat src, Mat dst, int f_size) {
	int p_size = f_size / 2;
	priority_queue<int> pixel;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			for (int k = 0; k < src.channels(); k++) {
				for (int h = -p_size; h <= p_size; h++) {
					for (int w = -p_size; w <= p_size; w++) {
						if (i + h < 0 || i + h >= src.rows || j + w < 0 || j + w >= src.cols) pixel.push(0);
						else pixel.push(src.data[(i + h) * src.cols * src.channels() + (j + w) * src.channels() + k]);
					}
				}
				for (int l = 0; l < f_size * f_size; l++) {
					if (l == f_size*f_size/2) dst.data[i * dst.cols * dst.channels() + j * dst.channels() + k] = pixel.top();
					pixel.pop();
				}
			}
		}
	}
}

void Adap_median_filter(Mat src, Mat dst, int S_init, int S_max) {
	priority_queue<int> pixel;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			for (int k = 0; k < src.channels(); k++) {
				for (int f_size = S_init; f_size <= S_max; f_size+=2) {
					int p_size = f_size / 2;
					for (int h = -p_size; h <= p_size; h++) {
						for (int w = -p_size; w <= p_size; w++) {
							if (i + h < 0 || i + h >= src.rows || j + w < 0 || j + w >= src.cols) pixel.push(0);
							else pixel.push(src.data[(i + h) * src.cols * src.channels() + (j + w) * src.channels() + k]);
						}
					}
					int B1, B2, Z_max, Z_min, Z_med;
					for (int l = 0; l < f_size * f_size; l++) {
						if (l == 0) Z_max = pixel.top();
						else if (l == f_size * f_size / 2) Z_med = pixel.top();
						else if (l == f_size * f_size - 1) Z_min = pixel.top();
						pixel.pop();
					}
					if (f_size == S_max) {
						dst.data[i * dst.cols * dst.channels() + j * dst.channels() + k] = src.data[i * src.cols * src.channels() + j * src.channels() + k];
						break;
					}
					if (Z_med - Z_min > 0 && Z_med - Z_max < 0) {
						if (src.data[i * src.cols * src.channels() + j * src.channels() + k] - Z_min > 0 && src.data[i * src.cols * src.channels() + j * src.channels() + k] - Z_max < 0) dst.data[i * dst.cols * dst.channels() + j * dst.channels() + k] = src.data[i * src.cols * src.channels() + j * src.channels() + k];
						else dst.data[i * dst.cols * dst.channels() + j * dst.channels() + k] = Z_med;
						break;
					}
				}
			}
		}
	}
}

float PSNR(Mat src, Mat dst) {
	int max_pixel = 255;
	long long mse = 0;
	for (int i = 0; i < src.rows*src.cols; i++) {
		mse += (src.data[i] - dst.data[i])* (src.data[i] - dst.data[i]);
	}
	mse /= src.rows * src.cols;
	if (mse == 0) return 100;

	double psnr = 20 * log10(max_pixel / sqrt(mse));
	return psnr;
}

int main()
{
	float val1, val2;
	int n;
	Mat img = imread("Lena512.jpg");
	cvtColor(img, img, COLOR_RGB2GRAY);

	// 문제 #4
	// Gaussian noise 생성
	Mat G_noise = Mat(img.size(), CV_8UC1);
	memset(G_noise.data, 0, G_noise.rows * G_noise.cols);
	
	cout << "mean : ";
	cin >> val1;
	cout << "Std Dev : ";
	cin >> val2;
	G(img, G_noise, val1, val2);
//	imshow("G_noise", G_noise);
//	imwrite("0_30_G_noise.jpg", G_noise);

	// Salt&Pepper noise 생성
	Mat SP_noise = Mat(img.size(), CV_8UC1);
	memset(SP_noise.data, 0, SP_noise.rows * SP_noise.cols);

	cout << "Salt Prob : ";
	cin >> val1;
	cout << "Pepper Prob : ";
	cin >> val2;
	SP(img, SP_noise, val1, val2);
//	imshow("SP_noise", SP_noise);
//	imwrite("25_SP_noise.jpg", SP_noise);

	Mat rst_img = Mat(img.size(), CV_8UC1);
	
	// 문제 #5 Geometric mean filter
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols);
	cout << "mean filter size : ";
	cin >> n;
	Geometric_mean_filter(G_noise, rst_img, n);
//	imshow("Geo_mean", rst_img);

	// 문제 #6 median filter
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols);
	cout << "median filter size : ";
	cin >> n;
	median_filter(SP_noise, rst_img, n);
//	imshow("median filter", rst_img);

	// 문제 #7 adaptive median fliter
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols);
	Adap_median_filter(SP_noise, rst_img, 3, 11);
//	imshow("Adaptive median filter", rst_img);
	
	// 문제 #8 PSNR
	double psnr;
	
	// 문제 #9 통합 테스트
	// Gaussian noise + mean filter 성능평가
	Mat noise_img = imread("0_5_G_noise.jpg");
	Geometric_mean_filter(noise_img, rst_img, 3);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("0_5_3mean", rst_img);
	Geometric_mean_filter(noise_img, rst_img, 5);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("0_5_5mean", rst_img);

	noise_img = imread("0_10_G_noise.jpg");
	Geometric_mean_filter(noise_img, rst_img, 3);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("0_10_3mean", rst_img);
	Geometric_mean_filter(noise_img, rst_img, 5);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("0_10_5mean", rst_img);

	noise_img = imread("0_30_G_noise.jpg");
	Geometric_mean_filter(noise_img, rst_img, 3);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("0_30_3mean", rst_img);
	Geometric_mean_filter(noise_img, rst_img, 5);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("0_30_5mean", rst_img);
	printf("\n");
	// Salt&Pepper noise + median filter 성능평가
	noise_img = imread("5_SP_noise.jpg");
	median_filter(noise_img, rst_img, 3);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("5%_median", rst_img);
	Adap_median_filter(noise_img, rst_img, 3, 7);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("5%_Adapt", rst_img);

	noise_img = imread("10_SP_noise.jpg");
	median_filter(noise_img, rst_img, 3);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("10%_median", rst_img);
	Adap_median_filter(noise_img, rst_img, 3, 7);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("10%_Adapt", rst_img);

	noise_img = imread("25_SP_noise.jpg");
	median_filter(noise_img, rst_img, 3);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("25%_median", rst_img);
	Adap_median_filter(noise_img, rst_img, 3, 7);
	psnr = PSNR(img, rst_img);
	printf("%2.2f\n", psnr);
//	imshow("25%_Adapt", rst_img);

	waitKey(0);
	return 0;
}