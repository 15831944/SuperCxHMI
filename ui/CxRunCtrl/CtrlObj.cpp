// CtrlObj.cpp: implementation of the CCtrlObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RunInc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CCtrlObj, CDevObj, 0)

CCtrlObj::CCtrlObj(CDevObjDoc* pContainer) :
	CDevObj(pContainer)
{
	m_position = CRect(10, 10, 10, 10);
	m_pClientItem = NULL;
}

CCtrlObj::~CCtrlObj()
{
//	if (m_pClientItem != NULL)
//	{
//		m_pClientItem->Release();
//		delete m_pClientItem;
//		m_pClientItem = NULL;
//	}
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ���ǰ������ƿؼ�����
////////////////////////////////////////////////////////////////////////
void CCtrlObj::DrawFrontToBack(CDC* pDC)
{
    ASSERT_VALID(this);

	CCtrlItem* pItem = m_pClientItem;
	if (pItem != NULL)
	{
		// draw the OLE item itself
		pItem->DrawFrontToBack(pDC, &m_position.GetRECT());
	}
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��Ӻ�ǰ���ƿؼ�����
////////////////////////////////////////////////////////////////////////
void CCtrlObj::DrawBackToFront(CDC* pDC)
{
	ASSERT_VALID(this);

	CCtrlItem* pItem = m_pClientItem;
	if (pItem != NULL)
	{
		// draw the OLE item itself
		pItem->DrawBackToFront(pDC, &m_position.GetRECT());
	}
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ����ƿؼ�����
////////////////////////////////////////////////////////////////////////
void CCtrlObj::Draw(CDC* pDC)
{
	ASSERT_VALID(this);

	if (m_bVisible)
	{
		CCtrlItem* pItem = m_pClientItem;
		if (pItem != NULL)
		{
			// draw the OLE item itself
			pItem->Draw(pDC, &m_position.GetRECT());
		}
	}
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��ƶ��ؼ�����
////////////////////////////////////////////////////////////////////////
void CCtrlObj::MoveTo(const CRectF& position, BOOL bUniformScale, CRunView* pView)
{
	BOOL bSizeChanged;
	ASSERT_VALID(this);

	if (position == m_position)
		return;

	Invalidate(pView);

	bSizeChanged = (m_position.Size() != position.Size());
	m_position = position;
	
	if (bSizeChanged)
		SetExtent(pView);

	if (m_pClientItem->IsInPlaceActive())
		m_pClientItem->SetItemRects();

	Invalidate(pView);
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��õ�����ӿ�ָ��
////////////////////////////////////////////////////////////////////////
LPUNKNOWN CCtrlObj::GetObjectUnknown()
{
	return m_pClientItem->m_lpObject;
}

IDispatch* CCtrlObj::GetObjectDispatch()
{
	return m_pClientItem->m_pDispatch;
}

void CCtrlObj::Invalidate(CRunView* pView)
{
	CDevObj::Invalidate(pView);
//	m_pClientItem->InvalidateItem();
}

////////////////////////////////////////////////////////////
//�� �ܣ��ͷſؼ�����
//�� ������Ҫ����Ŀ�ͷ�
////////////////////////////////////////////////////////////
void CCtrlObj::Release()
{
	CDevObj::Release();

	if (m_pClientItem != NULL)
		m_pClientItem->Release();
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��ؼ��������л�
////////////////////////////////////////////////////////////////////////
void CCtrlObj::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);
	ASSERT(ar.IsLoading());

	CDevObj::Serialize(ar);

	ar >> m_position;
	CCtrlItem* pItem = NULL;
	ar >> pItem;

	if (m_dwVer >= _SUPERCX_VER_0206)
	{
		ar >> m_nTabIndex;
		ar >> m_bTabStop;
		ar >> m_bDefault;
		ar >> m_bCancel;

// 		if (m_bDefault)
// 			ASSERT(FALSE);
	}
	else
	{
		m_nTabIndex = m_pDocument->GetCtrlItemCount();
		m_bTabStop = !pItem->IsLabelControl();
		m_bDefault = FALSE;
		m_bCancel = FALSE;
	}

	FinishCreate(pItem);

	//���л���̬����
	SerializeDynamicProperty(ar);
}

/////////////////////////////////////////////////////////////////////////////
//�� �ܣ��������ߵ���ؼ�����
/////////////////////////////////////////////////////////////////////////////
BOOL CCtrlObj::Create(REFCLSID clsid)
{
	CCtrlItem* pItem = new CCtrlItem(m_pDocument);

	try
	{
		if (!pItem->CreateOrLoad(clsid, IID_NULL, NULL))
		{
			AfxThrowOleException(E_FAIL);
		}
	}
	catch (...)
	{
		pItem->Delete();
		throw;
	}

	return FinishCreate(pItem);
}

BOOL CCtrlObj::FinishCreate(CCtrlItem* pItem)
{
	ASSERT_VALID(pItem);

	m_pClientItem = pItem;
	m_pClientItem->m_pCtrlObj = this;

	return TRUE;	
}

void CCtrlObj::SetExtent(CRunView* pView)
{
	if (pView == NULL)
		pView = (CRunView *)((CFormDoc*)m_pDocument)->GetTopFormDoc()->GetRunView();
	ASSERT(pView != NULL);
	if (pView == NULL)
		return;
	
	CSizeF sizeF = m_position.Size();
	CSize size;
	size.cx = int(sizeF.cx);
	size.cy = int(sizeF.cy);
	pView->DocToClient(size);

	CClientDC dc(NULL);
	dc.DPtoHIMETRIC(&size);

	BOOL bSuccess = TRUE;
	TRY
	{
		CRectF position = m_position; // �����ڸı��ȵ�ʱ��ͨ��CCtrlItem::OnChangeItemPosition�ı�λ��
		m_pClientItem->SetExtent(size);
		m_position = position;
		m_pClientItem->UpdateExtent();
	}
	CATCH(CException, e)
	{
		bSuccess = FALSE;
	}
	END_CATCH
}

void CCtrlObj::OnShow(CRunView* pView)
{
	if (m_pClientItem->ShouldActivateWhenVisible())
	{
		if (m_pClientItem->IsInvisibleAtRuntime() || !m_bVisible)
		{
			m_pClientItem->DoVerb(OLEIVERB_HIDE, pView);
		}
		else
		{
			m_pClientItem->DoVerb(OLEIVERB_INPLACEACTIVATE, pView);
		}
	}
}

void CCtrlObj::Active(CRunView* pView)
{
	if (pView == NULL)
		pView = (CRunView *)((CFormDoc*)m_pDocument)->GetTopFormDoc()->GetRunView();
	ASSERT(pView != NULL);
	if (pView == NULL)
		return;

	SetExtent(pView);

	OnShow(pView);

	CDevObj::Active(pView);
}

void CCtrlObj::Deactive()
{

}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��ؼ���������Ϣ
//�� ����2������Ƕ����ؼ���������CRunView���ã�����Ϊtrue��
//����Ϊ�������Ƕ�׵��ã�����Ϊfalse
//�� �����ؼ�����Ŀǰ��֧������������붯̬���ԣ����Բ�������bSubObj
//Ϊtrue�����
BOOL CCtrlObj::OnWindowMessage(const MSG* pMessage, BOOL bSubObj)
{
	if (!bSubObj && m_pClientItem->IsWindowless())
		return m_pClientItem->OnWindowMessage(pMessage);
	
	return FALSE;
}

BOOL CCtrlObj::TranslateAccelerator(const MSG* pMessage)
{
	IOleInPlaceActiveObjectPtr pInPlaceActiveObject;

	pInPlaceActiveObject = m_pClientItem->m_lpObject;
	if (pInPlaceActiveObject != NULL)
	{
		HRESULT hr = pInPlaceActiveObject->TranslateAccelerator((MSG *)pMessage);
		if (hr == S_OK)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CCtrlObj::HitTest(CPoint point, const CRunView* pView)
{
	if (m_bVisible)
	{
		if (m_pClientItem->HitTest(point) == HITRESULT_HIT)
			return TRUE;
	}
	
	return FALSE;
}

void CCtrlObj::SetTabIndex(int nTabIndex)
{
	if (nTabIndex < 0)
		nTabIndex = 0;

	m_nTabIndex = m_pDocument->SetItemTabOrder(m_pClientItem, nTabIndex);
}