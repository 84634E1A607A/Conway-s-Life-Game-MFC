#pragma once


// DlgOptions dialog

class DlgOptions : public CDialogEx
{
	DECLARE_DYNAMIC(DlgOptions)

public:
	DlgOptions(CWnd* pParent = theApp.m_pMainWnd);   // standard constructor
	virtual ~DlgOptions();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedPreview();
	afx_msg void OnChangeBuiltin();
	afx_msg void OnChangeDirection();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeTimer();
	afx_msg void OnChangeScale();
	afx_msg void OnBnClickedStartstop();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDown();
	afx_msg void OnBnClickedLeft();
	afx_msg void OnBnClickedRight();
	afx_msg void OnChangeXpivot();
	afx_msg void OnChangeYpivot();
};

extern DlgOptions theDlg;