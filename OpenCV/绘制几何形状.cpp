#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

void DrawLine(Mat &);
void DrawRect(Mat &);
void DrawEliipse(Mat &);
void DrawCircle(Mat &);
void DrawPoly(Mat &);

int main()
{
	Mat src;
	src = imread("1.jpg");
	if (! src.data)
	{
		std::cout << "Load Image Failed" << std::endl;
		return -1;
	}
	imshow("image", src);

	DrawLine(src);
	DrawRect(src);
	DrawEliipse(src);
	DrawCircle(src);
	DrawPoly(src);

	imshow("image111", src);
	waitKey(0);
	return 0;
}

void DrawLine(Mat &img)
{
	Point p1(10, 10);
	Point p2(200, 300);
	Scalar color(0, 0, 255);
						  //线宽   线型
	line(img, p1, p2, color, 1, CV_AA);
}

void DrawRect(Mat &img)
{
	Rect rect(40, 60, 300, 300);
	Scalar color(255, 255, 0);
	rectangle(img, rect, color,3,CV_AA);
}

void DrawEliipse(Mat &img)
{							//椭圆中心点						//长轴   短轴					旋转角度
	ellipse(img, RotatedRect(Point(img.rows / 2, img.cols / 2), Size2f(img.rows / 4, img.rows / 8), 45), Scalar(255, 255, 0), 2, CV_AA);
}

void DrawCircle(Mat &img)
{
	circle(img, Point(img.cols / 2,img.rows / 2), 60, Scalar(0, 255, 255), 6, CV_AA);

	circle(img, Point(img.rows / 2, img.cols / 2), 60, Scalar(255, 0, 255), 6, CV_AA);
}

void DrawPoly(Mat &img)
{
	Point point[1][5] = {
		Point(10, 10),
		Point(10, 100),
		Point(100, 100),
		Point(100, 10),
		Point(10, 10) 
	};
	const Point* ppts[] = { point[0] };
	int pointNum[] = { 5 };
	fillPoly(img, ppts, pointNum, 1, Scalar(125, 255, 0), 8);
	putText(img, "666", Point(100, 100), CV_FONT_HERSHEY_COMPLEX, 2.0, Scalar(255, 125, 125), 6, CV_AA);
}