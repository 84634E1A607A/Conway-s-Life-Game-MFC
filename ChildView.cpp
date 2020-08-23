
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "Life-MFC.h"
#include "ChildView.h"
#include "map.h"
#include "DlgOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
//	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
//	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
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

#ifdef DEBUG
	clock_t ts = clock();
#endif // DEBUG


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
	//mdc.BitBlt(0, 0, CliRect.right, CliRect.bottom, &dc, 0, 0, SRCCOPY);

	mdc.SelectObject(hBlackBrush);
	mdc.SelectObject(hLinePen);

	/*  init   */
	mdc.FillSolidRect(&CliRect, RGB(255,255,255));

	/*  lines  */
	for (int i = 0; i <= CliRect.right / side_length; i++) {
		mdc.MoveTo(i * side_length, 0);
		mdc.LineTo(i * side_length, CliRect.bottom);
	}
	for (int i = 0; i <= CliRect.bottom / side_length; i++) {
		mdc.MoveTo(0, i * side_length);
		mdc.LineTo(CliRect.right, i * side_length);
	}

	/*  blocks  */
	map.draw(mdc, CliRect);

	dc.BitBlt(0, 0, CliRect.right, CliRect.bottom, &mdc, 0, 0, SRCCOPY);
	mdc.DeleteDC();
	bmp.DeleteObject();

#ifdef DEBUG
	clock_t te = clock();
	clock_t t = te - ts;
	TCHAR s[16];
	_itow_s(t, s, 10);
	if (theDlg.GetSafeHwnd())
		theDlg.SetDlgItemText(IDC_HEADPOOL_SIZE, s);
#endif // DEBUG
}

//BOOL CChildView::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: Add your message handler code here and/or call default
//	needErase = true;
//	return CWnd::OnEraseBkgnd(pDC);
//}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int xc = point.x / side_length, yc = point.y / side_length;
	map.change(xc + xpivot, yc + ypivot, (!mi.isClick && !mi.isPen) ? 2 : 0);
	RECT crect = { xc * side_length + 1, yc * side_length + 1, (xc + 1) * side_length, (yc + 1) * side_length };
	if (ad.adstate) {
		switch (ad.count) {
		case 0: {
			ad.p1.x = xc;
			ad.p1.y = yc;
			ad.count++;
			break;
		}
		case 1: {
			ad.p2.x = xc;
			ad.p2.y = yc;
			CPoint p1, p2;
			p1.x = min(ad.p1.x, ad.p2.x);
			p1.y = min(ad.p1.y, ad.p2.y);
			p2.x = max(ad.p1.x, ad.p2.x);
			p2.y = max(ad.p1.y, ad.p2.y);
			RECT adrect = { (LONG)(p1.x + xpivot), (LONG)(p1.y + ypivot), (LONG)(p2.x + xpivot), (LONG)(p2.y + ypivot) };
			map.add_delete_region(adrect, ad.act, ad.isrand);
			crect = { p1.x * side_length + 1, p1.y * side_length + 1, (p2.x + 1) * side_length, (p2.y + 1) * side_length };
			ad.count = 0;
			ad.adstate = false;
			break;
		}
		default: {
			break;
		}
		}
	}
	else{
		mi.pprev = { (LONG)(xc + xpivot), (LONG)(yc + ypivot) };
	}
	RedrawWindow(&crect, 0, RDW_INVALIDATE);
	CWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int xc = point.x / side_length, yc = point.y / side_length;
	map.add_builtin(xc + xpivot, yc + ypivot);
	//RECT crect;
	//if (selected_direction < 0);
	//else if (selected_direction < 4)
	//	crect = { xc * side_length + 1, yc * side_length + 1, (xc + map.get_builtin_info().right) * side_length, (yc + map.get_builtin_info().bottom) * side_length };
	//else if (selected_direction < 8)
	//	crect = { xc * side_length + 1, yc * side_length + 1, (xc + map.get_builtin_info().bottom) * side_length, (yc + map.get_builtin_info().right) * side_length };
	//RedrawWindow(&crect, 0, RDW_INVALIDATE);
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)    //bug:skip points when the cursor moves too fast
{
	if ((nFlags & MK_LBUTTON) && !mi.isClick) {
		int xc = point.x / side_length, yc = point.y / side_length;
		CPoint pcur = { (LONG)(xc + xpivot),(LONG)(yc + ypivot) };
		if (pcur != mi.pprev) {
			mi.pprev = pcur;
			map.change(pcur.x, pcur.y, mi.isPen ? 1 : 2);
			RECT crect = { xc * side_length + 1, yc * side_length + 1, (xc + 1) * side_length, (yc + 1) * side_length };
			RedrawWindow(&crect, 0, RDW_INVALIDATE);
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}


//void CChildView::OnTimer(UINT_PTR nIDEvent)
//{
//	if (started) {
//		map.calc();
//		RECT rect;
//		GetClientRect(&rect);
//		RedrawWindow(&rect, 0, RDW_INVALIDATE);
//	}
//}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case VK_LEFT: {
		xpivot -= move_length / side_length;
		RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
		change_xpivot();
		break;
	}
	case VK_RIGHT: {
		xpivot += move_length / side_length;
		RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
		change_xpivot();
		break;
	}
	case VK_UP: {
		ypivot -= move_length / side_length;
		RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
		change_ypivot();
		break;
	}
	case VK_DOWN: {
		ypivot += move_length / side_length;
		RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
		change_ypivot();
		break;
	}
	case VK_SPACE: {
		started = !started;
		if (!started) map.free_extra();
		break;
	}
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
		_itow_s(i, str, 10);
		pScale->SetWindowText(str);
	}
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
