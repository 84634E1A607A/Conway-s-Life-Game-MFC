
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "Life-MFC.h"
#include "ChildView.h"
#include "map.h"
#include "DlgOptions.h"
#include "CHelpDlg.h"
#include "CAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CWinThread* pCalcThread;
// CChildView

CChildView::CChildView()
{
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

	CPaintDC dc(this); // device context for painting

	static HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH); //CreateSolidBrush(RGB(0x00, 0x00, 0x00));
	static HPEN hLinePen = CreatePen(PS_SOLID, 1, RGB(0xaa, 0xaa, 0xaa));

	CDC mdc;
	mdc.CreateCompatibleDC(&dc);
	CBitmap bmp;
	RECT CliRect;
	GetClientRect(&CliRect);
	bmp.CreateCompatibleBitmap(&dc, CliRect.right, CliRect.bottom);
	mdc.SelectObject(bmp);

	mdc.SelectObject(hBlackBrush);
	mdc.SelectObject(hLinePen);

	/*  init   */
	mdc.FillSolidRect(&CliRect, RGB(255,255,255));

	int mid_x = CliRect.right / 2, mid_y = CliRect.bottom / 2;

	/*  lines  */
	for (int i = mid_x % side_length; i <= CliRect.right; i += side_length)
		mdc.MoveTo(i, 0), mdc.LineTo(i, CliRect.bottom);
	for (int i = mid_y % side_length; i <= CliRect.bottom; i += side_length)
		mdc.MoveTo(0, i), mdc.LineTo(CliRect.right, i);

	/*  blocks  */
	map.draw(mdc, CliRect);

	dc.BitBlt(0, 0, CliRect.right, CliRect.bottom, &mdc, 0, 0, SRCCOPY);
	mdc.DeleteDC();
	bmp.DeleteObject();
	clock_t te = clock();
	clock_t t = te - ts;
	TCHAR s[16];
	_itow_s(t, s, 10);
	if (theDlg.GetSafeHwnd())
		theDlg.SetDlgItemText(IDC_PAINT_TIME, s);
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (mi.state == 3 && !ad.adstate) return CWnd::OnLButtonDown(nFlags, point);
	if (ci.state == CALCINFO::busy) return CWnd::OnLButtonDown(nFlags, point);

	RECT CliRect;
	GetClientRect(&CliRect);
	int mid_x = CliRect.right / 2, mid_y = CliRect.bottom / 2;
	int xc = (int)((point.x - mid_x + 0x1000 * side_length) / side_length - 0x1000 + xpivot), yc = (int)((point.y - mid_y + 0x1000 * side_length) / side_length - 0x1000 + ypivot);
	map.change(xc, yc, (mi.state == 2) ? 2 : 0);
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
	}
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	CWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (ci.state == CALCINFO::busy) return CWnd::OnRButtonDown(nFlags, point);
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
	if (ci.state == CALCINFO::busy) return CWnd::OnRButtonDown(nFlags, point);
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
				change_xpivot(), change_ypivot();
			}
			else {
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
				mi.pprev = pcur;
			}
		}
		else mi.pprev = pcur;
		RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
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
	change_xpivot();
}



void CChildView::OnMoveRight()
{
	xpivot += move_length / side_length;
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	change_xpivot();
}



void CChildView::OnMoveUp()
{
	ypivot -= move_length / side_length;
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	change_ypivot();
}


void CChildView::OnMoveDown()
{
	ypivot += move_length / side_length;
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	change_ypivot();
}


void CChildView::OnStartStop()
{
	pCalcThread->PostThreadMessageW(UM_START, 0, 0);
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
	CWnd::OnLButtonUp(nFlags, point);
}
