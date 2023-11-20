#include<opencv2/opencv.hpp>
#include<math.h>
using namespace cv;

void Filter(Mat src, Mat dst, Mat check, float num_filter, int f_size) {
	int p_size = f_size / 2;
	int i = p_size, j = p_size;
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			if (check.at<uchar>(i, j) == 255) {
				float tmp1 = 0, tmp2 = 0, tmp3 = 0;
				for (int h = -p_size; h <= p_size; h++) {
					for (int w = -p_size; w <= p_size; w++) {
						if (h + i < 0 || h + i >= dst.rows || w + j < 0 || w + j >= dst.cols) continue;
						tmp1 += num_filter * src.data[(i + h) * src.cols * 3 + (j + w) * 3];
						tmp2 += num_filter * src.data[(i + h) * src.cols * 3 + (j + w) * 3 + 1];
						tmp3 += num_filter * src.data[(i + h) * src.cols * 3 + (j + w) * 3 + 2];
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
	Mat img = imread("image.jpg");
	Mat img_skin = img.clone();
	
	memset(img_skin.data, 0, img_skin.cols * img_skin.rows * 3);
	for(int i=0; i<img.rows; i++){
		for (int j = 0; j < img.cols; j++) {
			if (((img.at<Vec3b>(i, j)[0] >= 125 && img.at<Vec3b>(i, j)[0] <= 185) && (img.at<Vec3b>(i, j)[1] >= 154 && img.at<Vec3b>(i, j)[1] <= 194) && (img.at<Vec3b>(i, j)[2] >= 190 && img.at<Vec3b>(i, j)[2] <= 230)) ||
				((img.at<Vec3b>(i, j)[0] >= 189 && img.at<Vec3b>(i, j)[0] <= 226) && (img.at<Vec3b>(i, j)[1] >= 203 && img.at<Vec3b>(i, j)[1] <= 229) && (img.at<Vec3b>(i, j)[2] >= 232 && img.at<Vec3b>(i, j)[2] <= 255)) ||
				((img.at<Vec3b>(i, j)[0] >= 98 && img.at<Vec3b>(i, j)[0] <= 154) && (img.at<Vec3b>(i, j)[1] >= 124 && img.at<Vec3b>(i, j)[1] <= 164) && (img.at<Vec3b>(i, j)[2] >= 163 && img.at<Vec3b>(i, j)[2] <= 204)) ||
				((img.at<Vec3b>(i, j)[0] >= 65 && img.at<Vec3b>(i, j)[0] <= 110) && (img.at<Vec3b>(i, j)[1] >= 75 && img.at<Vec3b>(i, j)[1] <= 130) && (img.at<Vec3b>(i, j)[2] >= 110 && img.at<Vec3b>(i, j)[2] <= 180)))
			{
				img_skin.at<Vec3b>(i, j)[0] = img.at<Vec3b>(i, j)[0];
				img_skin.at<Vec3b>(i, j)[1] = img.at<Vec3b>(i, j)[1];
				img_skin.at<Vec3b>(i, j)[2] = img.at<Vec3b>(i, j)[2];
			}
		}
	}

	Mat img_black = Mat::zeros(img.rows,img.cols,CV_8UC1);
	for (int i = 0; i < img_black.rows; i++) {
		for (int j = 0; j < img_black.cols; j++) {
			if (img_skin.at<Vec3b>(i, j)[0] == 0 && img_skin.at<Vec3b>(i, j)[1] == 0 && img_skin.at<Vec3b>(i, j)[2] == 0) img_black.at<uchar>(i,j) = 0;
			else img_black.at<uchar>(i, j) = 255;
		}
	}

	Mat img_filter = img_skin.clone();
	memset(img_filter.data, 0, img_filter.rows * img_filter.cols * 3);
	Filter(img, img_filter, img_black, 1, 7);

	Mat img_result = Mat(img.rows,img.cols,CV_8UC3);
	
	for (int i = 0; i < img_black.rows; i++) {
		for (int j = 0; j < img_black.cols; j++) {
			if(img_black.at<uchar>(i,j) == 0){
				img_result.at<Vec3b>(i, j)[0] = img.at<Vec3b>(i, j)[0];
				img_result.at<Vec3b>(i, j)[1] = img.at<Vec3b>(i, j)[1];
				img_result.at<Vec3b>(i, j)[2] = img.at<Vec3b>(i, j)[2];
			}
			else {
				img_result.at<Vec3b>(i, j)[0] = img_filter.at<Vec3b>(i, j)[0];
				img_result.at<Vec3b>(i, j)[1] = img_filter.at<Vec3b>(i, j)[1];
				img_result.at<Vec3b>(i, j)[2] = img_filter.at<Vec3b>(i, j)[2];
			}
		}
	}
	imshow("img_Ori", img);
	imshow("result", img_result);
	imwrite("result.jpg", img_result);
	waitKey(0);
}