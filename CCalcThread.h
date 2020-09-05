#pragma once



// CCalcThread

class CCalcThread : public CWinThread
{
	DECLARE_DYNCREATE(CCalcThread)

protected:
	CCalcThread(); 
	virtual ~CCalcThread();


public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


