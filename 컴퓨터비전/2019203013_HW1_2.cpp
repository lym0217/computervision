#include <opencv2/opencv.hpp>
#include<iostream>
#include<math.h>

using namespace cv;
using namespace std;

void main()
{
	Mat img_input = imread("Lena.png");
	cvtColor(img_input, img_input, cv::COLOR_RGB2GRAY);
	Mat img_output = Mat::zeros(img_input.cols, img_input.rows, img_input.type());
	int degree;
	const double PI = 3.141592;

	cin >> degree;
	double rad = degree * PI / 180; // 각도를 radian으로 변환
	double y_center = (double)img_input.cols / 2, x_center = (double)img_input.rows / 2;
	int x_point, y_point;

	for (int i = 0; i < img_input.rows; i++) {
		for (int j = 0; j < img_input.cols; j++) {
			//반 시계 방향으로의 회전 좌표를 계산
			x_point = (int)(y_center + ((double)i - x_center) * cos(rad) - ((double)j - y_center) * sin(rad));
			y_point = (int)(x_center + ((double)i - x_center) * sin(rad) + ((double)j - y_center) * cos(rad));
			if (x_point >= 0 && x_point < img_input.cols && y_point >= 0 && y_point < img_input.rows) {
				// screen의 범위를 넘어가면 오류 발생하여 img_input의 범위 내에 해당하는 경우에만 저장
				img_output.at<uchar>(x_point, y_point) = img_input.at<uchar>(i, j);
			}
		}
	}
	
	imshow("output image", img_output);

	waitKey(0);
}
