
// Life-MFC.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Life-MFC.h"
#include "MainFrm.h"
#include "DlgOptions.h"
#include "Map.h"
#include "CHelpDlg.h"
#include "CAboutDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLifeMFCApp

BEGIN_MESSAGE_MAP(CLifeMFCApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CLifeMFCApp::OnAppAbout)
	ON_COMMAND(ID_VIEW_OPTIONS, &CLifeMFCApp::OnViewOptions)
	ON_COMMAND(ID_OPTIONS_OPEN, &CLifeMFCApp::OnOptionsOpen)
	ON_COMMAND(ID_OPTIONS_SAVE, &CLifeMFCApp::OnOptionsSave)
	ON_COMMAND(ID_HELP_HELP, &CLifeMFCApp::OnHelpHelp)
END_MESSAGE_MAP()

// CLifeMFCApp construction

CLifeMFCApp::CLifeMFCApp() noexcept
{
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	// SetAppID(_T("LifeMFC.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CLifeMFCApp object

CLifeMFCApp theApp;
extern DlgOptions theDlg;

// CLifeMFCApp initialization

BOOL CLifeMFCApp::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	// SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW, nullptr,
		nullptr);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	theDlg.Create(IDD_OPTIONS);
	pFrame->SetFocus();
	//theDlg.ShowWindow(SW_SHOW);

	return TRUE;
}

int CLifeMFCApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	return CWinApp::ExitInstance();
}

// CLifeMFCApp message handlers





CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// App command to run the dialog
void CLifeMFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CLifeMFCApp message handlers


void CLifeMFCApp::OnViewOptions()
{
	theDlg.ShowWindow(SW_SHOWNOACTIVATE);
}


void CLifeMFCApp::OnOptionsOpen()
{
	CFileDialog fOpenDlg(TRUE, L"lf", L"", OFN_HIDEREADONLY, L"Life Files (*.lf)|*.lf||", m_pMainWnd);
	if (!fOpenDlg.DoModal()) return;
	CString fname = fOpenDlg.GetPathName();
	if (!fname.GetLength()) return;
	char fnameA[256] = "";
	wsprintfA(fnameA, "%ws", fname.GetString());
	map.load(fnameA);
}


void CLifeMFCApp::OnOptionsSave()
{
	CFileDialog fSaveDlg(FALSE, L"lf", L"", OFN_OVERWRITEPROMPT, L"Life Files (*.lf)|*.lf||", m_pMainWnd);
	if (!fSaveDlg.DoModal()) return;
	CString fname = fSaveDlg.GetPathName();
	if (!fname.GetLength()) return;
	char fnameA[256] = "";
	wsprintfA(fnameA, "%ws", fname.GetString());
	map.dump(fnameA);
}

void CLifeMFCApp::OnHelpHelp()
{
	CHelpDlg helpdlg;
	helpdlg.DoModal();
}
