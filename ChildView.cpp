
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "Life-MFC.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "Map.h"
#include "DlgOptions.h"
#include "CHelpDlg.h"
#include "CAboutDlg.h"
#include "Select.h"
#include "d2dresources.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
*/

extern DlgOptions theDlg;

// CChildView


CChildView::CChildView()
{
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	CHECK_HR_NORETURN();
}

CChildView::~CChildView()
{
	
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_FASTER, &CChildView::OnFaster)
	ON_COMMAND(ID_SLOWER, &CChildView::OnSlower)
	ON_COMMAND(ID_CONTROLWND, &CChildView::OnControlwnd)
	ON_COMMAND(ID_MOVE_LEFT, &CChildView::OnMoveLeft)
	ON_COMMAND(ID_MOVE_RIGHT, &CChildView::OnMoveRight)
	ON_COMMAND(ID_MOVE_UP, &CChildView::OnMoveUp)
	ON_COMMAND(ID_MOVE_DOWN, &CChildView::OnMoveDown)
	ON_COMMAND(ID_START_STOP, &CChildView::OnStartStop)
	ON_COMMAND(ID_SWITCH_WINDOW, &CChildView::OnSwitchWindow)
	ON_WM_LBUTTONUP()
//	ON_MESSAGE(UM_SENDDATA, &CChildView::OnUmSenddata)
//ON_WM_DESTROY()
//ON_WM_CREATE()
//ON_WM_SIZE()
ON_WM_SIZE()
ON_COMMAND(ID_COPY, &CChildView::OnCopy)
ON_COMMAND(ID_PASTE, &CChildView::OnPaste)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);
	
	return TRUE;
}

void CChildView::OnPaint() 
{

	extern int side_length;

	clock_t ts = clock();

	hr = CreateRes();
	if (SUCCEEDED(hr))
	{
		////static Gdiplus::SolidBrush BlackBrush(Gdiplus::Color(0, 0, 0));
		////static Gdiplus::SolidBrush WhiteBrush(Gdiplus::Color(255, 255, 255));
		////static Gdiplus::Pen LinePen(Gdiplus::Color(0xaa, 0xaa, 0xaa));
		//static CPen LinePen(PS_SOLID, 1, RGB(0xaa, 0xaa, 0xaa));
		//static CBrush BlackBrush(RGB(255, 255, 255));
		///*static Pen BluePen(Color(0,100,221),2);
		//static SolidBrush TransParentBrush(Color(140, 40, 100, 221));*/

		//CPaintDC dc(this);
		//CDC mdc;
		//mdc.CreateCompatibleDC(&dc);
		//CBitmap bmp;
		//RECT CliRect;
		//GetClientRect(&CliRect);
		//bmp.CreateCompatibleBitmap(&dc, CliRect.right, CliRect.bottom);
		//mdc.SelectObject(bmp);
		//mdc.SelectObject(LinePen);

		///*Graphics graphics(mdc.GetSafeHdc());*/
		///*  init   */
		//mdc.FillSolidRect(&CliRect, RGB(255, 255, 255));

		//int mid_x = CliRect.right / 2, mid_y = CliRect.bottom / 2;

		///*  lines  */
		//for (int i = mid_x % side_length; i <= CliRect.right; i += side_length)
		//	mdc.MoveTo(i, 0), mdc.LineTo(i, CliRect.bottom);
		//for (int i = mid_y % side_length; i <= CliRect.bottom; i += side_length)
		//	mdc.MoveTo(0, i), mdc.LineTo(CliRect.right, i);

		///*  blocks  */
		//map.draw(mdc, CliRect);
		////graphics.FillRectangle(&TransParentBrush, 0, 0, 300, 300);
		////selector.paint_rgn(CliRect, graphics, TransParentBrush, BluePen);
		//dc.BitBlt(0, 0, CliRect.right, CliRect.bottom, &mdc, 0, 0, SRCCOPY);

		pRenderTarget->BeginDraw();
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		pRenderTarget->Clear(D2D1::ColorF(1,1,1));
		
		D2D1_SIZE_F rtSize = pRenderTarget->GetSize();
		/*RECT tmprect;
		HWND hStatusBar = ((CMainFrame*)AfxGetMainWnd())->GetStatusBarHWND();
		GetWindowRect(&tmprect);
		rtSize.height -= (tmprect.bottom - tmprect.top);*/
		int width = static_cast<int>(rtSize.width), height = static_cast<int>(rtSize.height);
		int midx = width / 2, midy = height / 2;

		float lnwidth = 0.05f * side_length;
		for (int i = midx % side_length; i <= width; i += side_length)
		{
			pRenderTarget->DrawLine(D2D1::Point2F(static_cast<float>(i), 0), D2D1::Point2F(static_cast<float>(i), static_cast<float>(height)), pGrayBrush, lnwidth);
		}
		for (int i = midy % side_length; i <= height; i += side_length)
		{
			pRenderTarget->DrawLine(D2D1::Point2F(0, static_cast<float>(i)), D2D1::Point2F(static_cast<float>(width), static_cast<float>(i)), pGrayBrush, lnwidth);
		}
		if (SUCCEEDED(hr)) bRedrawBkgnd = false;
		map.draw(pRenderTarget, width, height, lnwidth);
		if (mi.state == 4)
		{
			RECT rect;
			if (selector.is_active())
			{
				selector.get_current_select(rect);
				pRenderTarget->DrawRectangle(D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom), pSelectRectPen, 3 * lnwidth);
				pRenderTarget->FillRectangle(D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom), pSelectRectBrush);
			}
			else
			{
				selector.get_rgn(rect);
				D2D1_RECT_F paintrect = D2D1::RectF(
					(rect.left - xpivot) * side_length + midx + 1 + lnwidth / 2 - 1,
					(rect.top - ypivot) * side_length + midy + 1 + lnwidth / 2 - 1,
					(rect.right - xpivot + 1) * side_length + midx - lnwidth / 2,
					(rect.bottom - ypivot + 1) * side_length + midy - lnwidth / 2
				);
				pRenderTarget->DrawRectangle(&paintrect, pSelectRectPen, 3 * lnwidth);
				pRenderTarget->FillRectangle(&paintrect, pSelectRectBrush);
			}
		}
		hr = pRenderTarget->EndDraw();
		ValidateRect(NULL); 

	}
	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DeleteRes();
	}
	clock_t te = clock();
	clock_t t = te - ts;
	TCHAR s[16];
	_itow_s(t, s, 10);
	if (theDlg.GetSafeHwnd())
		theDlg.SetDlgItemText(IDC_PAINT_TIME, s);
	
}

HRESULT CChildView::CreateRes()
{
	hr = S_OK;
	if (!pRenderTarget)
	{
		GetWindowRect(&rc);
		hr = pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				AfxGetApp()->GetMainWnd()->GetSafeHwnd(),
				D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
			),
			&pRenderTarget
		);

		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&pBlackBrush
		);

		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(0.3f, 0.3f, 0.3f),
			&pGrayBrush
		);

		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(76.8f, 76.8f, 76.8f),
			&pBkgndPen
		);

		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(0x00 / 256.0f, 0x97 / 256.0f, 0xa7 / 256.0f, 0.5f),
			&pSelectRectBrush
		);

		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(0x1a / 256.0f, 0x23 / 256.0f, 0x7e / 256.0f),
			&pSelectRectPen
		);

		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(0x66, 0xbb, 0x6a, 0.5f),
			&pCopyBrush
		);

		pCopyPen = pSelectRectPen;
	}

	return hr;
}

void CChildView::DeleteRes()
{
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(pGrayBrush);
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(pBkgndPen);
	SAFE_RELEASE(pSelectRectBrush);
	SAFE_RELEASE(pSelectRectPen);
	SAFE_RELEASE(pCopyBrush);
	//SAFE_RELEASE(pCopyPen);
	SAFE_RELEASE(pBkgndGeometry);
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (mi.state == 3 && !ad.adstate) return CWnd::OnLButtonDown(nFlags, point);
	

	RECT CliRect;
	GetClientRect(&CliRect);
	int mid_x = CliRect.right / 2, mid_y = CliRect.bottom / 2;
	int xc = (int)((point.x - mid_x + 0x1000 * side_length) / side_length - 0x1000 + xpivot), yc = (int)((point.y - mid_y + 0x1000 * side_length) / side_length - 0x1000 + ypivot);
	if (mi.state != 4)map.change(xc, yc, (mi.state == 2) ? 2 : 0);
	selector.unselect();
	if (ad.adstate) {
		if (!ad.count) {
			ad.p1.x = xc;
			ad.p1.y = yc;
			ad.count++;
		}
		else{
			CPoint p1, p2;
			p1.x = min(ad.p1.x, xc);
			p1.y = min(ad.p1.y, yc);
			p2.x = max(ad.p1.x, xc);
			p2.y = max(ad.p1.y, yc);
			RECT adrect = { p1.x, p1.y, p2.x, p2.y };
			map.add_delete_region(adrect, ad.act, ad.isrand);
			ad.count = 0;
			ad.adstate = false;
		}
	}
	else{
		mi.pprev = { xc, yc };
		if (mi.state == 4)
		{
			selector.activate(true);
			selector.select_point(point);
			return CWnd::OnLButtonDown(nFlags, point);
		}
	}
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	CWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	RECT CliRect;
	GetClientRect(&CliRect);
	int mid_x = CliRect.right / 2, mid_y = CliRect.bottom / 2;
	int xc = (int)((point.x - mid_x + 0x1000 * side_length) / side_length - 0x1000 + xpivot), yc = (int)((point.y - mid_y + 0x1000 * side_length) / side_length - 0x1000 + ypivot);
	map.add_builtin(xc, yc);
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) && mi.state) {
		RECT CliRect;
		GetClientRect(&CliRect);
		int mid_x = CliRect.right / 2, mid_y = CliRect.bottom / 2;
		int xc = (int)((point.x - mid_x + 0x1000 * side_length) / side_length - 0x1000 + xpivot), yc = (int)((point.y - mid_y + 0x1000 * side_length) / side_length - 0x1000 + ypivot);
		CPoint pcur = { xc, yc };
		if (mi.pprev != CPoint(0, 0))
		{
			if (mi.state == 3) {
				xpivot = mi.pprev.x - xc + xpivot;
				ypivot = mi.pprev.y - yc + ypivot;
				xpivot_need_refresh = true, ypivot_need_refresh = true;
			}
			else {
				if (mi.state == 4)
				{
					selector.select_point(point);
				}
				else
				{
					{
						CPoint& s = (pcur.x <= mi.pprev.x) ? pcur : mi.pprev, & e = (s == pcur) ? mi.pprev : pcur;
						double k = ((double)e.y - s.y) / ((double)e.x - s.x);
						for (int i = s.x; i <= e.x; i++)
							map.change(i, (int)(s.y + ((double)i - s.x) * k), (mi.state == 1) ? 1 : 2);
					}
					{
						CPoint& s = (pcur.y <= mi.pprev.y) ? pcur : mi.pprev, & e = (s == pcur) ? mi.pprev : pcur;
						double k = ((double)e.x - s.x) / ((double)e.y - s.y);
						for (int i = s.y; i <= e.y; i++)
							map.change((int)(s.x + ((double)i - s.y) * k), i, (mi.state == 1) ? 1 : 2);
					}
				}
				mi.pprev = pcur;
			}
		}
		else mi.pprev = pcur;

		RedrawWindow(nullptr, nullptr, RDW_INVALIDATE|RDW_NOERASE);
	}
	CWnd::OnMouseMove(nFlags, point);
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case 'B': {
		kbd_input_state = 1;
		break;
	}
	case 'D': {
		kbd_input_state = 2;
		break;
	}
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
		int idc;
		if (kbd_input_state) idc = kbd_input_state == 1 ? IDC_BUILTIN : IDC_DIRECTION;
		else break;
		WCHAR buf[2] = L"";
		_itow_s((int)(nChar)-'0', buf, 2, 10);
		theDlg.GetDlgItem(idc)->SendMessage(WM_SETTEXT, 0, (LPARAM)buf);
		break;
	}
	default: {
		break;
	}
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CWnd* pScale = theDlg.GetDlgItem(IDC_SCALE);
	WCHAR str[16];
	pScale->GetWindowText(str, 16);
	int i = _wtoi(str);
	if (i == 3 && zDelta < 0);
	else {
		i += zDelta / 40;
		if (i < 3) i = 3;
		if (i > 999) i = 999;
		_itow_s(i, str, 10);
		pScale->SetWindowText(str);
		bRedrawBkgnd = true;
	}
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CChildView::OnFaster()
{
	if (TIMER > 5)TIMER -= 5;
	else TIMER = 1;
	theApp.m_pMainWnd->SetTimer(1, TIMER, NULL);
	theDlg.SetDlgItemInt(IDC_TIMER, TIMER, FALSE);
}


void CChildView::OnSlower()
{
	if (TIMER <= 99994)TIMER += 5;
	else TIMER = 99999;
	theApp.m_pMainWnd->SetTimer(1, TIMER, NULL);
	theDlg.SetDlgItemInt(IDC_TIMER, TIMER, FALSE);
}


void CChildView::OnControlwnd()
{
	bool isvisible = theDlg.IsWindowVisible();
	theDlg.ShowWindow(isvisible ? SW_HIDE : SW_SHOWNOACTIVATE);
}


void CChildView::OnMoveLeft()
{
	xpivot -= move_length / side_length;
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	xpivot_need_refresh = true;
}



void CChildView::OnMoveRight()
{
	xpivot += move_length / side_length;
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	xpivot_need_refresh = true;
}



void CChildView::OnMoveUp()
{
	ypivot -= move_length / side_length;
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	ypivot_need_refresh = true;
}


void CChildView::OnMoveDown()
{
	ypivot += move_length / side_length;
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	ypivot_need_refresh = true;
}


void CChildView::OnStartStop()
{
	started = !started;
	if (!started)
	{
		map.free_extra();
	}
	RECT rect;
	GetClientRect(&rect);
	RedrawWindow(&rect, 0, RDW_INVALIDATE | RDW_UPDATENOW);
}


void CChildView::OnSwitchWindow()
{
	if (theDlg.IsWindowVisible()) 
	{
		HWND focusedHwnd = GetActiveWindow()->GetSafeHwnd(), mwnd = ::AfxGetMainWnd()->GetSafeHwnd();
		if (mwnd != focusedHwnd)
			theApp.m_pMainWnd->SetFocus();
		else theDlg.SetFocus();
	}
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	mi.pprev = { 0, 0 };
	if (mi.state == 4)
	{
		selector.select_point(point);
		RECT rect;
		GetClientRect(&rect);
		selector.convert(rect);
		selector.activate(false);
		redraw();
	}
	CWnd::OnLButtonUp(nFlags, point);
}


//int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CWnd::OnCreate(lpCreateStruct) == -1)
//		return -1;
//	
//	return 0;
//}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (pRenderTarget)
	{
		pRenderTarget->Resize(D2D1::SizeU(cx, cy));
	}
	bRedrawBkgnd = true;
}





void CChildView::OnCopy()
{
	selector.copy();
	redraw();
}


void CChildView::OnPaste()
{
	selector.paste();
	redraw();
}
