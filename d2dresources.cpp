#include "pch.h"
#include "d2dresources.h"
ID2D1Factory* pD2DFactory = NULL; // Direct2D factory
ID2D1HwndRenderTarget* pRenderTarget = NULL;   // Render target
ID2D1SolidColorBrush* pGrayBrush = NULL;
ID2D1SolidColorBrush* pBlackBrush = NULL;
ID2D1SolidColorBrush* pBkgndPen = NULL;
ID2D1SolidColorBrush* pSelectRectBrush = NULL;
ID2D1SolidColorBrush* pSelectRectPen = NULL;
ID2D1SolidColorBrush* pCopyBrush = NULL;
ID2D1SolidColorBrush* pCopyPen = NULL;
ID2D1PathGeometry* pBkgndGeometry = NULL;
RECT rc; // Render area
HRESULT hr;
bool bRedrawBkgnd = true;