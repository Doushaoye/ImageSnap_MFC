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
	CDC MemDC; //首先定义一个显示设备对象  
	CBitmap MemBitmap;//定义一个位图对象  
	CDC * pDC;
	int width;
	int height;

};