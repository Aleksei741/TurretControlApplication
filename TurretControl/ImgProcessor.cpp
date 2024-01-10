//******************************************************************************
//include
//******************************************************************************
#include "main.h"
#include "ImgProcessor.h"
#include <chrono>
#include <stdlib.h>

//#include <opencv2/core.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>
//******************************************************************************
// Секция определения переменных, используемых в модуле
//******************************************************************************
//------------------------------------------------------------------------------
// Глобальные
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Локальные
//------------------------------------------------------------------------------
HBITMAP hBitmap;
unsigned char* PixelData = NULL;
unsigned long PixelDataSize;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
void AimDrawing(HDC hCompatibleDC, UINT bmHeight, UINT bmWidth);
//******************************************************************************
// Секция описания функций
//******************************************************************************
void ImgBufferSet(unsigned char* RGB, int height, int wight, int linesize)
{
	unsigned long cnt;
	HDC dc;
	BITMAPINFO i;
	VOID* pvBits;
	int line;
	int index;
	int PixelIndex;

	if(hBitmap)
		DeleteObject(hBitmap);
	if(PixelData) 
		GlobalFree((HGLOBAL)PixelData);

	PixelDataSize = height * wight * 4;
	PixelData = (LPBYTE)GlobalAlloc(GMEM_FIXED, PixelDataSize);

	for (line = 0; line < height; line++)
	{
		for (cnt = 0; cnt < wight; cnt++)
		{
			PixelIndex = (height - line - 1) * wight + cnt;
			
			index = line * linesize + cnt * 3;

			if (param.HealPoint <= 0 && param.DamageOption.VideoOff)
			{
				PixelData[PixelIndex * 4 + 0] = 0; //Blue
				PixelData[PixelIndex * 4 + 1] = 0;	//Green
				PixelData[PixelIndex * 4 + 2] = 100;	//Red
			}
			else
			{
				PixelData[PixelIndex * 4 + 0] = RGB[index + 2]; //Blue
				PixelData[PixelIndex * 4 + 1] = RGB[index + 1];	//Green
				PixelData[PixelIndex * 4 + 2] = RGB[index];	//Red
			}
		}
	}

	dc = CreateCompatibleDC(NULL);


	ZeroMemory(&i.bmiHeader, sizeof(BITMAPINFOHEADER));
	i.bmiHeader.biWidth = wight;     // Set size you need
	i.bmiHeader.biHeight = height;    // Set size you need
	i.bmiHeader.biPlanes = 1;
	i.bmiHeader.biBitCount = 32; // Can be 8, 16, 32 bpp or 
	// other number
	i.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		
	hBitmap = CreateDIBSection(dc,
		&i,
		DIB_RGB_COLORS,
		&pvBits,
		NULL,
		0);
	// You don't have to use 'pvBits', later DeleteObject(hbmp)
	// will free this bit array.
	SetDIBits(NULL, hBitmap, 0, i.bmiHeader.biWidth, PixelData, &i, DIB_RGB_COLORS);

	DeleteDC(dc);
	RedrawWindow(hwndWideo, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
}

void DrawImgCamera(HWND hWnd, HDC hDC)
{
	float proportion;
	BITMAP Bitmap;
	RECT Rect, r;
	HDC hCompatibleDC;
	HANDLE hOldBitmap;
	
	if (!hBitmap) return;

	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);

	hCompatibleDC = CreateCompatibleDC(hDC);
	hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
	
	
	
	GetClientRect(hWnd, &Rect);
	if (Bitmap.bmWidth > 0 && Bitmap.bmHeight > 0)
	{
		AimDrawing(hCompatibleDC, Bitmap.bmHeight, Bitmap.bmWidth);
		
		//Расчет пользовательского окна
		proportion = (float)Bitmap.bmWidth / (float)Bitmap.bmHeight;
		if (Rect.bottom * proportion < Rect.right)
		{
			r.left = Rect.right / 2 - Rect.bottom * proportion / 2;
			r.top = 0;
			r.right = Rect.right / 2 + Rect.bottom * proportion / 2;
			r.bottom = Rect.bottom;
		}
		else
		{
			r.left = 0;
			r.top = Rect.bottom / 2 - Rect.right / proportion / 2;
			r.right = Rect.right;
			r.bottom = Rect.bottom / 2 + Rect.right / proportion / 2;
		}

		StretchBlt(hDC, r.left, r.top, r.right - r.left, r.bottom - r.top, hCompatibleDC, 0, 0, Bitmap.bmWidth,
			Bitmap.bmHeight, SRCCOPY);
	}
	
	SelectObject(hCompatibleDC, hOldBitmap);

	
	DeleteObject(hBitmap);
	DeleteDC(hCompatibleDC);
}

void AimDrawing(HDC hCompatibleDC, UINT bmHeight, UINT bmWidth)
{
	HPEN hPen; //создаём перо
	UINT Xleft, Xright;
	UINT Ybot;
	int cnt;

	HFONT h_font;
	h_font = CreateFont(param.AimOption.TextWidthPix, param.AimOption.TextHeightPix, 0, 0,
		FW_NORMAL, 0,
		0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

	SetTextColor(hCompatibleDC, RGB(0, 0, 0));
	SetBkMode(hCompatibleDC, TRANSPARENT);
	SelectObject(hCompatibleDC, h_font);
		
	hPen = CreatePen(PS_SOLID, param.AimOption.LineWidthPix, RGB(0, 0, 0));
	SelectObject(hCompatibleDC, hPen);

	Xleft = param.AimOption.X - param.AimOption.width / 2;
	Xright = param.AimOption.X + param.AimOption.width / 2;
	Ybot = param.AimOption.Y + param.AimOption.height;

	MoveToEx(hCompatibleDC, Xleft, param.AimOption.Y, NULL);
	LineTo(hCompatibleDC, Xright, param.AimOption.Y);

	MoveToEx(hCompatibleDC, param.AimOption.X, param.AimOption.Y, NULL);
	LineTo(hCompatibleDC, param.AimOption.X, Ybot);

	for (cnt = 0; cnt < sizeof(param.AimOption.AimText) / sizeof(param.AimOption.AimText[0]); cnt++)
	{
		if (param.AimOption.AimText[cnt].Active)
		{
			Xleft = param.AimOption.X - param.AimOption.AimText[cnt].LenLine / 2;
			Xright = param.AimOption.X + param.AimOption.AimText[cnt].LenLine / 2;

			MoveToEx(hCompatibleDC, Xleft, param.AimOption.Y + param.AimOption.AimText[cnt].Offset, NULL);
			LineTo(hCompatibleDC, Xright, param.AimOption.Y + param.AimOption.AimText[cnt].Offset);

			TextOut(hCompatibleDC, Xright + 5, param.AimOption.Y + param.AimOption.AimText[cnt].Offset - param.AimOption.TextWidthPix / 2, param.AimOption.AimText[cnt].Text, lstrlen(param.AimOption.AimText[cnt].Text));
		}
	}

	DeleteObject(hPen);
}