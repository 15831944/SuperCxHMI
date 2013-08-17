// Action.cpp: implementation of the CAction class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DevInc.h"
#include "Action.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBufferPool::CBufferPool()
{
	m_pStorage = NULL;
	m_dwNextActionNumber = 0;
}

CBufferPool::~CBufferPool()
{
	if (m_pStorage != NULL)
		m_pStorage->Release();
}

/////////////////////////////////////////////////////////////
//����:��ʼ��
/////////////////////////////////////////////////////////////
HRESULT CBufferPool::Init()
{
	if (m_pStorage)
		return S_OK;

	return ::StgCreateDocfile(/*L"D:\\temp.dat"*/NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_TRANSACTED|
		STGM_DELETEONRELEASE , 0, &m_pStorage); 
}

/////////////////////////////////////////////////////////////
//����:ȡ��һ�����غŵı��
/////////////////////////////////////////////////////////////
DWORD CBufferPool::GetNextActionNumber()
{
	return m_dwNextActionNumber++;
}

static OLECHAR szStream[] = L"Contents";
//////////////////////////////////////////////////////////////
//����:Ϊָ���Ļ��˶��󴴽���
//////////////////////////////////////////////////////////////
IStorage* CBufferPool::CreateActionStorage(CAction* pAction, IStream** ppStream, int n)
{
	VERIFY(Init() == S_OK);
	ASSERT(pAction != NULL);
	ASSERT(ppStream != NULL);

	if (pAction->m_dwActionNumber == -1)
		ASSERT(FALSE);		//δ��ʼ��

	*ppStream = NULL;

	//������Ӧ������
	LPSTORAGE pStorage = NULL;
	WCHAR strName[256];
	swprintf(strName, L"Action%dNo%d", pAction->m_dwActionNumber, n);

	//׼������
	HRESULT hr = m_pStorage->CreateStorage(strName, STGM_CREATE | STGM_READWRITE |  
		STGM_SHARE_EXCLUSIVE, 0, 0, &pStorage);
	if (FAILED(hr))
		AfxThrowOleException(hr);

	hr = pStorage->CreateStream(szStream, STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 
		0, 0, ppStream);
	if (FAILED(hr))
	{
		pStorage->Release();
		AfxThrowOleException(hr);
	}

	return pStorage;
}

//////////////////////////////////////////////////////////////
//����:Ϊָ���ļ��˶�����Ѵ�����
//����:pAction[in]		Ҫ�򿪴���Ļ��˶���
//		ppStream[out]	�򿪵���
//����:Ϊ֮�򿪵Ĵ���
//////////////////////////////////////////////////////////////
IStorage* CBufferPool::OpenActionStorage(CAction* pAction, IStream** ppStream, int n)
{
	ASSERT(m_pStorage != NULL);
	ASSERT(pAction != NULL);
	ASSERT(ppStream != NULL);
	if (pAction->m_dwActionNumber == -1)
		ASSERT(FALSE);		//δ��ʼ��
	
	*ppStream = NULL;
	
	//������Ӧ����
	LPSTORAGE pStorage = NULL;
	WCHAR strName[256];
	swprintf(strName, L"Action%dNo%d", pAction->m_dwActionNumber, n);

	//׼������
	HRESULT hr = m_pStorage->OpenStorage(strName, NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStorage);
	if (FAILED(hr))
		AfxThrowOleException(hr);
	hr = pStorage->OpenStream(szStream, 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, ppStream);
	if (FAILED(hr))
	{
		pStorage->Release();
		AfxThrowOleException(hr);
	}

	return pStorage;
}

/////////////////////////////////////////////////////////////
//����:ɾ��ָ�����˶���ԭ�����������
/////////////////////////////////////////////////////////////
BOOL CBufferPool::DeleteActionData(CAction* pAction, int n)
{
	//������Ӧ����
	WCHAR strName[256];
	swprintf(strName, L"Action%dNo%d", pAction->m_dwActionNumber, n);

	HRESULT hr = m_pStorage->DestroyElement(strName);

#ifdef _DEBUG
	if (FAILED(hr))
		ASSERT(NULL);		//pActionδ�����
#endif

//	m_pStorage->Commit(STGC_DEFAULT);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//CAction
//////////////////////////////////////////////////////////////////////////////

CBufferPool CAction::m_bufPool;

IMPLEMENT_DYNAMIC(CAction, CObject)

CAction::CAction(CFormDoc* pDoc, LPCTSTR lpszActionName)
	: m_bSaved(FALSE), m_pDocument(pDoc), m_bUnit(FALSE)
{
	m_dwActionNumber = m_bufPool.GetNextActionNumber();
	m_strName = lpszActionName;
}

CAction::~CAction()
{
	if (m_bSaved)
		m_bufPool.DeleteActionData(this);
}

HRESULT CAction::CommitBufferPool()
{
	return m_bufPool.m_pStorage->Commit(STGC_DEFAULT);
}

///////////////////////////////////////////////////////////////
//����:ȡ���˶������ʾ��
///////////////////////////////////////////////////////////////
CString& CAction::GetActionName()
{
	return m_strName;
}

////////////////////////////////////////////////////////////////
//����:��ar�б��������ڻ��˻��ظ�������
//����: ar[in]		�����Ŀ�ĵ�
//	pStorage[in]	�����õĴ���
////////////////////////////////////////////////////////////////
BOOL CAction::OnSaveInfo(CArchive& ar, LPSTORAGE pStorage)
{
#ifdef _DEBUG
	ar << m_dwActionNumber;
#endif

	return TRUE;
}

///////////////////////////////////////////////////////////////
//����:��ar�ж������ڻ��˻��ظ�������
//����: ar[in]		�����Ŀ�ĵ�
//	pStorage[in]	�����õĴ���
///////////////////////////////////////////////////////////////
BOOL CAction::OnLoadInfo(CArchive& ar, LPSTORAGE pStorage)
{
#ifdef _DEBUG
	UINT id;
	ar >> id;
	if (id != m_dwActionNumber)	//����Ķ����Ǳ���Ķ���
		ASSERT(NULL);
#endif

	return TRUE;
}


BOOL CAction::SaveData()
{
	IStreamPtr pStream;
	IStoragePtr pStorage = m_bufPool.CreateActionStorage(this, &pStream);
	if (pStorage == NULL)
		return FALSE;

	m_bSaved = TRUE;

	COleStreamFile file(pStream);
	CArchive ar(&file, CArchive::store);

	if (!OnSaveInfo(ar, pStorage))	//��������ʧ��
		return FALSE;

//	pStream->Commit(STGC_DEFAULT);
//	pStorage->Commit(STGC_DEFAULT);
//	m_bufPool.m_pStorage->Commit(STGC_DEFAULT);

	pStorage->Release();

	return TRUE;
}

///////////////////////////////////////////////////////////////
//����:�ڶ�Ӧ�ĵ��в�����ΪstrObjName�Ķ���
///////////////////////////////////////////////////////////////
CDevObj* CAction::FindObject(CString& strObjName)
{
	CDevObj* pObj;

	if (m_pDocument->GetNamedObjectsMap()->Lookup(strObjName, pObj))
		return pObj;
	else //���󲻴���
	{	
		ASSERT(FALSE);
		return NULL;
	}
}

////////////////////////////////////////////////////////////////////
//����:װ��ԭ�����������
////////////////////////////////////////////////////////////////////
BOOL CAction::LoadData()
{
	IStreamPtr pStream;
	IStoragePtr pStorage = m_bufPool.OpenActionStorage(this, &pStream);
	if (pStorage == NULL)
		return FALSE;

	COleStreamFile file(pStream);
	CArchive ar(&file, CArchive::load);

	if (!OnLoadInfo(ar, pStorage))	//��������ʧ��
		return FALSE;

	pStorage->Release();
	return TRUE;
}


