#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>

using namespace cv;
using namespace std;

vector<Mat> video;
Mat tempImg;
Point ptOld;
int draw_x, draw_y;
int frame_point = 0;

void onMouse(int evt, int x, int y, int flags, void* param) {
	switch (evt) {
	case EVENT_LBUTTONDOWN:
		ptOld = Point(x, y);
		break;
	case EVENT_LBUTTONUP:
		rectangle(video[frame_point], ptOld, Point(x, y), Scalar(0, 0, 255), 2);
		imshow("video", video[frame_point]);
		tempImg = video[frame_point].clone();
		break;
	case EVENT_MOUSEMOVE:
		if (flags & EVENT_FLAG_LBUTTON) {
			rectangle(tempImg, ptOld, Point(x, y), Scalar(0, 0, 255), 2);
			imshow("video", tempImg);
			tempImg = video[frame_point].clone();
		}
		break;
	default:
		break;
	}
}
void main()
{
	VideoCapture cap("video.mp4");
	Mat img;
	bool play_or_stop = false;

	if (!cap.isOpened()) {
		printf("file can not open!\n");
		return;
	}

	namedWindow("video");

	while (1) {
		cap >> img;
		video.push_back(img.clone());
		if (img.empty()) break;
	}
	int key = 0;
	while (1) {
		if (!play_or_stop) {
			imshow("video", video[frame_point]);
			tempImg = video[frame_point].clone();
			setMouseCallback("video", onMouse);
			key = waitKey(0);
			if (key == 80 || key == 112) frame_point--;
			if (key == 78 || key == 110) frame_point++;
			if (key == 32) play_or_stop = !play_or_stop;
		}
		else {
			imshow("video", video[frame_point]);
			key = waitKey(1000 / 30);
			frame_point++;
			if(key == 32) play_or_stop = !play_or_stop;
		}
	}
}