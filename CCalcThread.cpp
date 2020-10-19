// CCalcThread.cpp: 实现文件
//

#include "pch.h"
#include "Life-MFC.h"
#include "Map.h"
#include "CCalcThread.h"


// CCalcThread
extern Map map;
extern CLifeMFCApp theApp;
extern CMutex theMutex;

IMPLEMENT_DYNCREATE(CCalcThread, CWinThread)

CCalcThread::CCalcThread()
{
}

CCalcThread::~CCalcThread()
{
}

BOOL CCalcThread::InitInstance()
{
	return TRUE;
}

int CCalcThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCalcThread, CWinThread)
END_MESSAGE_MAP()




BOOL CCalcThread::PreTranslateMessage(MSG* pMsg)
{
	
	switch(pMsg->message)
	{
	case UM_NEEDDATA: 
	{
		theMutex.Lock();
		map.calc();
		theMutex.Unlock();
		theApp.m_pMainWnd->SendMessage(UM_SENDDATA);
		return TRUE;
	}
	case UM_CLEAR:
	{
		theMutex.Lock();
		xpivot = ypivot = 0x08000000;
		started = false;
		map.clear();
		theMutex.Unlock();
		return TRUE;
	}
	case UM_START:
	{
		theMutex.Lock();
		started = !started;
		if (!started)
		{
			map.free_extra();
		}
		theMutex.Unlock();
		RECT rect;
		theApp.m_pMainWnd->GetClientRect(&rect);
		theApp.m_pMainWnd->RedrawWindow(&rect, 0, RDW_INVALIDATE | RDW_UPDATENOW);
		return TRUE;
	}
	case UM_CLOSETHREAD:
	{
		PostQuitMessage(0);
		return TRUE;
	}
	default:
		break;
	}
	
	return CWinThread::PreTranslateMessage(pMsg);
}
