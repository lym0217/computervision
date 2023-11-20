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
	double x_rate = (double)x_size / (double)img_input.cols; // x�� ����
	double y_rate = (double)y_size / (double)img_input.rows; // y�� ����

	for (int i = 0; i < img_input.rows; i++) {
		for (int j = 0; j < img_input.cols; j++) {
			img_output.at<uchar>(i * y_rate, j * x_rate) = img_input.at<uchar>(i, j); //��� ���Ͽ� �Է� ���� ������ ���缭 �Է�
		}
	}

	Mat img_rate = img_output.clone(); // ������ �°� ��Ī ��Ų ���� ������ ã�� ���� ����

	for (int i = 0; i < img_output.rows; i++) {
		for (int j = 0; j < img_output.cols; j++) {
			if (img_rate.at<uchar>(i, j) != 0) { // �ȼ� ���� ����ִ� ��(���� ������ �ȼ� ���� ��� �ִ� �ڸ�)�� ����
				int x_point = j, y_point = i;
				for (int h = i + 1; h < img_output.rows; h++) { // �ش� ���� ���� ����� �ȼ��� ã��
					if (img_rate.at<uchar>(h,j) != 0) {
						y_point = h;
						break;
					}
				}
				for (int w = j + 1; w < img_output.cols; w++) { // �ش� ���� ���� ����� �ȼ��� ã��
					if (img_rate.at<uchar>(i,w) != 0) {
						x_point = w;
						break;
					}
				}
				if (x_point == j) { // bilinear interpolation �Ҷ� �������� �� ���� ���� �� �Ų��ִ� �ڵ�
					for (int h = i; h <= y_point; h++) {
						for (int w = j; w < img_output.cols; w++) {
							img_output.at<uchar>(h, w) = img_output.at<uchar>(h, j);
						}
					}
				}
				else if (y_point == i) { // bilinear interpolation �Ҷ� �ϴ��� �� ���� ���� �� �Ų��ִ� �ڵ�
					for (int h = i; h < img_output.rows; h++) {
						for (int w = j; w <=x_point; w++) {
							img_output.at<uchar>(h, w) = img_output.at<uchar>(i, w);
						}
					}
				}
				else { // bilinear interpolation ���
					for (int h = i; h <= y_point; h++) {
						for (int w = j; w <= x_point; w++) {
							if (img_output.at<uchar>(h, w) == 0) { //�� ������ �̿��� �簢���� �׷��� �� ����ִ� �� ã��
								int q1, q2;
								if (h == i) { //�� ������ �̿��� �簢���� �׷��� �� ���� �� �κ� ����
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
								else if(h == y_point) { //�� ������ �̿��� �簢���� �׷��� �� �Ʒ��� �� �κ� ����
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
								else if (w == j) { //�� ������ �̿��� �簢���� �׷��� �� ���� �� �κ� ����
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
								else if (w == x_point) { //�� ������ �̿��� �簢���� �׷��� �� ������ �� �κ� ����
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
								else { // �� ���̽��� ������ �簢�� ���� �ȼ� ä��� ����
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

