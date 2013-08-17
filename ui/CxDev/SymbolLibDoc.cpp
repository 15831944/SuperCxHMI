// SymbolLibDoc.cpp : implementation file

#include "stdafx.h"
#include "DevInc.h"
#include "devobjdoc.h"
#include "SymbolLibDoc.h"

#include "direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSymbolLibDoc

LPTSTR CSymbolLibDoc::m_szGLibFileExt = _T("slb");

IMPLEMENT_DYNCREATE(CSymbolLibDoc, CDevObjDoc)

CSymbolLibDoc::CSymbolLibDoc()
{
	AfxOleUnlockApp();
	m_bAutoDelete = FALSE;
}

BOOL CSymbolLibDoc::OnNewDocument()
{
	if (!CDevObjDoc::OnNewDocument())
		return FALSE;

	return TRUE;
}

CSymbolLibDoc::~CSymbolLibDoc()
{
	AfxOleLockApp();
}


BEGIN_MESSAGE_MAP(CSymbolLibDoc, CDevObjDoc)
	//{{AFX_MSG_MAP(CSymbolLibDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibDoc diagnostics

#ifdef _DEBUG
void CSymbolLibDoc::AssertValid() const
{
	CDevObjDoc::AssertValid();
}

void CSymbolLibDoc::Dump(CDumpContext& dc) const
{
	CDevObjDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibDoc serialization

void CSymbolLibDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		m_dwVer = _SUPERCX_VER;
		ar << m_dwVer;
	}
	else
	{
		ar >> m_dwVer;
	}
	CDevObjDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibDoc commands

////////////////////////////////////////////////////////
//����:ȡĬ�ϵ�ͼ��·��
////////////////////////////////////////////////////////
CString CSymbolLibDoc::GetDefaultPath()
{	
	CString strRet = theApp.GetCurrentAppDirectory();
	strRet += _T("Symbol");
	return strRet;
}

////////////////////////////////////////////////////////
//����:���������ĵ�
//����:strLibName[in]		ͼ����
//����:�ɹ�ͼ���ĵ�ָ��,ʧ��NULL
/////////////////////////////////////////////////////////
void CSymbolLibDoc::RenameLib(LPCTSTR lpszOldName, LPCTSTR lpszNewName)
{
	CString strPath = GetDefaultPath();
	strPath += _T("\\");

	CString strOldName = strPath + lpszOldName + _T(".") + m_szGLibFileExt;
	CString strNewName = strPath + lpszNewName + _T(".") + m_szGLibFileExt;

	CFile::Rename(strOldName, strNewName);
}

////////////////////////////////////////////////////////
//����:��Ϊ���ĵ�
//����:strLibName[in]		ͼ����
//����:�ɹ�ͼ���ĵ�ָ��,ʧ��NULL
/////////////////////////////////////////////////////////
BOOL CSymbolLibDoc::OpenLib(CString strLibName)
{
	CString strPath = GetDefaultPath();
	strPath += _T("\\");
	strPath += strLibName + _T(".");
	strPath += m_szGLibFileExt;
	m_strPathName = strPath;
	return OnOpenDocument(strPath);
}


///////////////////////////////////////////////////////////
//����:���浱ǰͼ���ļ�������
//����:�ɹ�TRUE, ʧ��FALSE
///////////////////////////////////////////////////////////
BOOL CSymbolLibDoc::SaveLib()
{
	return DoSave(m_strPathName, TRUE);
}

///////////////////////////////////////////////////////////
//����:�����µ�ͼ��
//����:strName[in]		ͼ����
//����:�ɹ�ͼ���ĵ�ָ��,ʧ��NULL
////////////////////////////////////////////////////////////
CSymbolLibDoc* CSymbolLibDoc::NewLib(CString &strName)
{
	CSymbolLibDoc* pDoc = new CSymbolLibDoc();
	if (pDoc == NULL)
		AfxThrowMemoryException();
	if (!pDoc->OnNewDocument())
		return NULL;

	CString strNameUse = GetDefaultPath();
	strNameUse += _T("\\");
	strNameUse += strName + ".";
	strNameUse += m_szGLibFileExt;

	pDoc->DoSave(strNameUse, TRUE);
	pDoc->SetPathName(strNameUse, FALSE);

	return pDoc;
}


//////////////////////////////////////////////////////
//����:�ر��ĵ�
//����:�ɹ�TRUE,ʧ��FALSE
//////////////////////////////////////////////////////
BOOL CSymbolLibDoc::CloseLib()
{
	OnCloseDocument();
	DeleteContents();
	return TRUE;
}

//////////////////////////////////////////////////////
//����:���ص�ǰͼ���еĶ�������
//////////////////////////////////////////////////////
int CSymbolLibDoc::GetObjectCount()
{
	return m_objects.GetCount();
}

//////////////////////////////////////////////////////
//����:���strLibNameָ����ͼ��
//����:strLibName[in]		ͼ����
//����:�ɹ�TRUE,ʧ��FALSE
//////////////////////////////////////////////////////
BOOL CSymbolLibDoc::DeleteLib(CString &strLibName)
{
	CString strPath = GetDefaultPath();
	strPath += "\\";
	strPath += strLibName + ".";
	strPath += m_szGLibFileExt;

	return DeleteFile(strPath);
}

BOOL CSymbolLibDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
	CString newName = lpszPathName;
	if (newName.IsEmpty())
	{
		ASSERT(FALSE);		//���Ʊ������
	}

	CWaitCursor wait;

	if (!OnSaveDocument(newName))
	{
		if (lpszPathName == NULL)
		{
			// be sure to delete the file
			TRY
			{
				CFile::Remove(newName);
			}
			CATCH_ALL(e)
			{
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	return TRUE;        // success
}

///////////////////////////////////////////////////////////
//����:�ӿ���ɾ��ָ������
//����:�ɹ�TRUE, ʧ��FALSE
///////////////////////////////////////////////////////////
BOOL CSymbolLibDoc::DeleteObject(CDevObj *pObj)
{
	POSITION pos = m_objects.GetHeadPosition();
	while (pos != NULL)
	{
		CDevObj* pSearchObj = (CDevObj*)(m_objects.GetNext(pos));
		if (pObj == pSearchObj)
		{
			pSearchObj->Delete();
			return TRUE;
		}
	}

	return FALSE;
}

CDevObj* CSymbolLibDoc::ConvertObjectToSymbol(CDevObj* pObj)
{	
	CSymbolObj* pSymbolObj = AddSymbolObj(NULL);
	ASSERT_VALID(pSymbolObj);
	pSymbolObj->AddSubObject(pObj);
	
	POSITION pos = m_objects.Find(pSymbolObj);
	m_objects.RemoveAt(pos);
	pos = m_objects.Find(pObj);
	m_objects.SetAt(pos, pSymbolObj);
	
	return pSymbolObj;
}