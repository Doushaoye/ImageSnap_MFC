//  Processing.cpp
//    2010.8.23

#include "stdafx.h"
#include "Processing.h"

//---------------------------------------------------------

LPBITMAPINFO CtreateMapInfo(IplImage* workImg,int flag)
{                                           //  ����λͼ��Ϣ
    BITMAPINFOHEADER BIH={40,1,1,1,8,0,0,0,0,0,0};
	LPBITMAPINFO lpBmi;
	int      wid,hei,bits,colors,i;
  
 	wid =workImg->width;
	hei =workImg->height;
	bits=workImg->depth*workImg->nChannels;

	if (bits>8) colors=0;
	else colors=1<<bits;

	lpBmi=(LPBITMAPINFO) malloc(40+4*colors);
	BIH.biWidth   =wid;
	BIH.biHeight  =hei;
	BIH.biBitCount=(BYTE) bits;
  	memcpy(lpBmi,&BIH,40);                  //  ����λͼ��Ϣͷ

	if (bits==8) {                          //  256 ɫλͼ
		if (flag==1) {                      //  ���ûҽ׵�ɫ��
			for (i=0;i<256;i++) {
				VgaColorTab[i].rgbRed=VgaColorTab[i].rgbGreen=
						VgaColorTab[i].rgbBlue=(BYTE) i;
			}
			memcpy(lpBmi->bmiColors,VgaColorTab,1024);
		}
		else if (flag==2) {                 //  ����Ĭ�ϵ�ɫ��
			memcpy(lpBmi->bmiColors,VgaDefPal,1024);
		}
		else if (flag==3) {                 //  �����Զ����ɫ��
			memcpy(lpBmi->bmiColors,VgaColorTab,1024);
		}
	}
	return(lpBmi);
}

int  ImageType(IplImage* p) 
{
  	int	 i,j,k,bpl,n,pg[256];
	BYTE *buf;
  
	k=p->nChannels;
	if (k==1) {                             //  ����ֵͼ��
        for (i=0;i<256;i++) pg[i]=0;
		buf=(BYTE*)p->imageData;
		bpl=p->widthStep;
        for (i=0;i<p->height;i++) {
			for (j=0;j<p->width;j++) pg[buf[j]]++;
			buf+=bpl;
		}
        for (i=0,n=0;i<256;i++) {
			if (pg[i]) n++;
		}
        if (n==2) k=-1;                     //  ��ֵͼ��
    }
  	return(k);
}

int  ImageClone(IplImage* pi,IplImage** ppo)  //  ���� IplImage λͼ
{
	if (*ppo) {
		cvReleaseImage(ppo);                //  �ͷ�ԭ��λͼ
	}
 	(*ppo) = cvCloneImage(pi);              //  ������λͼ
	return(1);
}

int  ImageReplace(IplImage* pi,IplImage** ppo)  //  λͼ�滻
{
	if (*ppo) 
		cvReleaseImage(ppo);                //  �ͷ�ԭ��λͼ
 	(*ppo) = pi;                            //  λͼ����
	return(1);
}

//---------------------------------------------------------
//  VGA 256ɫĬ�ϵ�ɫ������

RGBQUAD VgaDefPal[256] = {
    {0x00,0x00,0x00,0x00},{0xa8,0x00,0x00,0x00},{0x00,0xa8,0x00,0x00},{0xa8,0xa8,0x00,0x00},
    {0x00,0x00,0xa8,0x00},{0xa8,0x00,0xa8,0x00},{0x00,0x54,0xa8,0x00},{0xa8,0xa8,0xa8,0x00},
    {0x54,0x54,0x54,0x00},{0xfc,0x54,0x54,0x00},{0x54,0xfc,0x54,0x00},{0xfc,0xfc,0x54,0x00},
    {0x54,0x54,0xfc,0x00},{0xfc,0x54,0xfc,0x00},{0x54,0xfc,0xfc,0x00},{0xfc,0xfc,0xfc,0x00},
    {0x00,0x00,0x00,0x00},{0x14,0x14,0x14,0x00},{0x20,0x20,0x20,0x00},{0x2c,0x2c,0x2c,0x00},
    {0x38,0x38,0x38,0x00},{0x44,0x44,0x44,0x00},{0x50,0x50,0x50,0x00},{0x60,0x60,0x60,0x00},
    {0x70,0x70,0x70,0x00},{0x80,0x80,0x80,0x00},{0x90,0x90,0x90,0x00},{0xa0,0xa0,0xa0,0x00},
    {0xb4,0xb4,0xb4,0x00},{0xc8,0xc8,0xc8,0x00},{0xe0,0xe0,0xe0,0x00},{0xfc,0xfc,0xfc,0x00},
    {0xfc,0x00,0x00,0x00},{0xfc,0x00,0x40,0x00},{0xfc,0x00,0x7c,0x00},{0xfc,0x00,0xbc,0x00},
    {0xfc,0x00,0xfc,0x00},{0xbc,0x00,0xfc,0x00},{0x7c,0x00,0xfc,0x00},{0x40,0x00,0xfc,0x00},
    {0x00,0x00,0xfc,0x00},{0x00,0x40,0xfc,0x00},{0x00,0x7c,0xfc,0x00},{0x00,0xbc,0xfc,0x00},
    {0x00,0xfc,0xfc,0x00},{0x00,0xfc,0xbc,0x00},{0x00,0xfc,0x7c,0x00},{0x00,0xfc,0x40,0x00},
    {0x00,0xfc,0x00,0x00},{0x40,0xfc,0x00,0x00},{0x7c,0xfc,0x00,0x00},{0xbc,0xfc,0x00,0x00},
    {0xfc,0xfc,0x00,0x00},{0xfc,0xbc,0x00,0x00},{0xfc,0x7c,0x00,0x00},{0xfc,0x40,0x00,0x00},
    {0xfc,0x7c,0x7c,0x00},{0xfc,0x7c,0x9c,0x00},{0xfc,0x7c,0xbc,0x00},{0xfc,0x7c,0xdc,0x00},
    {0xfc,0x7c,0xfc,0x00},{0xdc,0x7c,0xfc,0x00},{0xbc,0x7c,0xfc,0x00},{0x9c,0x7c,0xfc,0x00},
    {0x7c,0x7c,0xfc,0x00},{0x7c,0x9c,0xfc,0x00},{0x7c,0xbc,0xfc,0x00},{0x7c,0xdc,0xfc,0x00},
    {0x7c,0xfc,0xfc,0x00},{0x7c,0xfc,0xdc,0x00},{0x7c,0xfc,0xbc,0x00},{0x7c,0xfc,0x9c,0x00},
    {0x7c,0xfc,0x7c,0x00},{0x9c,0xfc,0x7c,0x00},{0xbc,0xfc,0x7c,0x00},{0xdc,0xfc,0x7c,0x00},
    {0xfc,0xfc,0x7c,0x00},{0xfc,0xdc,0x7c,0x00},{0xfc,0xbc,0x7c,0x00},{0xfc,0x9c,0x7c,0x00},
    {0xfc,0xb4,0xb4,0x00},{0xfc,0xb4,0xc4,0x00},{0xfc,0xb4,0xd8,0x00},{0xfc,0xb4,0xe8,0x00},
    {0xfc,0xb4,0xfc,0x00},{0xe8,0xb4,0xfc,0x00},{0xd8,0xb4,0xfc,0x00},{0xc4,0xb4,0xfc,0x00},
    {0xb4,0xb4,0xfc,0x00},{0xb4,0xc4,0xfc,0x00},{0xb4,0xd8,0xfc,0x00},{0xb4,0xe8,0xfc,0x00},
    {0xb4,0xfc,0xfc,0x00},{0xb4,0xfc,0xe8,0x00},{0xb4,0xfc,0xd8,0x00},{0xb4,0xfc,0xc4,0x00},
    {0xb4,0xfc,0xb4,0x00},{0xc4,0xfc,0xb4,0x00},{0xd8,0xfc,0xb4,0x00},{0xe8,0xfc,0xb4,0x00},
    {0xfc,0xfc,0xb4,0x00},{0xfc,0xe8,0xb4,0x00},{0xfc,0xd8,0xb4,0x00},{0xfc,0xc4,0xb4,0x00},
    {0x70,0x00,0x00,0x00},{0x70,0x00,0x1c,0x00},{0x70,0x00,0x38,0x00},{0x70,0x00,0x54,0x00},
    {0x70,0x00,0x70,0x00},{0x54,0x00,0x70,0x00},{0x38,0x00,0x70,0x00},{0x1c,0x00,0x70,0x00},
    {0x00,0x00,0x70,0x00},{0x00,0x1c,0x70,0x00},{0x00,0x38,0x70,0x00},{0x00,0x54,0x70,0x00},
    {0x00,0x70,0x70,0x00},{0x00,0x70,0x54,0x00},{0x00,0x70,0x38,0x00},{0x00,0x70,0x1c,0x00},
    {0x00,0x70,0x00,0x00},{0x1c,0x70,0x00,0x00},{0x38,0x70,0x00,0x00},{0x54,0x70,0x00,0x00},
    {0x70,0x70,0x00,0x00},{0x70,0x54,0x00,0x00},{0x70,0x38,0x00,0x00},{0x70,0x1c,0x00,0x00},
    {0x70,0x38,0x38,0x00},{0x70,0x38,0x44,0x00},{0x70,0x38,0x54,0x00},{0x70,0x38,0x60,0x00},
    {0x70,0x38,0x70,0x00},{0x60,0x38,0x70,0x00},{0x54,0x38,0x70,0x00},{0x44,0x38,0x70,0x00},
    {0x38,0x38,0x70,0x00},{0x38,0x44,0x70,0x00},{0x38,0x54,0x70,0x00},{0x38,0x60,0x70,0x00},
    {0x38,0x70,0x70,0x00},{0x38,0x70,0x60,0x00},{0x38,0x70,0x54,0x00},{0x38,0x70,0x44,0x00},
    {0x38,0x70,0x38,0x00},{0x44,0x70,0x38,0x00},{0x54,0x70,0x38,0x00},{0x60,0x70,0x38,0x00},
    {0x70,0x70,0x38,0x00},{0x70,0x60,0x38,0x00},{0x70,0x54,0x38,0x00},{0x70,0x44,0x38,0x00},
    {0x70,0x50,0x50,0x00},{0x70,0x50,0x58,0x00},{0x70,0x50,0x60,0x00},{0x70,0x50,0x68,0x00},
    {0x70,0x50,0x70,0x00},{0x68,0x50,0x70,0x00},{0x60,0x50,0x70,0x00},{0x58,0x50,0x70,0x00},
    {0x50,0x50,0x70,0x00},{0x50,0x58,0x70,0x00},{0x50,0x60,0x70,0x00},{0x50,0x68,0x70,0x00},
    {0x50,0x70,0x70,0x00},{0x50,0x70,0x68,0x00},{0x50,0x70,0x60,0x00},{0x50,0x70,0x58,0x00},
    {0x50,0x70,0x50,0x00},{0x58,0x70,0x50,0x00},{0x60,0x70,0x50,0x00},{0x68,0x70,0x50,0x00},
    {0x70,0x70,0x50,0x00},{0x70,0x68,0x50,0x00},{0x70,0x60,0x50,0x00},{0x70,0x58,0x50,0x00},
    {0x40,0x00,0x00,0x00},{0x40,0x00,0x10,0x00},{0x40,0x00,0x20,0x00},{0x40,0x00,0x30,0x00},
    {0x40,0x00,0x40,0x00},{0x30,0x00,0x40,0x00},{0x20,0x00,0x40,0x00},{0x10,0x00,0x40,0x00},
    {0x00,0x00,0x40,0x00},{0x00,0x10,0x40,0x00},{0x00,0x20,0x40,0x00},{0x00,0x30,0x40,0x00},
    {0x00,0x40,0x40,0x00},{0x00,0x40,0x30,0x00},{0x00,0x40,0x20,0x00},{0x00,0x40,0x10,0x00},
    {0x00,0x40,0x00,0x00},{0x10,0x40,0x00,0x00},{0x20,0x40,0x00,0x00},{0x30,0x40,0x00,0x00},
    {0x40,0x40,0x00,0x00},{0x40,0x30,0x00,0x00},{0x40,0x20,0x00,0x00},{0x40,0x10,0x00,0x00},
    {0x40,0x20,0x20,0x00},{0x40,0x20,0x28,0x00},{0x40,0x20,0x30,0x00},{0x40,0x20,0x38,0x00},
    {0x40,0x20,0x40,0x00},{0x38,0x20,0x40,0x00},{0x30,0x20,0x40,0x00},{0x28,0x20,0x40,0x00},
    {0x20,0x20,0x40,0x00},{0x20,0x28,0x40,0x00},{0x20,0x30,0x40,0x00},{0x20,0x38,0x40,0x00},
    {0x20,0x40,0x40,0x00},{0x20,0x40,0x38,0x00},{0x20,0x40,0x30,0x00},{0x20,0x40,0x28,0x00},
    {0x20,0x40,0x20,0x00},{0x28,0x40,0x20,0x00},{0x30,0x40,0x20,0x00},{0x38,0x40,0x20,0x00},
    {0x40,0x40,0x20,0x00},{0x40,0x38,0x20,0x00},{0x40,0x30,0x20,0x00},{0x40,0x28,0x20,0x00},
    {0x40,0x2c,0x2c,0x00},{0x40,0x2c,0x30,0x00},{0x40,0x2c,0x34,0x00},{0x40,0x2c,0x3c,0x00},
    {0x40,0x2c,0x40,0x00},{0x3c,0x2c,0x40,0x00},{0x34,0x2c,0x40,0x00},{0x30,0x2c,0x40,0x00},
    {0x2c,0x2c,0x40,0x00},{0x2c,0x30,0x40,0x00},{0x2c,0x34,0x40,0x00},{0x2c,0x3c,0x40,0x00},
    {0x2c,0x40,0x40,0x00},{0x2c,0x40,0x3c,0x00},{0x2c,0x40,0x34,0x00},{0x2c,0x40,0x30,0x00},
    {0x2c,0x40,0x2c,0x00},{0x30,0x40,0x2c,0x00},{0x34,0x40,0x2c,0x00},{0x3c,0x40,0x2c,0x00},
    {0x40,0x40,0x2c,0x00},{0x40,0x3c,0x2c,0x00},{0x40,0x34,0x2c,0x00},{0x40,0x30,0x2c,0x00},
    {0xa8,0xa8,0xa8,0x00},{0x54,0xfc,0xfc,0x00},{0xfc,0x54,0xfc,0x00},{0xfc,0xfc,0x54,0x00},
    {0xfc,0x54,0x54,0x00},{0x54,0xfc,0x54,0x00},{0x54,0x54,0xfc,0x00},{0xfc,0xfc,0xfc,0x00}  
};
RGBQUAD  VgaColorTab[256];
 
//---------------------------------------------------------
//  ����ͼ����

void ImageDwindle(IplImage *pi,IplImage *po,int n,int nCnls)   
{                                           //  ͼ����������С
	int  i,j,k,bpl,bplt;
	char *buf,*buft;

	bpl=pi->widthStep;     bplt=po->widthStep;
	buf=pi->imageData;     buft=po->imageData;
	for (i=0;i<pi->height;i+=n) {
		for (j=0,k=0;j<pi->width*nCnls;k+=nCnls,j+=n*nCnls)
			memcpy(&buft[k],&buf[j],nCnls);   //  ����һ������
		buf+=n*bpl;     buft+=bplt;
	}

}

int  NextColor(int start,int k,int step)    //  ��һ��ɫ��
{
	k++;                                    //  �޸���ɫ��
	if (k==7) k+=2;                         //  ����Ĭ�ϵ�ɫ��7��8����ɫ
	else if (k==15) k+=17;                  //  ����Ĭ�ϵ�ɫ��15~31����ɫ
	if (k>32) k+=step-1;  
	if (k>247) k=start;                     //  ����Ĭ�ϵ�ɫ��248����ɫ����ͷ��ʼ
	return(k);
}
  
void Histog(BYTE *buf,int *pg,int Dx,int Dy)
{                                          //  ͳ��ֱ��ͼ                              
  int  i;
 
  for (i=0;i<256;i++)  pg[i]=0;
  for (i=0;i<Dx*Dy;i++) pg[buf[i]]++;
}

int  BasicGlobalThreshold(int *pg,int start,int end)
{                                           //  ����ȫ����ֵ��
   int  i,t,t1,t2,k1,k2;
   double u,u1,u2;
  
   t=0;     u=0;
   for (i=start;i<end;i++) {
      t+=pg[i];		
	  u+=i*pg[i];
   }
   k2=(int) (u/t);                          //  ����˷�Χ�Ҷȵ�ƽ��ֵ

   do {
      k1=k2;
      t1=0;     u1=0;
      for (i=start;i<=k1;i++) {             //  ����ͻҶ�����ۼӺ�
	     t1+=pg[i];	
		 u1+=i*pg[i];
      }
      t2=t-t1;
      u2=u-u1;
      if (t1) u1=u1/t1;                     //  ����ͻҶ����ƽ��ֵ
      else u1=0;
      if (t2) u2=u2/t2;                     //  ����߻Ҷ����ƽ��ֵ
      else u2=0;
	  k2=(int) ((u1+u2)/2);                 //  �õ��µ���ֵ����ֵ
   }
   while(k1!=k2);                           //  ����δ�ȶ�������

   return(k1);                              //  ������ֵ
}

void Thresholding(BYTE *buf,int Dx,int Dy,int thre,int m)
{                                           //  ʵ�ֻҶȱ任
  BYTE **list;
  int  i,j,T_gray[256];
  
  list=(BYTE**) malloc(Dy*sizeof(BYTE*));   //  �����ά��������
  for(i=0;i<Dy;i++) list[i]=buf+i*Dx;

  for (i=0;i<thre;i++)   T_gray[i]=0;       //  ������ֵ�ĺ�ɫ
  for (i=thre;i<256;i++) T_gray[i]=m;       //  ���ڡ�������ֵ�İ�ɫ

  for (i=0;i<Dy;i++) {                                 
    for (j=0;j<Dx;j++) {
       list[i][j]=(BYTE) T_gray[list[i][j]];      
    }
  }

  free(list);  
}

void GrayColorTransfor(RGBQUAD *pal) 
{
	int     i,j;
  
	for (i=0,j=0;i<64;i++,j+=4) {           //  ���任�����͸���Ԫ��ɫ����
		pal[i].rgbRed  =0;
		pal[i].rgbGreen=(BYTE) j;
		pal[i].rgbBlue =255;
	}
	for (i=64,j=0;i<128;i++,j+=4) {         //  ���任�����͸���Ԫ��ɫ����
		pal[i].rgbRed  =0;
		pal[i].rgbGreen=255;
		pal[i].rgbBlue =(BYTE) (255-j);
	}
	for (i=128,j=0;i<192;i++,j+=4) {        //  ���任�����͸���Ԫ��ɫ����
		pal[i].rgbRed  =(BYTE) j;
		pal[i].rgbGreen=255;
		pal[i].rgbBlue =0;
	}
	for (i=192,j=0;i<256;i++,j+=4) {        //  ���任�����͸���Ԫ��ɫ����
		pal[i].rgbRed  =255;
		pal[i].rgbGreen=(BYTE) (255-j);
		pal[i].rgbBlue =0;
	}
}

 