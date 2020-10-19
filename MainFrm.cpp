
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "Life-MFC.h"
#include "Map.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CWinThread* pCalcThread;
extern CALCINFO ci;

// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_COMMAND(ID_CREATE_DELETE, &CMainFrame::OnCreateDelete)
	ON_COMMAND(ID_CREATE_RANDOM, &CMainFrame::OnCreateRandom)
	ON_COMMAND(ID_CREATE_RECTANGLE, &CMainFrame::OnCreateRectangle)
	ON_WM_DROPFILES()
	ON_MESSAGE(UM_SENDDATA, &CMainFrame::OnUmSenddata)
	ON_MESSAGE(UM_CLOSETHREAD, &CMainFrame::OnUmClosethread)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_MAINFRM));
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	SetTimer(1, TIMER, NULL);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
		return TRUE;
	return CFrameWnd::PreTranslateMessage(pMsg);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (started) {
		if (ci.state == CALCINFO::idle) {
			RECT rect;
			GetClientRect(&rect);
			m_wndView.RedrawWindow(&rect, 0, RDW_INVALIDATE|RDW_UPDATENOW);
			ci.state = CALCINFO::busy;
			pCalcThread->PostThreadMessageW(UM_NEEDDATA, 0, 0);
		}
	}
}

void CMainFrame::OnCreateDelete()
{
	ad.act = false;
	ad.adstate = true;
	started = false;
	// TODO: Add your command handler code here
}


void CMainFrame::OnCreateRandom()
{
	ad.act = true;
	ad.adstate = true;
	started = false;
	ad.isrand = true;
	// TODO: Add your command handler code here
}


void CMainFrame::OnCreateRectangle()
{
	ad.act = true;
	ad.adstate = true;
	started = false;
	ad.isrand = false;
	// TODO: Add your command handler code here
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	TCHAR fname[MAX_PATH];
	DragQueryFile(hDropInfo, 0, fname, MAX_PATH);
	map.load(CString(fname));
	DragFinish(hDropInfo);

	CFrameWnd::OnDropFiles(hDropInfo);
}


afx_msg LRESULT CMainFrame::OnUmSenddata(WPARAM wParam, LPARAM lParam)
{
	ci.state = CALCINFO::idle;
	return 0;
}

afx_msg LRESULT CMainFrame::OnUmClosethread(WPARAM wParam, LPARAM lParam)
{
	ci.state = CALCINFO::destroyed;
	return 0;
}


void CMainFrame::OnClose()
{
	ci.state = ci.destroyed;
	DWORD threadcode;
	::GetExitCodeThread(pCalcThread->m_hThread, &threadcode);
	if (threadcode == STILL_ACTIVE)
		pCalcThread->PostThreadMessageW(UM_CLOSETHREAD, 0, (LPARAM)0);
	CFrameWnd::OnClose();
}
