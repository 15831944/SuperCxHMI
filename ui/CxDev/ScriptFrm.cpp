// ScriptFrame.cpp : implementation file
//

#include "stdafx.h"
#include <afxcmn.h>
#include "DevInc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptFrame

IMPLEMENT_DYNCREATE(CScriptFrame, CMDIChildWnd)

CScriptFrame::CScriptFrame()
{
}

CScriptFrame::~CScriptFrame()
{
}

BEGIN_MESSAGE_MAP(CScriptFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CScriptFrame)
	ON_WM_CREATE()
	ON_WM_MDIACTIVATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptFrame message handlers

int CScriptFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}



///////////////////////////////////////////////////////////////
//�� �ܣ����ش˺������´��ڱ���
////////////////////////////////////////////////////////////////
void CScriptFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle)
	{
		CString strText;
		strText = pDocument->GetTitle();
		strText += _T("(Script)");
		AfxSetWindowText(m_hWnd, strText);
	}
}


void CScriptFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	if (!bActivate)
		return;

	CDocument* pDoc = GetActiveDocument();
	if (pDoc != NULL && pDoc->IsKindOf(RUNTIME_CLASS(CFormDoc)))
	{
		((CFormDoc *)pDoc)->OnActivate();
	}

//	CLayoutView* pView = (CLayoutView* )GetActiveView();
//	if (pView != NULL)
//	{
//		SetActiveView(pView);
//		CFormDoc* pDoc = pView->GetDocument();
//		if (pDoc != NULL && bActivate)
//		{
//			pDoc->OnActivate();
//		}
//		
//		pView->m_bActive = bActivate;
//	}
//	
//	if (pActivateWnd == NULL)
//		theApp.NotifyDocumentActivate(0, 0);

}

BOOL CScriptFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.cx = theApp.m_settingMainForm.m_nWidth * 3 / 5;
	cs.cy = theApp.m_settingMainForm.m_nHeight * 3 / 4;
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

void CScriptFrame::OnClose() 
{
	if (m_lpfnCloseProc != NULL && !(*m_lpfnCloseProc)(this))
		return;
	
	CDocument* pActiveDoc = GetActiveDocument();
	if (!pActiveDoc->IsKindOf(RUNTIME_CLASS(CFormDoc)))
		return;
	
	CFormDoc* pFormDoc = (CFormDoc *)pActiveDoc;
	CDevDoc* pDoc = pFormDoc->m_pDocument;
	
	//��ʾ�Ƿ�������
	if (pFormDoc->m_viewList.GetCount() == 1)
	{
		//��ʾ�Ƿ�������
		if (pFormDoc->m_pFormObj == NULL && pFormDoc->IsModified() && 
			pFormDoc->m_viewList.GetCount() == 1)
		{
			int iRet = AfxMessageBox("�Ƿ񱣴滭�����ݣ�", MB_YESNOCANCEL | MB_ICONQUESTION);
			if (iRet == IDCANCEL)
				return;
			else if(iRet == IDNO)
			{	
				//������
				if (pDoc != NULL && pDoc->GetPathName() == "" &&
					pFormDoc->IsMainForm())
				{	
					//������ʾ����, �����ĵ�Ҳû�б���,��ɾ�����ĵ�
					pDoc->CloseDocument();
					return;
				}
				
				if (pDoc != NULL)
					pDoc->SetModifiedFlag(FALSE);
			}
			else
			{	
				if (pDoc != NULL)
					pDoc->SaveSubDocument(pFormDoc->m_pFormSubDoc);
			}
		}
		else
		{	
			if (pDoc != NULL && pDoc->GetPathName().Compare("") == 0 &&
				pFormDoc->IsMainForm())
			{	
				//ɾ�����ĵ�
				pDoc->CloseDocument();
				return;
			}
		}
	}

	pFormDoc->PreCloseFrame(this);
	
	// then destroy the window
	DestroyWindow();

//	if (m_lpfnCloseProc != NULL && !(*m_lpfnCloseProc)(this))
//		return;
//
//	CDocument* pActiveDoc = GetActiveDocument();
//	if (!pActiveDoc->IsKindOf(RUNTIME_CLASS(CFormDoc)))
//		return;
//
//	CFormDoc* pFormDoc = (CFormDoc *)pActiveDoc;
//	CDevDoc* pDoc = pFormDoc->m_pDocument;
//
//	if (pFormDoc->m_pFormObj != NULL)
//	{
//		pFormDoc->PreCloseFrame(this);
//
//		// then destroy the window
//		DestroyWindow();
//		return;
//	}
//	
//	CMDIChildWnd::OnClose();
}

void CScriptFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);

	CMainFrame* pMainFrame = (CMainFrame *)GetMDIFrame();
	if (pMainFrame->m_bFullScreenMode)
	{
		CRect rectWindow, rectClient;
		GetWindowRect(&rectWindow);
		GetClientRect(&rectClient);
		ClientToScreen(&rectClient);
		
		int nFullWidth = GetSystemMetrics(SM_CXSCREEN);
		int nFullHeight = GetSystemMetrics(SM_CYSCREEN);
		
		CRect rect;
		rect.left = rectWindow.left - rectClient.left;
		rect.top = rectWindow.top - rectClient.top;
		rect.right = rectWindow.right - rectClient.right + nFullWidth;
		rect.bottom = rectWindow.bottom - rectClient.bottom + nFullHeight;
		
		lpMMI->ptMaxSize.x = rect.Width();
        lpMMI->ptMaxSize.y = rect.Height();
        lpMMI->ptMaxTrackSize.x = rect.Width();
		lpMMI->ptMaxTrackSize.y = rect.Height();
	}
}
