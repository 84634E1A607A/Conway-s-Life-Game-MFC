#pragma once
#include "framework.h"
extern ID2D1Factory* pD2DFactory ; // Direct2D factory
extern ID2D1HwndRenderTarget* pRenderTarget ;   // Render target
extern ID2D1SolidColorBrush* pGrayBrush ;
extern ID2D1SolidColorBrush* pBlackBrush ;
extern ID2D1SolidColorBrush* pBkgndPen ;
extern ID2D1SolidColorBrush* pSelectRectBrush ;
extern ID2D1SolidColorBrush* pSelectRectPen ;
extern ID2D1SolidColorBrush* pCopyBrush ;
extern ID2D1SolidColorBrush* pCopyPen ;
extern ID2D1PathGeometry* pBkgndGeometry ;
extern RECT rc; // Render area
extern HRESULT hr;
extern bool bRedrawBkgnd;

#define SAFE_RELEASE(P) do{if(P){P->Release() ; P = NULL ;}}while(0)
#define CHECK_HR() do{\
if (FAILED(hr))\
{\
	MessageBox(L"Create failed!");\
	return FALSE;\
}\
}while(0)

#define CHECK_HR_NORETURN() do{\
if (FAILED(hr))\
{\
	MessageBox(L"Create failed!");\
}\
}while(0)