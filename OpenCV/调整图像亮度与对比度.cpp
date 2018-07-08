#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
int main()
{
	Mat src;
	Mat imageOut;
	src = imread("1.jpg");
	if (!src.data)
	{
		std::cout << "Load Pic Error" << std::endl;
		return -1;
	}
	cvtColor(src, src, CV_BGR2GRAY);
	imshow("image", src);

	int row = src.rows;
	int col = src.cols;

	Mat out;
	src.convertTo(out, CV_32F);

	imageOut = Mat::zeros(src.size(), src.type());
	float beta = 30;
	float alpha = 1.2;

	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			if (src.channels() == 3)
			{
				float b = out.at<Vec3f>(i, j)[0];
				float g = out.at<Vec3f>(i, j)[1];
				float r = out.at<Vec3f>(i, j)[2];

				imageOut.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(b * alpha + beta);
				imageOut.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(g * alpha + beta);
				imageOut.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(r * alpha + beta);
			}
			else if (src.channels() == 1)
			{
				int v = src.at<uchar>(i, j);
				imageOut.at<uchar>(i, j) = saturate_cast<uchar>(v * alpha + beta);
			}
		}
	}
	imshow("out image", imageOut);
	waitKey(0);
	return 0;
}