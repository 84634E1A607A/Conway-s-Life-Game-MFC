#pragma once


class CHelpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHelpDlg)

public:
	CHelpDlg(CWnd* pParent = nullptr); 
	virtual ~CHelpDlg();


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HELP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CListBox Sitelist;
public:
	afx_msg void OnBnClickedGo();
};
