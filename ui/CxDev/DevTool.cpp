// DevTool.cpp : implementation file
//
#include "stdafx.h"
#include "DevInc.h"

#include "CxHmiMisc_i.c"
#include "DTGUID.h"
#include "ActionObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDevTool

CPtrList CDevTool::c_tools;

static CSelectTool selectTool;
static CRectItemTool rectItemTool;
static CDrawTool pipeTool(DRAW_PIPE, CLSID_PolyTool);
static CDrawTool rectTool(DRAW_RECTANGLE, CLSID_RectTool);
static CDrawTool ellipseTool(DRAW_ELLIPSE, CLSID_RectTool);
static CDrawTool roundRectTool(DRAW_ROUNDRECT, CLSID_RectTool);
static CDrawTool lineTool(DRAW_LINE, CLSID_RectTool);
static CDrawTool textTool(DRAW_TEXT, CLSID_TextTool);
static CDrawTool datalinkTool(DRAW_DATALINK, CLSID_DataLinkTool);
static CDrawTool variableTool(DRAW_VARIABLE, CLSID_RectTool);
static CDrawTool arcTool(DRAW_ARC, CLSID_PieTool);
static CDrawTool pieTool(DRAW_PIE, CLSID_PieTool);
static CDrawTool chordTool(DRAW_CHORD, CLSID_PieTool);
static CDrawTool polygonTool(DRAW_POLYGON, CLSID_PolyTool);
static CDrawTool polylineTool(DRAW_POLYLINE, CLSID_PolyTool);
static CDrawTool buttonTool(DRAW_BUTTON, CLSID_RectTool);
static CDrawTool timerTool(DRAW_TIMER, CLSID_RectTool);
static CDrawTool chartTool(DRAW_CHART, CLSID_RectTool);
static CDrawTool almSumTool(DRAW_ALMSUM, CLSID_RectTool);
static CDrawTool menuTool(DRAW_MENU, CLSID_RectTool);


DRAW_SHAPE CDevTool::c_drawShape = DRAW_SELECTION;
CDevObj::TrackerState CDevTool::c_enumTrackerState = CDevObj::general;
CPoint CDrawTool::c_down;
CLayoutView* CDevTool::c_pActiveView = NULL;

int CDevTool::c_nScrollInset;
UINT CDevTool::c_nScrollDelay;
UINT CDevTool::c_nScrollInterval;

CDevTool::CDevTool(DRAW_SHAPE drawShape)
{
	m_pDC = NULL;

	m_drawShape = drawShape;
	c_tools.AddTail(this);

	m_nTimerID = MAKEWORD(-1, -1);
}

CDevTool* CDevTool::FindTool(DRAW_SHAPE drawShape)
{
	POSITION pos = c_tools.GetHeadPosition();
	while (pos != NULL)
	{
		CDevTool* pTool = (CDevTool*)c_tools.GetNext(pos);
		if (pTool->m_drawShape == drawShape)
			return pTool;
	}

	return NULL;
}

void CDevTool::InitTools()
{
	POSITION pos = c_tools.GetHeadPosition();
	while (pos != NULL)
	{
		CDevTool* pTool = (CDevTool*)c_tools.GetNext(pos);
		pTool->Initialize();
	}

	// get scroll metrics from win.ini
	static const TCHAR szWindows[] = _T("windows");
	static const TCHAR szScrollDelay[] = _T("DragScrollDelay");
	static const TCHAR szScrollInset[] = _T("DragScrollInset");
	static const TCHAR szScrollInterval[] = _T("DragScrollInterval");
	
	c_nScrollInset = GetProfileInt(szWindows, szScrollInset, DD_DEFSCROLLINSET);
	c_nScrollDelay = GetProfileInt(szWindows, szScrollDelay, DD_DEFSCROLLDELAY);
	c_nScrollInterval = GetProfileInt(szWindows, szScrollInterval,
		DD_DEFSCROLLINTERVAL);
}

void CDevTool::DeInitTools()
{
	POSITION pos = c_tools.GetHeadPosition();
	while (pos != NULL)
	{
		CDevTool* pTool = (CDevTool*)c_tools.GetNext(pos);
		pTool->DeInitialize();
	}
}

void CDevTool::Initialize()
{
}

void CDevTool::DeInitialize()
{
}

void CDevTool::SetTrackerState(CDevObj::TrackerState state, BOOL bUpdate)
{
	//ˢ�»��ͼ
	CMDIFrameWnd* pParentFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	CMDIChildWnd* pMDIActive = pParentFrame->MDIGetActive();
	CView* pView = NULL;
	if (pMDIActive != NULL)
	{
		CDocument* pActiveDoc = pMDIActive->GetActiveDocument();
		if (pActiveDoc != NULL)
		{
			POSITION pos = pActiveDoc->GetFirstViewPosition();
			while (pos != NULL)
			{
				pView = pActiveDoc->GetNextView(pos);
				if (pView->IsKindOf(RUNTIME_CLASS(CLayoutView)))
				{
					((CLayoutView *)pView)->OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
					break;
				}
			}
		}
	}
	
	c_enumTrackerState = state;

	if (bUpdate && pView != NULL)
		((CLayoutView *)pView)->OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
}

void CDevTool::OnDraw(CLayoutView* pView, CDC* pDC)
{
}

void CDevTool::OnLButtonDown(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	pView->SetCapture();
	c_down = point;
}

void CDevTool::OnLButtonDblClk(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
}

void CDevTool::OnLButtonUp(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	ReleaseCapture();

	if (point == c_down)
		c_drawShape = DRAW_SELECTION;
}

void CDevTool::OnMouseMove(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CDevTool::OnRButtonDown(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	OnCancel();
}

void CDevTool::OnChar(CLayoutView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CDevTool::OnKeyDown(CLayoutView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CDevTool::OnCancel()
{
	c_drawShape = DRAW_SELECTION;
	SetTrackerState(CDevObj::general);
}

void CDevTool::OnInit()
{
	c_pActiveView = NULL;

	CMDIFrameWnd* pParentFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	CMDIChildWnd* pMDIActive = pParentFrame->MDIGetActive();
	if (pMDIActive != NULL)
	{
		CDocument* pActiveDoc = pMDIActive->GetActiveDocument();
		if (pActiveDoc != NULL)
		{
			POSITION pos = pActiveDoc->GetFirstViewPosition();
			while (pos != NULL)
			{
				CView* pView = pActiveDoc->GetNextView(pos);
				if (pView->IsKindOf(RUNTIME_CLASS(CLayoutView)))
				{
					c_pActiveView = (CLayoutView *)pView;
					break;
				}
			}
		}
	}

}

void CDevTool::DrawRubberBand(CLayoutView* pView, CRect rect)
{
	CDC dc;
	CDC* pDrawDC = &dc;//pView->GetDC();
	pView->PrepareRopDC(pDrawDC);
	ASSERT_VALID(pDrawDC);

	pDrawDC->MoveTo(rect.left, rect.top);
	pDrawDC->LineTo(rect.right, rect.top);
	pDrawDC->LineTo(rect.right, rect.bottom);
	pDrawDC->LineTo(rect.left, rect.bottom);
	pDrawDC->LineTo(rect.left, rect.top);

	pView->ReleaseDC(pDrawDC);
}

//////////////////////////////////////////////////////////////////////////////////
//�� �ܣ�ѡ����Ƥ����
//////////////////////////////////////////////////////////////////////////////////
BOOL CDevTool::TrackRubberBand(CLayoutView* pView, const CPoint& point, CRect& rect)
{
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	AfxLockTempMaps();  // protect maps while looping

	// set capture to the window which received this message
	pView->SetCapture();
	ASSERT(pView == (CLayoutView *)CWnd::GetCapture());
	pView->UpdateWindow();

	CDC dc;
	CDC* pDrawDC = &dc;//pView->GetDC();
	pView->PrepareRopDC(pDrawDC, FALSE);
	ASSERT_VALID(pDrawDC);

	CPoint pointSave = point;
	CPoint pointOld = point;
	CPoint pointNew = point;
	CPoint ptTemp;
	
	BOOL bMouseUp;
	BOOL bMoved = FALSE;

	// get messages until capture lost or cancelled/accepted
	for (;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (pView != (CLayoutView *)CWnd::GetCapture())
			break;

		switch (msg.message)
		{
		// handle movement/accept messages
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			pointNew = CPoint((int)(short)LOWORD(msg.lParam), (int)(short)HIWORD(msg.lParam));
		
			//�ı�״̬������ֵ
			ptTemp = pointNew;
			pView->ClientToDoc(ptTemp);
			((CMainFrame *)(theApp.m_pMainWnd))->OnViewMousePosChange(ptTemp);
		
			bMouseUp = (msg.message == WM_LBUTTONUP);
			if (pointNew != pointOld || bMouseUp)
			{
				if (bMoved)
				{
					CRect rcTemp(pointSave, pointOld);
					rcTemp.NormalizeRect();
					pDrawDC->DrawFocusRect(rcTemp);
				}
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;
			}
			if (bMouseUp)
			{
				pointOld = pointNew;
				goto ExitLoop;
			}

			if (pointNew != pointOld)
			{
				pointOld = pointNew;
				CRect rect(pointSave, pointOld);
				rect.NormalizeRect();
				pDrawDC->DrawFocusRect(rect);
			}
			break;

		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				CRect rcTemp(pointSave, pointOld);
				rcTemp.NormalizeRect();
				pDrawDC->DrawFocusRect(rcTemp);
			}
			pointNew = pointSave;
			goto ExitLoop;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	pView->ReleaseDC(pDrawDC);
	ReleaseCapture();

	AfxUnlockTempMaps(FALSE);

	if (pointSave != pointNew)
	{
		CRect rcTemp(pointSave, pointNew);
		rcTemp.NormalizeRect();
		rect = rcTemp;
	}

	// return TRUE only if point has changed
	return (pointSave != pointNew);	
}

void CDevTool::OnDragScroll(CLayoutView* pView, CPoint point)
{
	ASSERT_VALID(pView);
	
	// get client rectangle of destination window
	CRect rectClient;
	pView->GetClientRect(&rectClient);
	CRect rect = rectClient;
	
	// hit-test against inset region
	UINT nTimerID = MAKEWORD(-1, -1);
	rect.InflateRect(-c_nScrollInset, -c_nScrollInset);
	CSplitterWnd* pSplitter = NULL;
	if (rectClient.PtInRect(point) && !rect.PtInRect(point))
	{
		// determine which way to scroll along both X & Y axis
		if (point.x < rect.left)
			nTimerID = MAKEWORD(SB_LINEUP, HIBYTE(nTimerID));
		else if (point.x >= rect.right)
			nTimerID = MAKEWORD(SB_LINEDOWN, HIBYTE(nTimerID));
		if (point.y < rect.top)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEUP);
		else if (point.y >= rect.bottom)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEDOWN);
		ASSERT(nTimerID != MAKEWORD(-1, -1));
		
		// check for valid scroll first
		pSplitter = CView::GetParentSplitter(pView, FALSE);
		BOOL bEnableScroll = FALSE;
		if (pSplitter != NULL)
			bEnableScroll = pSplitter->DoScroll(pView, nTimerID, FALSE);
		else
			bEnableScroll = pView->OnScroll(nTimerID, 0, FALSE);
		if (!bEnableScroll)
			nTimerID = MAKEWORD(-1, -1);
	}
	
	// save tick count when timer ID changes
	DWORD dwTick = GetTickCount();
	if (nTimerID != m_nTimerID)
	{
		m_dwLastTick = dwTick;
		m_nScrollDelay = c_nScrollDelay;
	}
	
	// scroll if necessary
	if (dwTick - m_dwLastTick > m_nScrollDelay)
	{
		if (pSplitter != NULL)
			pSplitter->DoScroll(pView, nTimerID, TRUE);
		else
			pView->OnScroll(nTimerID, 0, TRUE);
		m_dwLastTick = dwTick;
		m_nScrollDelay = c_nScrollInterval;
	}
	
	m_nTimerID = nTimerID;
}

/////////////////////////////////////////////////////////////////////////////
// CDevTool message handlers

//////////////////////////////////////////////////////////////////////
// COleDrawTool Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawObj* CDrawTool::c_pDrawingObject = NULL;

CDrawTool::CDrawTool(DRAW_SHAPE nDRAW_SHAPE, CLSID clsid) :
	CDevTool(nDRAW_SHAPE)
{
	m_clsid = clsid;
	m_pDrawTool = NULL;
	m_pIConnectionPoint = NULL;
}

CDrawTool::~CDrawTool()
{
}


BEGIN_MESSAGE_MAP(CDrawTool, CCmdTarget)
	//{{AFX_MSG_MAP(CDrawTool)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawTool message handlers

void CDrawTool::Initialize()
{
	HRESULT hr;
	IConnectionPointContainer* pContainer;
	DWORD dwCookie;

	hr = ::CoCreateInstance(
		m_clsid,           // either Gdi or GL
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICxDrawTool,
		reinterpret_cast<void**>(&m_pDrawTool));
	if (FAILED(hr))
	{
		TRACE("Failed to create the drawtool component.");
		return;
	}
	m_pDrawTool->SetDrawToolSite(&m_xCxDrawToolSite);

	hr = m_pDrawTool->QueryInterface(IID_IConnectionPointContainer, (void**)&pContainer) ;
	if (FAILED(hr))
	{
		TRACE("Failed to find IConnectionPointContainer.");
		return;
	}

	hr = pContainer->FindConnectionPoint(IID_ICxDrawToolEvents, &m_pIConnectionPoint) ;
	if (FAILED(hr))
		return;
	m_pIConnectionPoint->Advise((IUnknown *)&m_xCxDrawToolEvents, &dwCookie) ;
	
	pContainer->Release();
}

void CDrawTool::DeInitialize()
{
	if (m_pDrawTool != NULL)
	{
		m_pDrawTool->Release();
		m_pDrawTool = NULL;
	}
	
	if (m_pIConnectionPoint != NULL)
	{
		m_pIConnectionPoint->Release();
		m_pIConnectionPoint = NULL;
	}
}

void CDrawTool::OnDraw(CLayoutView* pView, CDC* pDC)
{
	c_pActiveView = pView;
	if (m_pDrawTool != NULL)
	{
		if (m_pDrawTool != NULL)
			m_pDrawTool->DoDraw(pDC->m_hDC);
	}
}

void CDrawTool::OnLButtonDown(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);
	pView->AlignToGrid(local);

	if (!pView->m_selection.IsEmpty())
	{
		pView->Select(NULL);
		pView->UpdateWindow();
	}

	c_pActiveView = pView;
	if (m_pDrawTool != NULL)
		m_pDrawTool->DoLButtonDown(nFlags, local);
}

void CDrawTool::OnLButtonDblClk(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);
	pView->AlignToGrid(local);

	c_pActiveView = pView;
	if (m_pDrawTool != NULL)
		m_pDrawTool->DoLButtonDblClk(nFlags, local);
}

void CDrawTool::OnLButtonUp(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);
	pView->AlignToGrid(local);

	c_pActiveView = pView;
	if (m_pDrawTool != NULL)
		m_pDrawTool->DoLButtonUp(nFlags, local);
}

void CDrawTool::OnMouseMove(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);
	pView->AlignToGrid(local);

	c_pActiveView = pView;
	if (m_pDrawTool != NULL)
		m_pDrawTool->DoMouseMove(nFlags, local);
}

void CDrawTool::OnRButtonDown(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);
	pView->AlignToGrid(local);

	c_pActiveView = pView;
	if (m_pDrawTool != NULL)
		m_pDrawTool->DoRButtonDown(nFlags, local);
}

void CDrawTool::OnChar(CLayoutView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	c_pActiveView = pView;
	if (m_pDrawTool != NULL)
		m_pDrawTool->DoChar(nChar, nRepCnt, nFlags);
}

void CDrawTool::OnKeyDown(CLayoutView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	c_pActiveView = pView;
	if (m_pDrawTool != NULL)
		m_pDrawTool->DoKeyDown(nChar, nRepCnt, nFlags);
}

void CDrawTool::OnCancel()
{
	if (m_pDrawTool != NULL)
		m_pDrawTool->DoCancel();
}

void CDrawTool::OnInit()
{
	CDevTool::OnInit();

	if (m_pDrawTool != NULL)
		m_pDrawTool->DoInit();

}

//�����Ƶ���۱仯
HRESULT CDrawTool::DrawAspectChange(RECT* pInvalidRect)
{
	HRESULT hr = S_OK;
	CRect rect;
	
	if (pInvalidRect != NULL)
	{
		rect = CRect(pInvalidRect);
		c_pActiveView->DocToClient(rect);
		c_pActiveView->InvalidateRect(pInvalidRect);
	}
	else if (m_pDrawTool != NULL)
	{
		hr = m_pDrawTool->GetBoundingRect(&rect);
		if (hr == S_OK)
		{
			c_pActiveView->DocToClient(rect);
			c_pActiveView->InvalidateRect(rect, FALSE);
		}
	}

	return hr;
}

HRESULT CDrawTool::DrawObjCreate()
{
	HRESULT hr = S_OK;

	CString strAction;
	strAction.LoadString(IDS_ADDOBJ);
	CFormDoc* pDoc = c_pActiveView->GetDocument();
	CAction* pAct = new CActionObjAdd(strAction, pDoc, c_pDrawingObject, TRUE);
	pDoc->m_stackUndo.Push(pAct);

	c_pDrawingObject->Invalidate();
	c_pActiveView->Select(c_pDrawingObject);
	c_pDrawingObject = NULL;
//	c_pActiveView->Invalidate();
	c_drawShape = DRAW_SELECTION;
	SetTrackerState(CDevObj::general);

	return hr;
}

HRESULT CDrawTool::DrawObjCancel()
{
	HRESULT hr = S_OK;
	
	if (c_pDrawingObject != NULL)
		c_pDrawingObject->Delete();
	c_pDrawingObject = NULL;
	if (c_pActiveView != NULL)
		c_pActiveView->Invalidate();
	c_drawShape = DRAW_SELECTION;
	SetTrackerState(CDevObj::general);

	return hr;
}

//////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CDrawTool, CCmdTarget)
	INTERFACE_PART(CDrawTool, IID_ICxDrawToolSite, CxDrawToolSite)
	INTERFACE_PART(CDrawTool, IID_ICxDrawToolEvents, CxDrawToolEvents)
END_INTERFACE_MAP()

STDMETHODIMP_(ULONG) CDrawTool::XCxDrawToolSite::AddRef()
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite)

    return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CDrawTool::XCxDrawToolSite::Release()
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite)

	return pThis->ExternalRelease();
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::QueryInterface(REFIID iid,
	LPVOID* ppvObj)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite)

	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::CaptureMouse()
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite);  
	
	pThis->c_pActiveView->SetCapture();

	return (HRESULT)NOERROR;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::ReleaseMouse()
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite);  
	
	ReleaseCapture();

	return (HRESULT)NOERROR;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::GetDrawObject(REFCLSID clsid, IUnknown **ppUnk )
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite); 
	
	if (pThis->c_pActiveView == NULL)
		return S_FALSE;
	
	CFormDoc* pDoc;
	CDrawObj* pObj;
	pDoc = pThis->c_pActiveView->GetDocument();
	pObj = pDoc->AddDrawObj(clsid, NULL);

	if (pObj != NULL)
	{
		*ppUnk = (IUnknown *)pObj->m_pDrawObject;
		c_pDrawingObject = pObj;
	}

	return (HRESULT)NOERROR;
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::GetDrawShape(DRAW_SHAPE* pDrawShape)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite);  
	
	*pDrawShape = pThis->c_drawShape;
	
	return (HRESULT)NOERROR;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::GetWindow(HWND* phWindow)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite);  
	
	*phWindow = pThis->c_pActiveView->GetSafeHwnd();
	
	return (HRESULT)NOERROR;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::GetDC(HDC* phDC)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite); 
	
	if (phDC == NULL)
		return E_INVALIDARG;

	if (pThis->m_pDC == NULL)
	{
	//	pThis->m_pDC = pThis->c_pActiveView->GetDC();
	//	pThis->c_pActiveView->OnPrepareDC(pThis->m_pDC, NULL);
		pThis->m_pDC = new CDC;
		pThis->c_pActiveView->PrepareRopDC(pThis->m_pDC);
	}
	ASSERT(pThis->m_pDC != NULL);

	*phDC = pThis->m_pDC->GetSafeHdc();
	
	return S_OK;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::ReleaseDC(HDC hDC)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite); 
	
	ASSERT(hDC == pThis->m_pDC->GetSafeHdc());

	pThis->c_pActiveView->ReleaseDC(pThis->m_pDC);
	delete pThis->m_pDC;
	pThis->m_pDC = NULL;
	
	return S_OK;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::DocToClient(POINT* pPoint)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite); 
	
	CPoint point = *pPoint;
	pThis->c_pActiveView->DocToClient(point);
	memcpy(pPoint, &point, sizeof(POINT));
	
	return S_OK;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::ClientToDoc(POINT* pPoint)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite); 
	
	CPoint point = *pPoint;
	pThis->c_pActiveView->ClientToDoc(point);
	memcpy(pPoint, &point, sizeof(POINT));

	return S_OK;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::GetExpressionCreatorProc(long lProc)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite); 

	if (pThis->c_pActiveView == NULL)
		return S_FALSE;

	CFormDoc::m_pExpCrtDoc = (CFormDoc *)pThis->c_pActiveView->GetDocument();
	*(ExpressionCreatorProc *)lProc = CFormDoc::ExpressionCreatorProc;
	
	return S_OK;   
}

STDMETHODIMP CDrawTool::XCxDrawToolSite::GetGridSpace(SIZE* pSize)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolSite); 

	if (pThis->c_pActiveView == NULL)
		return S_FALSE;

	CSize szGrid;
	pThis->c_pActiveView->GetGridSpace(szGrid);
	*pSize = szGrid;

	return S_OK;
}

///////////////////////�¼�����//////////////////////////////////
STDMETHODIMP_( ULONG ) CDrawTool::XCxDrawToolEvents::AddRef()
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolEvents);

	return pThis->ExternalAddRef();
}

STDMETHODIMP_( ULONG ) CDrawTool::XCxDrawToolEvents::Release()
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolEvents);

    return pThis->ExternalRelease();
}

STDMETHODIMP CDrawTool::XCxDrawToolEvents::QueryInterface(REFIID iid,
	LPVOID* ppvObj)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolEvents);

	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CDrawTool::XCxDrawToolEvents::OnViewChange(RECT* pInvalidRect)
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolEvents);

	return pThis->DrawAspectChange(pInvalidRect);
}

STDMETHODIMP CDrawTool::XCxDrawToolEvents::OnCreate()
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolEvents);

	return pThis->DrawObjCreate();
}

STDMETHODIMP CDrawTool::XCxDrawToolEvents::OnCancel()
{
	METHOD_PROLOGUE(CDrawTool, CxDrawToolEvents);

	return pThis->DrawObjCancel();
}


//////////////////////////////////////////////////////////////////////
// CRectItemTool Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLSID CRectItemTool::c_clsid = CLSID_NULL;

CRectItemTool::CRectItemTool() :
	CDevTool(DRAW_RECTITEM)
{
	m_bMouseDown = FALSE;
}

CRectItemTool::~CRectItemTool()
{
}

///////////////////////////////////////////////////////////////////////
//�� �ܣ���������������
//�� ������ɿؼ�����
//////////////////////////////////////////////////////////////////////////
void CRectItemTool::OnLButtonDown(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	c_pActiveView = pView;

	m_bMouseDown = TRUE;

	CPoint local = point;
	pView->ClientToDoc(local);
	pView->AlignToGrid(local);

	m_rect.left = m_rect.right = local.x;
	m_rect.top = m_rect.bottom = local.y;

	DrawRubberBand(pView, m_rect);

	CDevTool::OnLButtonDown(pView, nFlags, point);
}

void CRectItemTool::OnMouseMove(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	if (m_bMouseDown)
	{
		DrawRubberBand(pView, m_rect);

		CPoint local = point;
		pView->ClientToDoc(local);
		pView->AlignToGrid(local);

		m_rect.right = local.x;
		m_rect.bottom = local.y;
		
		DrawRubberBand(pView, m_rect);
	}

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
}

void CRectItemTool::OnLButtonUp(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	if (!m_bMouseDown)
		return;

	m_bMouseDown = FALSE;

	DrawRubberBand(pView, m_rect);

	CPoint local = point;
	pView->ClientToDoc(local);
	pView->AlignToGrid(local);
	
	m_rect.right = local.x;
	m_rect.bottom = local.y;

	ReleaseCapture();
		
	m_rect.NormalizeRect();

	CDevObj* pObj = NULL;

	ASSERT(c_clsid != CLSID_NULL);

	BeginWaitCursor();

	try
	{
		// �������ӵ��ĵ�������Ŀ
		CFormDoc* pDoc = pView->GetDocument();

		pObj = pDoc->AddCtrlObj(c_clsid, NULL, m_rect);
		pObj->MoveTo(m_rect, FALSE);
		ASSERT_VALID(pObj);

		//ѡ���²������Ŀ
		pView->Select(pObj);

		CString strAction;
		strAction.LoadString(IDS_ADDOBJ);
		CAction* pAct = new CActionObjAdd(strAction, pDoc, pObj, TRUE);
		pDoc->m_stackUndo.Push(pAct);
	}
	catch (COleException* pException)
	{
		CString strErrorMessage;
		_com_error error( pException->m_sc );
	
		pException->Delete();

		if (pObj != NULL)
		{
			ASSERT_VALID(pObj);
			pObj->Delete();
		}

		AfxFormatString1(strErrorMessage, IDS_CREATEFAILED,
			error.ErrorMessage());

		AfxMessageBox(strErrorMessage);
	}

	EndWaitCursor();

	c_drawShape = DRAW_SELECTION;
	SetTrackerState(CDevObj::general);
	c_clsid = CLSID_NULL;
}

void CRectItemTool::OnKeyDown(CLayoutView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CSize sizeGrid;
	pView->GetGridSpace(sizeGrid);

	CPoint ptOffset(0, 0);
	switch (nChar)
	{
	case VK_LEFT:
		ptOffset.x = -sizeGrid.cx;
		break;
	case VK_UP:
		ptOffset.y = -sizeGrid.cy;
		break;
	case VK_RIGHT:
		ptOffset.x = sizeGrid.cx;
		break;
	case VK_DOWN:
		ptOffset.y = sizeGrid.cy;
		break;
	default:
		break;
	}

	CPoint point;
	GetCursorPos(&point);
	point.x += ptOffset.x;
	point.y += ptOffset.y;
	SetCursorPos(point.x, point.y);
}

void CRectItemTool::OnCancel()
{
	m_bMouseDown = FALSE;
	ReleaseCapture();

	if (c_pActiveView != NULL)
		c_pActiveView->Invalidate();
	c_drawShape = DRAW_SELECTION;
	SetTrackerState(CDevObj::general);
}

//////////////////////////////////////////////////////////////////////
// CSelectTool Class
//////////////////////////////////////////////////////////////////////

BOOL CSelectTool::c_bTrackingHandle = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSelectTool::CSelectTool() :
	CDevTool(DRAW_SELECTION)
{
}

CSelectTool::~CSelectTool()
{

}


////////////////////////////////////////////////////////////////////////////////////////////
//���ܣ���������������
//������
/////////////////////////////////////////////////////////////////////////////////////////////
void CSelectTool::OnLButtonDown(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);

	CDevObj* pObj = NULL;
	int nDragHandle;

	//����Ѿ�ѡ�е�������
	if (pView->m_selection.GetCount() == 1)
	{
		pObj = pView->m_selection.GetHead();
		if (!pObj->m_bLocked)
		{
			//�Ƿ�����ֱ�
			nDragHandle = pObj->HitTestHandle(local, pView, CDevTool::c_enumTrackerState);
			if (nDragHandle != 0)
			{
				SetCursor(pObj->GetHandleCursor(nDragHandle, CDevTool::c_enumTrackerState));
				c_bTrackingHandle = TRUE;
				pObj->Invalidate();

				TrackHandle(pView, pObj, nDragHandle, point);

				if (c_enumTrackerState == CDevObj::rotate && nDragHandle != 1)
					SetTrackerState(CDevObj::general);
				c_bTrackingHandle = FALSE;
				return;
			}
		}
	}		

	pObj = pView->GetObjectAt(point);
	BOOL bControlState = (nFlags & MK_CONTROL) != 0;
	BOOL bDraged = FALSE;

	if (pObj != NULL)
	{
		if (!bControlState)
		{
			if (!pView->IsSelected(pObj))
			{
				pView->Select(pObj, FALSE, FALSE);
			}
			else if (pView->m_pFocusObj != pObj)
			{
				pView->Select(pObj, TRUE, FALSE);
				pView->Select(pObj, TRUE, FALSE);
				theApp.NotifySelectChanged();
			}
		}
		
		CDevTool::SetTrackerState(CDevObj::general, TRUE);

		//�����ȸ��´��ڣ����򽫳���ˢ������
		pView->UpdateWindow(); // update before entering the tracker
	
		/////////////////////////////////////////////////////

		//�ƶ����Ϸ�
		if (pView->m_selection.Find(pObj) != NULL && !pObj->m_bLocked)
		{
			CPoint ptStart = point;
			pView->ClientToDoc(ptStart);
			pView->AlignToGrid(ptStart);
							
			pView->m_bInDrag = TRUE;
			CLayoutView::m_bCanDropEffectCopy = FALSE;
			COleDataSource* pDataSource = pView->GetClipboardData(&ptStart, TRUE);
			DROPEFFECT dropEffect = pDataSource->DoDragDrop(
				DROPEFFECT_COPY|DROPEFFECT_MOVE, NULL);
			delete pDataSource;
			
			// ���û�н����Ϸţ�����Ϊ�ǿ��ܷ�ѡ
			if (dropEffect == DROPEFFECT_NONE && bControlState)
			{
				pView->Select(pObj, TRUE, FALSE);
				theApp.NotifySelectChanged();
			}
			
			if (pView->m_bInDrag == FALSE) // move in same window
				return;
			pView->m_bInDrag = FALSE;
			
			if (dropEffect == DROPEFFECT_MOVE)
			{
				pView->OnEditClear();
			}

			bDraged = TRUE;
		}
		else
		{
			if (bControlState)
			{
				pView->Select(pObj, TRUE, FALSE);
				theApp.NotifySelectChanged();
			}
		}
	}

	if (!bDraged)
	{
		if (!bControlState && pView->DoSpecialLButtonDown(nFlags, point) == TRUE)
			return;

		CRect rect;
		rect.SetRectEmpty();

		//Ȧ�����ο�ѡȡ
		if (TrackRubberBand(pView, point, rect))
		{
			pView->SelectWithinRect(rect, bControlState);
		}
		else if (pObj == NULL)
		{
			if (!bControlState)
				pView->Select(NULL);
		}
	}

//	//ʹ���ĿתΪ���״̬
//	COleClientItem* pActiveItem = pView->GetDocument()->GetInPlaceActiveItem(pView);
//	if (pActiveItem != NULL)
//	{
//		pActiveItem->Close();
//		ASSERT(pView->GetDocument()->GetInPlaceActiveItem(pView) == NULL);
//	}

}

////////////////////////////////////////////////////////////////////////////////////////////
//���ܣ��������˫��
//������1��˫�����򿪶�������ҳ���б༭
//      2����סSHIFT
/////////////////////////////////////////////////////////////////////////////////////////////
void CSelectTool::OnLButtonDblClk(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	BOOL bShiftState = (nFlags & MK_SHIFT) != 0;
	if (!bShiftState)
		pView->OnEditProperties();
}

////////////////////////////////////////////////////////////////////////////////////
//�� �ܣ���������ƶ�
//�� ���������������
//	1����ǰ��ͼû�в�����꣨����������ס����ѱ��϶�ʱ�Ჶ����꣩�����ѡ��һ������
//��������ѱ�λ�ã����ðѱ�����꣬������û������ü�ͷ��ꡣ
//  2����ǰ��ͼ������꣨����������ס����ѱ��϶�ʱ�Ჶ����꣩���ƶ�����ѱ��������������
////////////////////////////////////////////////////////////////////////////////////
void CSelectTool::OnMouseMove(CLayoutView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);

	//�ı��������״
	if (pView->GetCapture() != pView)
	{
		if (c_drawShape == DRAW_SELECTION && pView->m_selection.GetCount() == 1)
		{
			CDevObj* pObj = pView->m_selection.GetHead();
			if (!pObj->m_bLocked)
			{
				int nHandle = pObj->HitTestHandle(local, pView, CDevTool::c_enumTrackerState);
				if (nHandle != 0)
				{
					SetCursor(pObj->GetHandleCursor(nHandle, CDevTool::c_enumTrackerState));
					return; // bypass CDevTool
				}
			}
		}
	}

	if (c_drawShape == DRAW_SELECTION)
	{
		POSITION posObj = pView->m_selection.GetHeadPosition();
		while (posObj != NULL)
		{
			CDevObj* pObj = pView->m_selection.GetNext(posObj);
			if (pObj->HitTest(local, pView))
			{
			//	if (pObj->m_bLocked)
			//		break;
				
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				return;
			}
		}
		
		if (pView->DoSpecialMouseMove(nFlags, point) == TRUE)
			return;

		CDevTool::OnMouseMove(pView, nFlags, point);
	}
}

void CSelectTool::OnKeyDown(CLayoutView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CSize sizeGrid;
	pView->GetGridSpace(sizeGrid);

	CSize sizeDisplacement(0, 0);
	BOOL bMove = TRUE;
	switch (nChar)
	{
		case VK_DELETE:
			pView->OnEditClear();
			return;
		case VK_UP:
			sizeDisplacement = CSize(0, -sizeGrid.cy);
			break;
		case VK_DOWN:
			sizeDisplacement = CSize(0, sizeGrid.cy);;
			break;
		case VK_LEFT:
			sizeDisplacement = CSize(-sizeGrid.cx, 0);
			break;
		case VK_RIGHT:
			sizeDisplacement = CSize(sizeGrid.cx, 0);;
			break;
		case VK_PRIOR:
			sizeDisplacement = CSize(0, -sizeGrid.cy);;
			break;
		case VK_NEXT:
			sizeDisplacement = CSize(0, sizeGrid.cy);;
			break;
		default:
			bMove = FALSE;
	}

	POSITION pos;
	CDevObj* pObj;

	if (bMove)
	{
		CFormDoc* pDoc = pView->GetDocument();
		CActionObjMoveSet* pAct = new CActionObjMoveSet(_T("�ƶ�"), pDoc);
		if (pAct == NULL)
			AfxThrowMemoryException();

		pos = pView->m_selection.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = pView->m_selection.GetNext(pos);
			pAct->AddObj(pObj);
		}

		pAct->SetUnitState(TRUE);
		pDoc->m_stackUndo.Push(pAct);
		pDoc->SetModifiedFlag();

		pos = pView->m_selection.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = pView->m_selection.GetNext(pos);
			if (!pObj->m_bLocked)
			{
				CRectF rcItem = pObj->GetPositionRect();
				rcItem.OffsetRect((float)sizeDisplacement.cx, (float)sizeDisplacement.cy);
				pObj->MoveTo(rcItem, FALSE, pView);
				pObj->Invalidate();
			}
		}

		theApp.NotifySelectChanged(FALSE, 0, TRUE);
	}
}
//////////////////////////////////////////////////////////////////////////////////
//�� �ܣ�ȷ����ѡ����Ŀ�ĸ��ٿ�
//////////////////////////////////////////////////////////////////////////////////
void CSelectTool::SetupSelectionTracker(CLayoutView *pView, CRectTracker *pTracker)
{
	ASSERT(!pView->m_selection.IsEmpty());

	pTracker->m_rect.SetRectEmpty();
	CRect rcBound = pView->GetSelectionRect();
	pView->DocToClient(rcBound);
	pTracker->m_rect = rcBound;
	pTracker->m_rect.InflateRect(MARGIN_PIXELS, MARGIN_PIXELS);
	pTracker->m_nHandleSize = MARGIN_PIXELS + 1;
	pTracker->m_sizeMin.cx = 8;
	pTracker->m_sizeMin.cy = 8;
	pTracker->m_nStyle = CRectTracker::solidLine;
}

//////////////////////////////////////////////////////////////////////////////////
//�� �ܣ������ƶ�����ѱ�
//////////////////////////////////////////////////////////////////////////////////
BOOL CSelectTool::TrackHandle(CLayoutView* pView, CDevObj* pObj, int nHandle, CPoint point)
{
	ASSERT(nHandle >= 0);
	ASSERT(nHandle <= pObj->GetHandleCount(CDevTool::c_enumTrackerState)); 
	
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	AfxLockTempMaps();  // protect maps while looping

	// set capture to the window which received this message
	pView->SetCapture();
	ASSERT(pView == (CLayoutView *)CWnd::GetCapture());
	pView->UpdateWindow();

	CDC dc;
	CDC* pDrawDC = &dc;//pView->GetDC();
	pView->PrepareRopDC(pDrawDC);
	ASSERT_VALID(pDrawDC);

	CPen penGrid(PS_DOT, 0, RGB(0, 0, 0));
	CPen* pOldPen = pDrawDC->SelectObject(&penGrid);

	CPoint pointSave = point;
	CPoint pointOld = point;
	CPoint pointNew = point;
	CPoint pointTemp;

	CPoint pointOffset = point;
	pView->ClientToDoc(pointOffset);
	pointOffset -= pObj->GetHandle(nHandle, CDevTool::c_enumTrackerState, pView);
	
	BOOL bMouseUp;
	BOOL bMoved = FALSE;

	CSize sizeGrid;
	pView->GetGridSpace(sizeGrid);

//	pointTemp = point - pointOffset;
//	pView->ClientToDoc(pointTemp);
//	pObj->DrawTrackerHandleShape(pDrawDC, nHandle, CDevTool::c_enumTrackerState, pointTemp);

	// get messages until capture lost or cancelled/accepted
	for (;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (pView != (CLayoutView *)CWnd::GetCapture())
			break;

		switch (msg.message)
		{
		// handle movement/accept messages
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
			if (msg.message == WM_KEYDOWN)
			{
				// handle cancel messages
				if (msg.wParam == VK_ESCAPE)
				{
					if (bMoved)
					{
						pointTemp = pointOld;
						pView->ClientToDoc(pointTemp);
						pointTemp -= pointOffset;
						pView->AlignToGrid(pointTemp);
						pObj->DrawTrackerHandleShape(pDrawDC, nHandle, CDevTool::c_enumTrackerState, pointTemp, pView);
					}
					pointNew = pointSave;
					goto ExitLoop;
				}
				BOOL bChange = TRUE;
				CPoint ptOffset;
				
				switch (msg.wParam)
				{
				case VK_LEFT:
					ptOffset = CPoint(-sizeGrid.cx, 0);
					break;
				case VK_UP:
					ptOffset = CPoint(0, -sizeGrid.cy);
					break;
				case VK_RIGHT:
					ptOffset = CPoint(sizeGrid.cx, 0);
					break;
				case VK_DOWN:
					ptOffset = CPoint(0, sizeGrid.cy);
					break;
				default:
					bChange = FALSE;
					break;
				}
				if (bChange == FALSE)
					break;
				pView->ClientToDoc(pointNew);
				pointNew.Offset(ptOffset);
				pView->DocToClient(pointNew);

				pointTemp = pointNew;
				pView->ClientToScreen(&pointTemp);
				SetCursorPos(pointTemp.x, pointTemp.y);
			}
			else
			{
				pointNew = CPoint((int)(short)LOWORD(msg.lParam), (int)(short)HIWORD(msg.lParam));
				CPoint pt = pointNew;
				pView->ClientToDoc(pt);
				pView->AlignToGrid(pt);
				((CMainFrame *)(theApp.m_pMainWnd))->OnViewMousePosChange(pt);
			}
			bMouseUp = (msg.message == WM_LBUTTONUP);
			if (pointNew != pointOld || bMouseUp)
			{
				if (bMoved)
				{
					pointTemp = pointOld;
					pView->ClientToDoc(pointTemp);
					pointTemp -= pointOffset;
					pView->AlignToGrid(pointTemp);
					pObj->DrawTrackerHandleShape(pDrawDC, nHandle, CDevTool::c_enumTrackerState, pointTemp, pView);
				}
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;
			}
			if (bMouseUp)
			{
				pointOld = pointNew;
				goto ExitLoop;
			}

			if (pointNew != pointOld)
			{
				pointOld = pointNew;
				pointTemp = pointOld;
				pView->ClientToDoc(pointTemp);
				pointTemp -= pointOffset;
				pView->AlignToGrid(pointTemp);
				pObj->DrawTrackerHandleShape(pDrawDC, nHandle, CDevTool::c_enumTrackerState, pointTemp, pView);
			}
			break;

		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				pointTemp = pointOld;
				pView->ClientToDoc(pointTemp);
				pointTemp -= pointOffset;
				pView->AlignToGrid(pointTemp);
				pObj->DrawTrackerHandleShape(pDrawDC, nHandle, CDevTool::c_enumTrackerState, pointTemp, pView);
			}
			pointNew = pointSave;
			goto ExitLoop;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	pDrawDC->SelectObject(pOldPen);
	pView->ReleaseDC(pDrawDC);
	ReleaseCapture();

	AfxUnlockTempMaps(FALSE);

	if (pointSave != pointNew)
	{
		pointTemp = pointNew;
		pView->ClientToDoc(pointTemp);
		pointTemp -= pointOffset;
		pView->AlignToGrid(pointTemp);
		pObj->Invalidate(pView);

		CActionObj* pAct = new CActionObj(_T("�ص��ѱ�"), pView->GetDocument(), pObj);
		pView->GetDocument()->m_stackUndo.Push(pAct);

		pObj->MoveHandleTo(nHandle, CDevTool::c_enumTrackerState, pointTemp, pView);
		theApp.NotifySelectChanged(FALSE, 0, TRUE);
		pObj->m_pDocument->SetModifiedFlag();
	}
	pObj->Invalidate(pView);

	// return TRUE only if point has changed
	return (pointSave != pointNew);
}