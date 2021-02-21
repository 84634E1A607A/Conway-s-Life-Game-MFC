
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame() noexcept;
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//CToolBar          m_wndToolBar;
	CStatusBar    m_wndStatusbar;
	CChildView    m_wndView;
	HACCEL m_hAccel;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCreateDelete();
	afx_msg void OnCreateRandom();
	afx_msg void OnCreateRectangle();
	afx_msg void OnDropFiles(HDROP hDropInfo);


};


