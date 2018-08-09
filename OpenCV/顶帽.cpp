#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
int main()
{
	Mat src = imread("1.jpg");
	if (! src.data)
	{
		std::cout << "Load Image Failed" << std::endl;
		return -1;
	}
	imshow("input", src);

	Mat out;
	Mat ker = getStructuringElement(MORPH_RECT, Size(20, 20), Point(-1, -1));
	morphologyEx(src, out, CV_MOP_TOPHAT, ker);
	imshow("out", out);
	waitKey(0);
	return 0;
}