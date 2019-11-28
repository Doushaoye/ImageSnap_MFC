#pragma once
#include "stdafx.h"
class DoubleBufferSys
{
public:
	DoubleBufferSys();
	~DoubleBufferSys();
	void Resize(int width, int height);
	void SetCDC(CDC * pDC);
	CDC& GetMemDC();
	void Present();
private:
	CDC MemDC; //���ȶ���һ����ʾ�豸����  
	CBitmap MemBitmap;//����һ��λͼ����  
	CDC * pDC;
	int width;
	int height;

};