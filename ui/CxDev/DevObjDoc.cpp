// DevObjDoc.cpp : implementation file
//���ڹ���CDrawObj������ĵ�������
//����:CxDev
//����:��Խ��
//ʱ��:2001.12.3
//˵��:���ĵ��������Ӵ��ĵ�����Ļ���
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DevInc.h"
#include "cxdev.h"
#include "DevObjDoc.h"

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
	EnableCompoundFile();
	EnableAutomation();

	m_pMapClassNameIndex = NULL;

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
	//�������л�
	if (ar.IsStoring())
	{
		// ���ݶ������ڲ��������к�洢
		CDevObjList objects;
		for (int i = 0; i < 30; i++)
		{
			POSITION posObj = m_objects.GetHeadPosition();
			while (posObj != NULL)
			{
				CDevObj* pObj = m_objects.GetNext(posObj);
				if (pObj->m_nLayer == i)
					objects.AddTail(pObj);
			}
		}

		objects.Serialize(ar);
	}
	else
	{
		m_objects.Serialize(ar);
	}
	
	COleDocument::Serialize(ar);
}

void CDevObjDoc::CopyToClipboard(CArchive& ar, IStorage* pStorage)
{
	int nCount = m_objects.GetCount();
	ar << nCount;

	CDevObj* pObj;
	POSITION pos = m_objects.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = m_objects.GetNext(pos);
		ar << int(pObj->GetObjectType());
		pObj->CopyToClipboard(ar, pStorage);
	}
}

BOOL CDevObjDoc::CreateFromClipboard(CArchive& ar, IStorage* pStorage)
{
	int nObjType;
	int nCount;
	CDevObj* pObj;

	ar >> nCount;
	for (int i = 0; i < nCount; i++)
	{
		ar >> nObjType;
		switch (nObjType)
		{
		case CDevObj::drawobj:
			pObj = new CDrawObj(this);
			break;
		case CDevObj::groupobj:
			pObj = new CGroupObj(this);
			break;
		case CDevObj::symbolobj:
			pObj = new CSymbolObj(this);
			break;
		case CDevObj::ctrlobj:
			pObj = new CCtrlObj(this);
			break;
		case CDevObj::formobj:
			pObj = new CFormObj(this);
			break;
		case CDevObj::reportobj:
			pObj = new CReportObj(this);
			break;
		default:
			ASSERT(FALSE);
		}

		if (!pObj->CreateFromClipboard(ar, pStorage))
		{
			AfxThrowOleException(E_FAIL);
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDevObjDoc commands
//�����ĵ���Ψһ�Ķ�����
void CDevObjDoc::CreateUniqueObjectName(CDevObj* pObj, LPCTSTR lpszBaseName)
//	LPCTSTR pszBaseName, CString& strUniqueName)
{
	ASSERT(pObj != NULL);
	CDevObj* pSearchObj;
	ULONG nTag;
	BOOL bCollisionFound;
	CString strBaseName;
	CString strUniqueName;

	if (lpszBaseName != NULL)
		strBaseName = lpszBaseName;
	else
		strBaseName = pObj->GetClassName();
	CString strDocName = GetDisplayName();

	// �����ڱ��в�����������ţ����û�У���1��ʼ��
	if (m_pMapClassNameIndex == NULL || !m_pMapClassNameIndex->Lookup(strBaseName, (void *&)nTag))
		nTag = 1;

	do
	{
		strUniqueName.Format("%s%u", strBaseName, nTag);

		bCollisionFound = FALSE;
		nTag++;

		if (m_mapNamedObjects.LookupNoCase(strUniqueName, pSearchObj) || strUniqueName == strDocName)
			bCollisionFound = TRUE;
	} while (bCollisionFound);

	if (m_pMapClassNameIndex != NULL)
		m_pMapClassNameIndex->SetAt(strBaseName, (void *)nTag);

	m_mapNamedObjects.SetAt(strUniqueName, pObj);
	OnSymbolChanged(UPDATE_SYMBOL_OBJECT);

	pObj->PutDisplayName(strUniqueName);
}

BOOL CDevObjDoc::IsValidObjectName(LPCTSTR lpszName)
{
	return IsValidVBScriptVar(lpszName);
}

//��Ӷ��󵽶���������
void CDevObjDoc::AddObject(CDevObj* pObj)
{
	ASSERT(pObj->m_pDocument == NULL);     // not yet initialized
	m_objects.AddTail(pObj);
	pObj->m_pDocument = this;
	SetModifiedFlag();
 
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

void CDevObjDoc::DeleteContents() 
{
	//�ڴ��ͷŶ���
	POSITION pos = m_objects.GetHeadPosition();
	while (pos != NULL)
	{
		CDevObj* pObj = m_objects.GetNext(pos);
		pObj->Release();
		delete pObj;
	}
	m_objects.RemoveAll();
	
	COleDocument::DeleteContents();
}

CSymbolObj* CDevObjDoc::AddSymbolObj(LPCTSTR pszName, REFIID iidPersistanceMedium, IUnknown* pPersistanceMedium)
{
	CSymbolObj* pObj;

	pObj = NULL;
	try
	{
		pObj = new CSymbolObj(this);
		ASSERT_VALID(pObj);

		if (!pObj->CreateOrLoad(pszName, iidPersistanceMedium,
			pPersistanceMedium))
		{
			AfxThrowOleException(E_FAIL);
		}
		ASSERT_VALID(pObj);
	}
	catch( ... )
	{
		pObj->Delete();

		throw;
	}
	ASSERT(pObj != NULL);

	return pObj;
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

CCtrlItem* CDevObjDoc::GetNextTabItem(CCtrlItem* pItem)
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

			if (!pCtrlObj->m_bTabStop)
				continue;

			if (pItemT == pItem)
				continue;

			if (pFindObj == NULL)
				pFindObj = pCtrlObj;

			if (pFindObj->m_nTabIndex < nTabOrder && pCtrlObj->m_nTabIndex > nTabOrder)
			{
				pFindObj = pCtrlObj;
			}
			else if ((pCtrlObj->m_nTabIndex > nTabOrder) ||
				(pCtrlObj->m_nTabIndex < nTabOrder && pFindObj->m_nTabIndex < nTabOrder))
			{
				if (pFindObj->m_nTabIndex > pCtrlObj->m_nTabIndex)
					pFindObj = pCtrlObj;
			}
		}
	}
		
	return pFindObj != NULL ? pFindObj->m_pClientItem : NULL;
}