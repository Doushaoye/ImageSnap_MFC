//  Processing.h
//   2010.8.23

#ifndef		PROCESSING
#define		PROCESSING

//---------------------------------------------------------
//  
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

LPBITMAPINFO CtreateMapInfo(IplImage* workImg,int flag);
 
int  ImageType(IplImage* p);
int  ImageClone(IplImage* pi,IplImage** ppo);  //  复制 IplImage 位图
int  ImageReplace(IplImage* pi,IplImage** ppo);  //  位图替换
 
//---------------------------------------------------------
//  常规图像处理

void ImageDwindle(IplImage *pi,IplImage *po,int n,int nCnls);   
int  NextColor(int start,int k,int step);            //  下一彩色号
void Histog(BYTE *buf,int *pg,int Dx,int Dy);
int  BasicGlobalThreshold(int *pg,int start,int end);
void GrayColorTransfor(RGBQUAD *pal); 

extern RGBQUAD VgaDefPal[256];
extern RGBQUAD VgaColorTab[256];

#endif  //PROCESSING
