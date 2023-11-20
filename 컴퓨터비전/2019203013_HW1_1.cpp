#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>

using namespace cv;
using namespace std;

void main()
{
	Mat img_input = imread("Lena_256x256.png");
	cvtColor(img_input, img_input, cv::COLOR_RGB2GRAY);

	int x_size, y_size;
	cin >> x_size >> y_size;

	Mat img_output = Mat::zeros(x_size,y_size,img_input.type());
	double x_rate = (double)x_size / (double)img_input.cols; // x축 비율
	double y_rate = (double)y_size / (double)img_input.rows; // y축 비율

	for (int i = 0; i < img_input.rows; i++) {
		for (int j = 0; j < img_input.cols; j++) {
			img_output.at<uchar>(i * y_rate, j * x_rate) = img_input.at<uchar>(i, j); //출력 파일에 입력 파일 비율에 맞춰서 입력
		}
	}

	Mat img_rate = img_output.clone(); // 비율에 맞게 대칭 시킨 원본 파일을 찾기 위해 생성

	for (int i = 0; i < img_output.rows; i++) {
		for (int j = 0; j < img_output.cols; j++) {
			if (img_rate.at<uchar>(i, j) != 0) { // 픽셀 값이 들어있는 것(원본 파일의 픽셀 값이 들어 있는 자리)만 선택
				int x_point = j, y_point = i;
				for (int h = i + 1; h < img_output.rows; h++) { // 해당 열의 가장 가까운 픽셀값 찾기
					if (img_rate.at<uchar>(h,j) != 0) {
						y_point = h;
						break;
					}
				}
				for (int w = j + 1; w < img_output.cols; w++) { // 해당 행의 가장 가까운 픽셀값 찾기
					if (img_rate.at<uchar>(i,w) != 0) {
						x_point = w;
						break;
					}
				}
				if (x_point == j) { // bilinear interpolation 할때 오른쪽의 빈 공간 생길 때 매꿔주는 코드
					for (int h = i; h <= y_point; h++) {
						for (int w = j; w < img_output.cols; w++) {
							img_output.at<uchar>(h, w) = img_output.at<uchar>(h, j);
						}
					}
				}
				else if (y_point == i) { // bilinear interpolation 할때 하단의 빈 공간 생길 때 매꿔주는 코드
					for (int h = i; h < img_output.rows; h++) {
						for (int w = j; w <=x_point; w++) {
							img_output.at<uchar>(h, w) = img_output.at<uchar>(i, w);
						}
					}
				}
				else { // bilinear interpolation 계산
					for (int h = i; h <= y_point; h++) {
						for (int w = j; w <= x_point; w++) {
							if (img_output.at<uchar>(h, w) == 0) { //네 지점을 이용한 사각형을 그렸을 때 비어있는 곳 찾기
								int q1, q2;
								if (h == i) { //네 지점을 이용한 사각형을 그렸을 때 위쪽 변 부분 공식
									if (img_output.at<uchar>(i, x_point) > img_output.at<uchar>(i, j)) {
										img_output.at<uchar>(h, w) = (img_output.at<uchar>(i, x_point) - img_output.at<uchar>(i, j)) * ((double)(w - j) / (double)(x_point - j))+ img_output.at<uchar>(i, j);
									}
									else if (img_output.at<uchar>(i, x_point) < img_output.at<uchar>(i, j)) {
										img_output.at<uchar>(h, w) = (img_output.at<uchar>(i, j) - img_output.at<uchar>(i, x_point)) * ((double)(x_point - w) / (double)(x_point - j))+ img_output.at<uchar>(i, x_point);
									}
									else {
										img_output.at<uchar>(h, w) = img_output.at<uchar>(i, j);
									}
								}
								else if(h == y_point) { //네 지점을 이용한 사각형을 그렸을 때 아래쪽 변 부분 공식
									if (img_output.at<uchar>(y_point, x_point) > img_output.at<uchar>(y_point, j)) {
										img_output.at<uchar>(h, w) = (img_output.at<uchar>(y_point, x_point) - img_output.at<uchar>(y_point, j)) * ((double)(w - j) / (double)(x_point - j))+ img_output.at<uchar>(y_point, j);
									}
									else if (img_output.at<uchar>(y_point, x_point) < img_output.at<uchar>(y_point, j)) {
										img_output.at<uchar>(h, w) = (img_output.at<uchar>(y_point, j) - img_output.at<uchar>(y_point, x_point)) * ((double)(x_point - w) / (double)(x_point - j))+ img_output.at<uchar>(y_point, x_point);
									}
									else {
										img_output.at<uchar>(h, w) = img_output.at<uchar>(y_point, x_point);
									}
								}
								else if (w == j) { //네 지점을 이용한 사각형을 그렸을 때 왼쪽 변 부분 공식
									if (img_output.at<uchar>(y_point, j) > img_output.at<uchar>(i, j)) {
										img_output.at<uchar>(h, w) = (img_output.at<uchar>(y_point, j) - img_output.at<uchar>(i, j)) * ((double)(h - i) / (double)(y_point - i))+ img_output.at<uchar>(i, j);
									}
									else if (img_output.at<uchar>(i, x_point) < img_output.at<uchar>(i, j)) {
										img_output.at<uchar>(h, w) = (img_output.at<uchar>(i, j) - img_output.at<uchar>(y_point, j)) * ((double)(y_point - h) / (double)(y_point - i))+ img_output.at<uchar>(y_point, j);
									}
									else {
										img_output.at<uchar>(h, w) = img_output.at<uchar>(i, j);
									}
								}
								else if (w == x_point) { //네 지점을 이용한 사각형을 그렸을 때 오른쪽 변 부분 공식
									if (img_output.at<uchar>(y_point, x_point) > img_output.at<uchar>(i, x_point)) {
										img_output.at<uchar>(h, w) = (img_output.at<uchar>(y_point, x_point) - img_output.at<uchar>(i, x_point)) * ((double)(h - i) / (double)(y_point - i))+ img_output.at<uchar>(i, x_point);
									}
									else if (img_output.at<uchar>(y_point, x_point) < img_output.at<uchar>(y_point, j)) {
										img_output.at<uchar>(h, w) = (img_output.at<uchar>(i, x_point) - img_output.at<uchar>(y_point, x_point)) * ((double)(y_point - h) / (double)(y_point - i))+ img_output.at<uchar>(y_point, x_point);
									}
									else {
										img_output.at<uchar>(h, w) = img_output.at<uchar>(y_point, x_point);
									}
								}
								else { // 위 케이스를 제외한 사각형 안의 픽셀 채우는 공식
									if (img_output.at<uchar>(y_point, x_point) > img_output.at<uchar>(y_point, j)) {
										q2 = (img_output.at<uchar>(y_point, x_point) - img_output.at<uchar>(y_point, j)) * ((double)(w - j) / (double)(x_point - j)) + img_output.at<uchar>(y_point, j);
									}
									else if (img_output.at<uchar>(y_point, x_point) < img_output.at<uchar>(y_point, j)) {
										q2 = (img_output.at<uchar>(y_point, j) - img_output.at<uchar>(y_point, x_point)) * ((double)(x_point - w) / (double)(x_point - j)) + img_output.at<uchar>(y_point, x_point);
									}
									else {
										q2 = img_output.at<uchar>(y_point, x_point);
									}
									if (img_output.at<uchar>(i, x_point) > img_output.at<uchar>(i, j)) {
										q1 = (img_output.at<uchar>(i, x_point) - img_output.at<uchar>(i, j)) * ((double)(w - j) / (double)(x_point - j)) +img_output.at<uchar>(i, j);
									}
									else if (img_output.at<uchar>(i, x_point) < img_output.at<uchar>(i, j)) {
										q1 = (img_output.at<uchar>(i, j) - img_output.at<uchar>(i, x_point)) * ((double)(x_point - w) / (double)(x_point - j)) + img_output.at<uchar>(i, x_point);
									}
									else {
										q1 = img_output.at<uchar>(i, j);
									}
									if (q2 > q1) {
										img_output.at<uchar>(h, w) = (q2 - q1) * ((double)(h - i) / (double)(y_point - i)) + q1;
									}
									else if (q2 < q1) {
										img_output.at<uchar>(h, w) = (q1 - q2) * ((double)(y_point - h) / (double)(y_point - i)) + q2;
									}
									else {
										img_output.at<uchar>(h, w) = q1;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	imshow("output",img_output);
	waitKey(0);
}

