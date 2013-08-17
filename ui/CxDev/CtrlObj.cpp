// CtrlObj.cpp: implementation of the CCtrlObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DevInc.h"

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

	m_nTabIndex = -1;
	m_bTabStop = TRUE;
	m_bDefault = FALSE;
	m_bCancel = FALSE;
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

	if (IsDrawVisible())
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
void CCtrlObj::MoveTo(const CRectF& position, BOOL bUniformScale, CLayoutView* pView)
{
	BOOL bSizeChanged;
	BOOL bSuccess;
	CRectF rcOldPosition;
	CSizeF size;

	ASSERT_VALID(this);

	CRectF rcPos = position;
	rcPos.NormalizeRect();
	if (rcPos == m_position)
		return;

	// ����ʱ���ɼ��ؼ�ʹ�ù̶��ߴ�
	if (m_pClientItem->IsInvisibleAtRuntime())
	{
		rcPos.right = rcPos.left + m_position.Width();
		rcPos.bottom = rcPos.top + m_position.Height();
	}

	bSizeChanged = (m_position.Size() != rcPos.Size());

	Invalidate(pView);
	rcOldPosition = m_position;
	m_position = rcPos;
	
//	if (m_pClientItem->IsInPlaceActive())
//		m_pClientItem->SetItemRects();

	if (bSizeChanged)
	{
		CClientDC dc(NULL);

		bSuccess = TRUE;
		size = m_position.Size();
		SIZE sz;
		sz.cx = int(size.cx);
		sz.cy = int(size.cy);
		dc.DPtoHIMETRIC(&sz);
		size.cx = (float)sz.cx;
		size.cy = (float)sz.cy;

		TRY
		{
			m_pClientItem->SetExtent(sz);
		}
		CATCH(CException, e)
		{
			bSuccess = FALSE;
		}
		END_CATCH

		if (!bSuccess)
		{
			m_position = rcOldPosition;
		}
	}

	m_position = rcPos;

	if (m_pClientItem->IsInPlaceActive())
		m_pClientItem->SetItemRects();
	
	Invalidate(pView);
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��õ�����ӿ�ָ��
////////////////////////////////////////////////////////////////////////
LPUNKNOWN CCtrlObj::GetObjectUnknown()
{
	if (m_pClientItem != NULL)
		return m_pClientItem->m_lpObject;
	return NULL;
}

void CCtrlObj::Invalidate(CLayoutView* pView)
{
//	HWND hWnd = m_pClientItem->GetInPlaceObjectWindow();
//	if (hWnd != NULL)
//		RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
	//	InvalidateRect(hWnd, NULL, TRUE);

	CDevObj::Invalidate(pView);

//	m_pClientItem->InvalidateItem();
}

////////////////////////////////////////////////////////////
//�� �ܣ��ͷſؼ�����
//�� ������Ҫ����Ŀ�ͷ�
////////////////////////////////////////////////////////////
void CCtrlObj::Release()
{
	if (m_pClientItem != NULL)
		m_pClientItem->Release();

	CDevObj::Release();
}

void CCtrlObj::Delete(BOOL bAutoDelete)
{
	m_nTabIndex = m_pDocument->SetItemTabOrder(m_pClientItem, -1);

	if (m_pClientItem != NULL)
	{
		m_pClientItem->Delete();
		m_pClientItem = NULL;
	}

	CDevObj::Delete(bAutoDelete);
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��ؼ��������л�
////////////////////////////////////////////////////////////////////////
void CCtrlObj::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	CDevObj::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_position;
		ar << m_pClientItem;

		if (m_dwVer >= _SUPERCX_VER_0206)
		{
			ar << m_nTabIndex;
			ar << m_bTabStop;
			ar << m_bDefault;
			ar << m_bCancel;
		}
	}
	else
	{
		ar >> m_position;
		CCtrlItem* pItem = NULL;
		ar >> pItem;

		if (m_dwVer >= _SUPERCX_VER_0206)
		{
			ar >> m_nTabIndex;
			ar >> m_bTabStop;
			ar >> m_bDefault;
			ar >> m_bCancel;
		}
		else
		{
			m_nTabIndex = m_pDocument->GetCtrlItemCount() - 1;
			m_bTabStop = !pItem->IsLabelControl();
		}

		CRectF position = m_position;
		FinishCreate(pItem, TRUE);
		MoveTo(position, FALSE);
	}

	//���л���̬����
	SerializeDynamicProperty(ar);
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��������󵽲ü���
////////////////////////////////////////////////////////////////////////
void CCtrlObj::CopyToClipboard(CArchive& ar, IStorage* pStorage)
{
	ar << m_position;
	ar << m_bTabStop;
	ar << m_bDefault;
	ar << m_bCancel;

	m_pClientItem->CopyToClipboard(ar, pStorage);

	//������̬����
	SerializeDynamicProperty(ar);

	CDevObj::CopyToClipboard(ar, pStorage);
}

////////////////////////////////////////////////////////////////////////
//�� �ܣ��Ӳü����д�������
//�� ����Ӧ���ڴ˴�����Ŀ����
////////////////////////////////////////////////////////////////////////
BOOL CCtrlObj::CreateFromClipboard(CArchive& ar, IStorage* pStorage)
{
	m_dwObjectNumber = GetNewObjectNumber();
	
	ar >> m_position;
	ar >> m_bTabStop;
	ar >> m_bDefault;
	ar >> m_bCancel;

	ASSERT_VALID(this);
	ASSERT(m_pDocument != NULL);
	CCtrlItem* pItem = new CCtrlItem(m_pDocument, this);
	ASSERT_VALID(pItem);

	if (!pItem->CreateFromClipboard(ar, pStorage))
		return FALSE;

	m_nTabIndex = m_pDocument->GetCtrlItemCount() - 1;
	m_bTabStop = !pItem->IsLabelControl();

	//������̬����
	SerializeDynamicProperty(ar);

	CDevObj::CreateFromClipboard(ar, pStorage);

	CRectF position = m_position;
	BOOL b = FinishCreate(pItem, TRUE);
	MoveTo(position, FALSE);

	return b;
}

/////////////////////////////////////////////////////////////////////////////
//�� �ܣ��������ߵ���ؼ�����
/////////////////////////////////////////////////////////////////////////////
BOOL CCtrlObj::Create(REFCLSID clsid)
{
	m_dwObjectNumber = GetNewObjectNumber();
	
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

	m_nTabIndex = m_pDocument->GetCtrlItemCount() - 1;
	m_bTabStop = !pItem->IsLabelControl();

	return FinishCreate(pItem, TRUE);
}

BOOL CCtrlObj::FinishCreate(CCtrlItem* pItem, BOOL bActive)
{
	ASSERT_VALID(pItem);

	m_pClientItem = pItem;
	m_pClientItem->m_pCtrlObj = this;

	TRY
	{
		m_pClientItem->UpdateExtent();
		//���˶���Ϊ����Ӷ���ʱ���¾䵼��ˢ�´���
		CFormDoc* pDoc = (CFormDoc*)m_pDocument;
		if (bActive && pDoc->IsKindOf(RUNTIME_CLASS(CFormDoc)))
		{
			CLayoutView* pView = pDoc->GetTopFormDoc()->GetLayoutView();
			if (pView != NULL)
			{
				m_pClientItem->DoVerb(OLEIVERB_INPLACEACTIVATE, pView);
				m_pClientItem->BringWindowTop();
			}
		}
	}
	CATCH_ALL(e)
	{
		pItem->Delete();
		m_pClientItem = NULL;

		return FALSE;
	}
	END_CATCH_ALL

	m_strClassName = m_pClientItem->GetNameOfClass();
	
	if (GetDisplayName() == "")
	{
		m_pDocument->CreateUniqueObjectName(this);

		//����Ĭ�ϲ���
		IUnknownPtr pUnk = GetObjectUnknown();
		IDispatchPtr pDsp = pUnk;
		if(pDsp != NULL)
		{
			DISPID id = 0;
			WCHAR* str = OLESTR("Caption");
			HRESULT hr = pDsp->GetIDsOfNames(IID_NULL, &str, 1, LOCALE_SYSTEM_DEFAULT, &id);
		
			if(SUCCEEDED(hr) && (id == -518 || id == 0))
			{
				VARIANT Arg;
				CString strCaption = GetDisplayName();
				Arg.bstrVal = strCaption.AllocSysString(); 
				Arg.vt = VT_BSTR;
				DISPID dispidNamed = DISPID_PROPERTYPUT;
		
				DISPPARAMS param;
				memset(&param, 0, sizeof(param));
				param.cArgs = 1;
				param.cNamedArgs = 1;
				param.rgdispidNamedArgs = &dispidNamed;
				param.rgvarg = &Arg;
		
				pDsp->Invoke(id, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &param, NULL, NULL, NULL);

				::SysFreeString(Arg.bstrVal);
			}
		}
	}

	return TRUE;	
}

/////////////////////////////////////////////////////////////////////////////
//����:ȡ�ؼ���Ӧ������ҳ��CLSID��
/////////////////////////////////////////////////////////////////////////////
BOOL CCtrlObj::GetPropertyPageCLSIDs(CArray< CLSID, CLSID& >& aclsidPages)
{
	return m_pClientItem->GetPropertyPageCLSIDs(aclsidPages);
}

void CCtrlObj::SetTabIndex(int nTabIndex)
{
	if (nTabIndex < 0)
		nTabIndex = 0;

	m_nTabIndex = m_pDocument->SetItemTabOrder(m_pClientItem, nTabIndex);
}

void CCtrlObj::SetChildObjectDocument()
{
	if (m_pClientItem != NULL)
	{
		m_pClientItem->m_pDocument->RemoveItem(m_pClientItem);
		m_pDocument->AddItem(m_pClientItem);
	}
}