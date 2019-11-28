#include "stdafx.h"
#include "DoubleBufferSys.h"
#include <wingdi.h>
#pragma comment(lib,"Msimg32.lib")
DoubleBufferSys::DoubleBufferSys()
{
	MemDC.CreateCompatibleDC(NULL);
}

DoubleBufferSys::~DoubleBufferSys()
{
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}
void DoubleBufferSys::Present()
{
	pDC->BitBlt(0, 0, width, height, &MemDC, 0, 0, SRCCOPY);
}
void DoubleBufferSys::Resize(int _width, int _height)
{
	if (_width <= 0 || _height <= 0)
	{
		return;
	}
	width = _width;
	height = _height;

	MemBitmap.DeleteObject();
	MemBitmap.CreateCompatibleBitmap(pDC, width, height);
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	//TransparentBlt(pDC->m_hDC, 0, 0, width, height, MemDC.m_hDC, 0, 0, width, height, RGB(0xff, 0xff, 0xff));
	//MemDC.SelectObject(pOldBit);
	MemDC.FillSolidRect(0, 0, width, height, RGB(255, 255, 255));
}

void DoubleBufferSys::SetCDC(CDC *_pDC)
{
	pDC = _pDC;
}

CDC& DoubleBufferSys::GetMemDC()
{
	return MemDC;
}