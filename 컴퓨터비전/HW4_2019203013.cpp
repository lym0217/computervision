#include<opencv2/opencv.hpp>
#include<queue>

using namespace cv;
using namespace std;

void salt_pepper(Mat src, Mat dst, double noise_rate) {
	int pixel_num = (int)(src.rows * src.cols * src.channels() * noise_rate);

	for (int i = 0; i < pixel_num; i++) {
		int r_row = rand() % src.rows;
		int r_col = rand() % src.cols;
		int r_ch = rand() % src.channels();
		
		dst.data[r_row * dst.cols * dst.channels() + r_col * dst.channels() + r_ch] = (rand() % 2 == 1) ? 255 : 0;
	}
}

void mean_filter(Mat src, Mat dst, float filter[][3], int f_size) {
	int p_size = f_size / 2;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			for (int k = 0; k < src.channels(); k++) {
				float tmp = 0;
				for (int h = 0; h < f_size; h++) {
					for (int w = 0; w < f_size; w++) {
						if (i + h - p_size < 0 || i + h - p_size >= src.rows || j + w - p_size < 0 || j + w - p_size >= src.cols) continue;
						tmp += filter[h][w] * src.data[(i + h - p_size) * src.cols * src.channels() + (j + w - p_size) * src.channels() + k];
					}
				}
				dst.data[i * dst.cols * dst.channels() + j * dst.channels() + k] = tmp / (f_size * f_size);
			}
		}
	}
}

void median_filter(Mat src, Mat dst) {
	priority_queue<int> pixel;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			for (int k = 0; k < src.channels(); k++) {
				for (int h = -1; h <= 1; h++) {
					for (int w = -1; w <=1; w++) {
						if (i + h < 0 || i + h >= src.rows || j + w < 0 || j + w >= src.cols) pixel.push(0);
						else pixel.push(src.data[(i + h) * src.cols * src.channels() + (j + w) * src.channels() + k]);
					}
				}
				for (int l = 0; l < 9; l++) {
					if (l == 4) dst.data[i * dst.cols * dst.channels() + j * dst.channels() + k] = pixel.top();
					pixel.pop();
				}
			}
		}
	}
}

int main()
{
	Mat img = imread("Lena512.jpg");
	Mat G_noise1 = Mat(img.cols, img.rows, CV_8UC3);
	Mat G_noise2 = Mat(img.cols, img.rows, CV_8UC3);
	Mat SP_noise1 = img.clone();
	Mat SP_noise2 = img.clone();
	
	// gausian noise
	randn(G_noise1, 0, 30);
	addWeighted(img, 1, G_noise1, 1, 0, G_noise1);
	randn(G_noise2, 0, 80);
	addWeighted(img, 1, G_noise2, 1, 0, G_noise2);
//	imshow("G_noise1",G_noise1);
//	imshow("G_noise2", G_noise2);

	// salt and pepper noise
	salt_pepper(SP_noise1, SP_noise1, 0.05);
	cvtColor(SP_noise2, SP_noise2, COLOR_RGB2GRAY);
	salt_pepper(SP_noise2, SP_noise2, 0.1);
//	imshow("SP_noise1", SP_noise1);
	imshow("SP_noise2", SP_noise2);

	Mat rst_img = Mat(img.size(), CV_8UC3);
	/*
	//mean filter
	float Mean_filter[][3] = { {1,1,1},{1,1,1},{1,1,1} };
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols * rst_img.channels());
	mean_filter(G_noise1, rst_img, Mean_filter, 3);
	imwrite("G1_Mean.jpg", rst_img);
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols * rst_img.channels());
	mean_filter(G_noise2, rst_img, Mean_filter, 3);
	imwrite("G2_Mean.jpg", rst_img);
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols * rst_img.channels());
	mean_filter(SP_noise1, rst_img, Mean_filter, 3);
	imwrite("SP1_Mean.jpg", rst_img);
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols * rst_img.channels());
	mean_filter(SP_noise2, rst_img, Mean_filter, 3);
	imwrite("SP2_Mean.jpg", rst_img);
	
	//median filter

	memset(rst_img.data, 0, rst_img.rows * rst_img.cols * rst_img.channels());
	median_filter(G_noise1, rst_img);
	imwrite("G1_Median.jpg", rst_img);
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols * rst_img.channels());
	median_filter(G_noise2, rst_img);
	imwrite("G2_Median.jpg", rst_img);
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols * rst_img.channels());
	median_filter(SP_noise1, rst_img);
	imwrite("SP1_Median.jpg", rst_img);
	memset(rst_img.data, 0, rst_img.rows * rst_img.cols * rst_img.channels());
	median_filter(SP_noise2, rst_img);
	imwrite("SP2_Median.jpg", rst_img);
	
	
	*/


	waitKey(0);
	return 0;
}