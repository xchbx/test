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
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	erode(src, out, element);
	imshow("out", out);
	waitKey(0);
	return 0;
}