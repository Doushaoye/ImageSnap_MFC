#ifndef PUTTEXT_H_
#define PUTTEXT_H_

#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;

void GetStringSize(HDC hDC, const char* str, int* w, int* h);
void putTextZH(Mat &dst, const char* str, Point org, Scalar color, int fontSize,
	const char *fn = "Arial", BOOL italic = FALSE, BOOL underline = FALSE);

#endif // PUTTEXT_H_