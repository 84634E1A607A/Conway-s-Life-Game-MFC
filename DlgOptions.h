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
	HACCEL m_hAccel;

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
	afx_msg void OnBnClickedRadioclick();
	afx_msg void OnBnClickedRadiopen();
	afx_msg void OnBnClickedRadioeraser();
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedRadiodrag();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedRandfill();
	afx_msg void OnBnClickedFill();
	afx_msg void OnBnClickedCopy();
	afx_msg void OnBnClickedPaste();
	afx_msg void OnBnClickedNextgen();
	afx_msg void OnBnClickedOr();
	afx_msg void OnBnClickedAnd();
	afx_msg void OnBnClickedCover();
};

extern DlgOptions theDlg;
