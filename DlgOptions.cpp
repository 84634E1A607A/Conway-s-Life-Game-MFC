// DlgOptions.cpp : implementation file
//

#include "pch.h"
#include "Life-MFC.h"
#include "DlgOptions.h"
#include "afxdialogex.h"
#include "Map.h"
#include "MainFrm.h"
extern Map map;


// DlgOptions dialog
DlgOptions theDlg;

IMPLEMENT_DYNAMIC(DlgOptions, CDialogEx)

DWORD WINAPI draw_first_builtin(LPVOID Dlg) { Sleep(500); map.draw_builtin((CDialog*)Dlg); return 0; }

DlgOptions::DlgOptions(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPTIONS, pParent)
{
}

DlgOptions::~DlgOptions()
{
}

void DlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgOptions, CDialogEx)
	ON_STN_CLICKED(IDC_PREVIEW, &DlgOptions::OnStnClickedPreview)
	ON_EN_CHANGE(IDC_BUILTIN, &DlgOptions::OnChangeBuiltin)
	ON_EN_CHANGE(IDC_DIRECTION, &DlgOptions::OnChangeDirection)
	ON_EN_CHANGE(IDC_TIMER, &DlgOptions::OnChangeTimer)
	ON_EN_CHANGE(IDC_SCALE, &DlgOptions::OnChangeScale)
	ON_BN_CLICKED(IDC_STARTSTOP, &DlgOptions::OnBnClickedStartstop)
	ON_BN_CLICKED(IDC_RESET, &DlgOptions::OnBnClickedReset)
	ON_BN_CLICKED(IDC_OK, &DlgOptions::OnBnClickedOk)
	ON_BN_CLICKED(IDC_UP, &DlgOptions::OnBnClickedUp)
	ON_BN_CLICKED(IDC_DOWN, &DlgOptions::OnBnClickedDown)
	ON_BN_CLICKED(IDC_LEFT, &DlgOptions::OnBnClickedLeft)
	ON_BN_CLICKED(IDC_RIGHT, &DlgOptions::OnBnClickedRight)
END_MESSAGE_MAP()


// DlgOptions message handlers


void DlgOptions::OnStnClickedPreview()
{
	// TODO: Add your control notification handler code here
}


BOOL DlgOptions::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd* pBuiltin = GetDlgItem(IDC_BUILTIN);
	pBuiltin->SendMessage(EM_SETLIMITTEXT, 1, 0);
	pBuiltin->SendMessage(WM_SETTEXT, 0, (LPARAM)L"0");

	CWnd* pDirection = GetDlgItem(IDC_DIRECTION);
	pDirection->SendMessage(EM_SETLIMITTEXT, 1, 0);
	pDirection->SendMessage(WM_SETTEXT, 0, (LPARAM)L"0");

	CWnd* pTimer = GetDlgItem(IDC_TIMER);
	pTimer->SendMessage(EM_SETLIMITTEXT, 5, 0);
	pTimer->SendMessage(WM_SETTEXT, 0, (LPARAM)L"50");

	CWnd* pScale = GetDlgItem(IDC_SCALE);
	pScale->SendMessage(EM_SETLIMITTEXT, 3, 0);
	pScale->SendMessage(WM_SETTEXT, 0, (LPARAM)L"10");

	CWnd* pSetX = GetDlgItem(IDC_SX);
	pSetX->SendMessage(EM_SETLIMITTEXT, 10, 0);
	pSetX->SendMessage(WM_SETTEXT, 0, (LPARAM)L"0x08000000");

	CWnd* pSetY = GetDlgItem(IDC_SY);
	pSetY->SendMessage(EM_SETLIMITTEXT, 10, 0);
	pSetY->SendMessage(WM_SETTEXT, 0, (LPARAM)L"0x08000000");

	GetDlgItem(IDC_HEADPOOL_SIZE)->SendMessage(WM_SETTEXT, 0, (LPARAM)map.get_size());

	GetDlgItem(IDC_NODEPOOL_SIZE)->SendMessage(WM_SETTEXT, 0, (LPARAM)map.get_size());

	CloseHandle(CreateThread(NULL, 0, draw_first_builtin, this, 0, NULL));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void DlgOptions::OnChangeBuiltin()
{
	WCHAR val[16];
	GetDlgItem(IDC_BUILTIN)->GetWindowText(val, 16);
	selected_builtin = _wtoi(val);
	map.draw_builtin(this);
}


void DlgOptions::OnChangeDirection()
{
	WCHAR val[16];
	GetDlgItem(IDC_DIRECTION)->GetWindowText(val, 16);
	selected_direction = _wtoi(val);
	map.draw_builtin(this);
}


void DlgOptions::OnChangeTimer()
{
	WCHAR val[16];
	CWnd* pTimer = GetDlgItem(IDC_TIMER);
	pTimer->GetWindowText(val, 16);
	int v = _wtoi(val);
	if (v <= 0) pTimer->SendMessage(WM_SETTEXT, 0, (LPARAM)L"1");
	else {
		TIMER = v;
		theApp.m_pMainWnd->SetTimer(1, TIMER, NULL);
	}
}


void DlgOptions::OnChangeScale()
{
	WCHAR val[16];
	CWnd* pScale = GetDlgItem(IDC_SCALE);
	pScale->GetWindowText(val, 16);
	int v = _wtoi(val);
	if (v <= 2) pScale->SendMessage(WM_SETTEXT, 0, (LPARAM)L"3");
	else side_length = v, redraw_erase();
}


void DlgOptions::OnBnClickedStartstop()
{
	started = !started;
}


void DlgOptions::OnBnClickedReset()
{
	xpivot = ypivot = 0x08000000;
	started = false;
	map.clear();
}

void DlgOptions::OnBnClickedOk()
{
	WCHAR xstr[16], ystr[16];
	int x = 0, y = 0;
	CWnd* pXPivot = GetDlgItem(IDC_XPIVOT);
	CWnd* pYPivot = GetDlgItem(IDC_YPIVOT);
	CWnd* pSetX = GetDlgItem(IDC_SX);
	CWnd* pSetY = GetDlgItem(IDC_SY);
	pSetX->GetWindowText(xstr, 16);
	pSetY->GetWindowText(ystr, 16);
	swscanf_s(xstr, L"0x%08x", &x);
	swscanf_s(ystr, L"0x%08x", &y);
	swprintf_s(xstr, 16, L"0x%08x", x);
	swprintf_s(ystr, 16, L"0x%08x", y);
	if (x > 0) {
		xpivot = x;
		change_xpivot();
	}
	else {
		GetDlgItemText(IDC_XPIVOT, xstr, 16);
		pSetX->SendMessage(WM_SETTEXT, 0, (LPARAM)xstr);
	}
	if (y > 0) {
		ypivot = y;
		change_ypivot();
	}
	else {
		GetDlgItemText(IDC_YPIVOT, ystr, 16);
		pSetY->SendMessage(WM_SETTEXT, 0, (LPARAM)ystr);
	}
	redraw_erase();
}


void DlgOptions::OnBnClickedUp()
{
	ypivot -= 5 * move_length / side_length;
	redraw_erase();
	change_ypivot();
}


void DlgOptions::OnBnClickedDown()
{
	ypivot += 5 * move_length / side_length;
	redraw_erase();
	change_ypivot();
}


void DlgOptions::OnBnClickedLeft()
{
	xpivot -= 5 * move_length / side_length;
	redraw_erase();
	change_xpivot();
}


void DlgOptions::OnBnClickedRight()
{
	xpivot += 5 * move_length / side_length;
	redraw_erase();
	change_xpivot();
}
