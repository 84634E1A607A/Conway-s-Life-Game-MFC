// CHelpDlg.cpp: 实现文件
//

#include "pch.h"
#include "Life-MFC.h"
#include "CHelpDlg.h"
#include "afxdialogex.h"


// CHelpDlg 对话框

IMPLEMENT_DYNAMIC(CHelpDlg, CDialogEx)

CHelpDlg::CHelpDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HELP, pParent)
{
}

CHelpDlg::~CHelpDlg()
{
}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SITELIST, Sitelist);
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CHelpDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CHelpDlg 消息处理程序


BOOL CHelpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	Sitelist.AddString(L"baike.baidu.com");
	Sitelist.AddString(L"wikipedia.org");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CHelpDlg::OnBnClickedButton1()
{
	CString txt;
	int cursel = Sitelist.GetCurSel();
	if (cursel == -1) cursel = 0;
	Sitelist.GetText(cursel, txt);
	if (txt == L"baike.baidu.com")
		ShellExecute(theApp.m_pMainWnd->m_hWnd, nullptr, L"https://baike.baidu.com/item/%E5%BA%B7%E5%A8%81%E7%94%9F%E5%91%BD%E6%B8%B8%E6%88%8F/22668799", nullptr, nullptr, SW_SHOWDEFAULT);
	else if (txt == L"wikipedia.org") {
		ShellExecute(theApp.m_pMainWnd->m_hWnd, nullptr, L"https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life", nullptr, nullptr, SW_SHOWDEFAULT);
	}
}
