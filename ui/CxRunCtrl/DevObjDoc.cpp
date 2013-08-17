// DevObjDoc.cpp : implementation file
//���ڹ���CDrawObj������ĵ�������
//����:CxDev
//����:��Խ��
//ʱ��:2001.12.3
//˵��:���ĵ��������Ӵ��ĵ�����Ļ���
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RunInc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDevObjDoc

IMPLEMENT_DYNCREATE(CDevObjDoc, COleDocument)

CDevObjDoc::CDevObjDoc()
{
	m_dwVer = 0;
	m_dwNextObjectNumber = 0;
	m_bObjectsIsDeleting = FALSE;

	EnableCompoundFile();
	EnableAutomation();

	AfxOleLockApp();
}

BOOL CDevObjDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDevObjDoc::~CDevObjDoc()
{
	AfxOleUnlockApp();
}


BEGIN_MESSAGE_MAP(CDevObjDoc, COleDocument)
	//{{AFX_MSG_MAP(CDevObjDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevObjDoc diagnostics

#ifdef _DEBUG
void CDevObjDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CDevObjDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDevObjDoc serialization

void CDevObjDoc::Serialize(CArchive& ar)
{
	ASSERT(ar.IsLoading());
	m_objects.Serialize(ar);		//�������л�

	COleDocument::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CDevObjDoc commands
//�����ĵ���Ψһ�Ķ�����
void CDevObjDoc::CreateUniqueObjectName(CDevObj* pObj,
	LPCTSTR pszBaseName, CString& strUniqueName)
{
	CDevObj* pSearchObj;
	ULONG nTag;
	BOOL bCollisionFound;

	ASSERT(pszBaseName != NULL);

	// Assume the name is already unique
	strUniqueName.Format("%s%u", pszBaseName, 1);

	nTag = 1;
	do
	{
		bCollisionFound = FALSE;

		if (m_mapNamedObjects.Lookup(strUniqueName, pSearchObj))
		{
			bCollisionFound = TRUE;
			strUniqueName.Format("%s%u", pszBaseName, nTag);
			nTag++;
		}
	} while (bCollisionFound);

	m_mapNamedObjects.SetAt(strUniqueName, pObj);
}


//��Ӷ��󵽶���������
void CDevObjDoc::AddObject(CDevObj* pObj)
{
	ASSERT(pObj->m_pDocument == NULL);     // not yet initialized
	m_objects.AddTail(pObj);
	pObj->m_pDocument = this;
 
	ASSERT_VALID(pObj);    // now it must be valid
}

//�Ӷ���������ɾ�����󣬵�����ʵ��ɾ������
void CDevObjDoc::RemoveObject(CDevObj* pObj)
{
	ASSERT_VALID(pObj);    // must be valid before detach
	ASSERT(pObj->m_pDocument == this);     // formerly attached

	ASSERT(m_objects.Find(pObj) != NULL);  // must be in list
	m_objects.RemoveAt(m_objects.Find(pObj));
	ASSERT(m_objects.Find(pObj) == NULL);  // must not be in list now
	pObj->m_pDocument = NULL;
}

//////////////////////////////////////////////////////////////////////
//���ܣ�����DISPID���һ�������
//////////////////////////////////////////////////////////////////////
CAmbientProperty* CDevObjDoc::FindAmbientProperty(DISPID dispid)
{
	int iProp;

	for (iProp = 0; iProp < m_apAmbientProperties.GetSize(); iProp++)
	{
		if (m_apAmbientProperties[iProp]->GetID() == dispid)
		{
			return m_apAmbientProperties[iProp];
		}
	}

	// We didn't find the property.
	return NULL;
}

//////////////////////////////////////////////////////////////////////
//���ܣ��������ֲ��һ�������
//////////////////////////////////////////////////////////////////////
CAmbientProperty* CDevObjDoc::FindAmbientProperty( LPCOLESTR pszName )
{
	USES_CONVERSION;
	int iProp;
	LPCTSTR pszNameT;

	pszNameT = OLE2CT(pszName);
	for (iProp = 0; iProp < m_apAmbientProperties.GetSize(); iProp++)
	{
		if (m_apAmbientProperties[iProp]->GetName() == pszNameT)
		{
			return m_apAmbientProperties[iProp];
		}
	}

	// We didn't find the property.
	return NULL;
}

///////////////////////////////////////////////////////
//����:ȡ�ĵ��е�һ�������λ��
///////////////////////////////////////////////////////
POSITION CDevObjDoc::GetFirstObjPosition()
{
	return m_objects.GetHeadPosition();
}

//////////////////////////////////////////////////////////
//����:ȡposλ�õĶ���ָ��,���ƶ�pos����һ������,��NULL
//����:pos[in, out]		����λ��, ����ʱΪ��һ�������λ��
//����:�����NULLָ��
//////////////////////////////////////////////////////////
CDevObj* CDevObjDoc::GetNextObj(POSITION &pos)
{
	return m_objects.GetNext(pos);
}

//////////////////////////////////////////////////////////
//����:ȡposλ�õĶ���ָ��,���ƶ�pos��ǰһ������,��NULL
//����:pos[in, out]		����λ��, ����ʱΪǰһ�������λ��
//����:�����NULLָ��
//////////////////////////////////////////////////////////
CDevObj* CDevObjDoc::GetPrevObj(POSITION &pos)
{
	return m_objects.GetPrev(pos);
}

CDevObjList* CDevObjDoc::GetObjects() 
{ 
	// 2011�� �����Ѿ����ͷ�ʱ���������ƶ�ʱ����õ�������

	static CDevObjList emptyObjList;

	if (!m_bObjectsIsDeleting)
		return &m_objects; 

	return &emptyObjList;
}

void CDevObjDoc::DeleteContents() 
{
	m_bObjectsIsDeleting = TRUE;

	//�ڴ��ͷŶ���
	POSITION pos = m_objects.GetHeadPosition();
	while (pos != NULL)
	{
		CDevObj* pObj = m_objects.GetNext(pos);
		pObj->Release();
		delete pObj;
	}
	m_objects.RemoveAll();

	m_bObjectsIsDeleting = FALSE;

	COleDocument::DeleteContents();
}

CView* CDevObjDoc::GetRunView() const
{
	return NULL;
}

CFormScrEng* CDevObjDoc::GetScriptEngine()
{
	return NULL;
}

void CDevObjDoc::UpdateAllAssocViews(CView* pSender, LPARAM lHint, CObject* pHint)
{
}

void CDevObjDoc::UpdateAllViews(CView* pSender, LPARAM lHint, CObject* pHint)
{
	UpdateAllAssocViews(pSender, lHint, pHint);

	COleDocument::UpdateAllViews(pSender, lHint, pHint);
}

int CDevObjDoc::SetItemTabOrder(CCtrlItem* pItem, int nTabOrder)
{
	ATLASSERT(GetCtrlItemCount() > 0);

	if (nTabOrder >= GetCtrlItemCount())
		nTabOrder = GetCtrlItemCount() - 1;

	int nOldTabOrder = pItem->m_pCtrlObj->m_nTabIndex;
	
	POSITION posItem = GetStartPosition();
	while (posItem != NULL)
	{
		CCtrlItem* pItemT = (CCtrlItem *)GetNextItem(posItem);
		if (pItemT != NULL)
		{
			CCtrlObj* pCtrlObj = pItemT->m_pCtrlObj;
			ATLASSERT(pCtrlObj != NULL);

			if (pItemT == pItem)
			{
				pCtrlObj->m_nTabIndex = nTabOrder;
			}
			else
			{
				if (pCtrlObj->m_nTabIndex > nOldTabOrder)
					pCtrlObj->m_nTabIndex--;
				if (pCtrlObj->m_nTabIndex >= nTabOrder && nTabOrder >= 0)
					pCtrlObj->m_nTabIndex++;
			}
		}
	}

	return nTabOrder;
}

CCtrlItem* CDevObjDoc::GetNextTabItem(CCtrlItem* pItem, BOOL bPrevious)
{
	int nTabOrder = 0;
	CCtrlObj* pFindObj = NULL;

	if (pItem != NULL)
		nTabOrder = pItem->m_pCtrlObj->m_nTabIndex;

	POSITION posItem = GetStartPosition();
	while (posItem != NULL)
	{
		CCtrlItem* pItemT = (CCtrlItem *)GetNextItem(posItem);
		if (pItemT != NULL)
		{
			CCtrlObj* pCtrlObj = pItemT->m_pCtrlObj;
			ATLASSERT(pCtrlObj != NULL);

			if (!pCtrlObj->m_bTabStop || !pItemT->IsEnabled())
				continue;

			if (pItemT == pItem)
				continue;

			if (pFindObj == NULL)
				pFindObj = pCtrlObj;

			if (!bPrevious)
			{
				if (pFindObj->m_nTabIndex < nTabOrder && pCtrlObj->m_nTabIndex > nTabOrder)
				{
					pFindObj = pCtrlObj;
				}
				else if ((pCtrlObj->m_nTabIndex >= nTabOrder) ||
					(pCtrlObj->m_nTabIndex < nTabOrder && pFindObj->m_nTabIndex < nTabOrder))
				{
					if (pFindObj->m_nTabIndex > pCtrlObj->m_nTabIndex)
						pFindObj = pCtrlObj;
				}
			}
			else
			{
				if (pFindObj->m_nTabIndex > nTabOrder && pCtrlObj->m_nTabIndex < nTabOrder)
				{
					pFindObj = pCtrlObj;
				}
				else if ((pCtrlObj->m_nTabIndex <= nTabOrder) ||
					(pCtrlObj->m_nTabIndex > nTabOrder && pFindObj->m_nTabIndex > nTabOrder))
				{
					if (pFindObj->m_nTabIndex < pCtrlObj->m_nTabIndex)
						pFindObj = pCtrlObj;
				}
			}
		}
	}
		
	return pFindObj != NULL ? pFindObj->m_pClientItem : NULL;
}

CCtrlItem* CDevObjDoc::GetDefaultItem()
{
	if (m_bObjectsIsDeleting)
		return NULL;

	CCtrlObj* pFindObj = NULL; 

	POSITION posItem = GetStartPosition();
	while (posItem != NULL)
	{
		CCtrlItem* pItem = (CCtrlItem *)GetNextItem(posItem);
		if (pItem != NULL)
		{
			CCtrlObj* pCtrlObj = pItem->m_pCtrlObj;
			ATLASSERT(pCtrlObj != NULL);

			if (pCtrlObj->m_bDefault)
			{
				if (pFindObj == NULL || pCtrlObj->m_nTabIndex < pFindObj->m_nTabIndex)
					pFindObj = pCtrlObj;
			}
		}
	}

	return pFindObj != NULL ? pFindObj->m_pClientItem : NULL;
}

CCtrlItem* CDevObjDoc::GetCancelItem()
{
	if (m_bObjectsIsDeleting)
		return NULL;

	CCtrlObj* pFindObj = NULL; 

	POSITION posItem = GetStartPosition();
	while (posItem != NULL)
	{
		CCtrlItem* pItem = (CCtrlItem *)GetNextItem(posItem);
		if (pItem != NULL)
		{
			CCtrlObj* pCtrlObj = pItem->m_pCtrlObj;
			ATLASSERT(pCtrlObj != NULL);

			if (pCtrlObj->m_bCancel)
			{
				if (pFindObj == NULL || pCtrlObj->m_nTabIndex < pFindObj->m_nTabIndex)
					pFindObj = pCtrlObj;
			}
		}
	}

	return pFindObj != NULL ? pFindObj->m_pClientItem : NULL;
}