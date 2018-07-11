#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

void CreateAlpha(Mat &mat)
{
	for (int i = 0; i < mat.rows;++i)
	{
		for (int j = 0; j < mat.cols;++j)
		{
			Vec4b &bgra = mat.at<Vec4b>(i, j);
			bgra[0] = UCHAR_MAX;
			bgra[1] = saturate_cast<uchar>((float(mat.cols - j)) / ((float)mat.cols) *UCHAR_MAX);	
			bgra[2] = saturate_cast<uchar>(float(mat.rows - i) / ((float)mat.rows) *UCHAR_MAX);
			bgra[3] = saturate_cast<uchar>(0.5*(bgra[1] + bgra[2]));
		}
	}
}

int main()
{
	Mat src(480, 640, CV_8UC4);	
	CreateAlpha(src);
	Vector<int>compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);	
		
	try
	{
		imwrite("alpha.png", src);
		imshow("alpha", src);
		waitKey(0);
	}
	catch (std::runtime_error& ex)
	{
		fprintf(stderr, "imag converse failed %s\n", ex.what());
		return -1;
	}
	return 0;
}