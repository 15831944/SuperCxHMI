// ActionObj.cpp: implementation of the CActionObj class.
//����:�������ӻ�ɾ��������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DevInc.h"
#include "ActionObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//����:actionName[in]		���˲���������
//		pDoc[in]			������Ե��ĵ�
//		pObj[in]			���˲�����ԵĶ���
//		bAddOrDel[in]		�����������ӻ���ɾ��
//		bSave[in]			�Ƿ�������������
//////////////////////////////////////////////////////////////////////
CActionObjAdd::CActionObjAdd(LPCTSTR lpszActionName, CFormDoc* pDoc, CDevObj* pObj, BOOL bAdd, BOOL bChild) :
	CAction(pDoc, lpszActionName), 
	m_bAdd(bAdd)
{
	ASSERT(pDoc != NULL);
	ASSERT(pObj != NULL);

	m_pObject = pObj;
	m_strObjName = m_pObject->GetDisplayName();
	m_bChild = bChild;

	if (!bChild)
		SaveData();
}

CActionObjAdd::~CActionObjAdd()
{
}

////////////////////////////////////////////////////////////////
//����:��ar�б��������ڻ��˻��ظ�������
////////////////////////////////////////////////////////////////
BOOL CActionObjAdd::OnSaveInfo(CArchive& ar, LPSTORAGE pStorage)
{
	ASSERT(m_pObject != NULL);	//ȷ���������

	ar << int(m_pObject->GetObjectType());

	m_pObject->CopyToClipboard(ar, pStorage);

	return CAction::OnSaveInfo(ar, pStorage);
}

///////////////////////////////////////////////////////////////
//����:��ar�ж������ڻ��˻��ظ�������
//����:ar[in]		�������ݵ��ĵ�
//	pStorage[in]	�������ݵĴ���
//����:�ɹ�TRUE,ʧ��FALSE
///////////////////////////////////////////////////////////////
BOOL CActionObjAdd::OnLoadInfo(CArchive& ar, LPSTORAGE pStorage)
{
	int nObjType;
	ar >> nObjType;

	switch (nObjType)
	{
	case CDevObj::drawobj:
		m_pObject = new CDrawObj(m_pDocument);
		break;
	case CDevObj::groupobj:
		m_pObject = new CGroupObj(m_pDocument);
		break;
	case CDevObj::symbolobj:
		m_pObject = new CSymbolObj(m_pDocument);
		break;
	case CDevObj::ctrlobj:
		m_pObject = new CCtrlObj(m_pDocument);
		break;
	case CDevObj::formobj:
		m_pObject = new CFormObj(m_pDocument);
		break;
	case CDevObj::reportobj:
		m_pObject = new CReportObj(m_pDocument);
		break;
	default:
		ASSERT(FALSE);
	}

	m_pObject->CreateFromClipboard(ar, pStorage);
	VERIFY(m_pObject->PutDisplayName(m_strObjName));
	
	return CAction::OnLoadInfo(ar, pStorage);
}

////////////////////////////////////////////////////////////
//����:����
////////////////////////////////////////////////////////////
void CActionObjAdd::Undo()
{
	ASSERT(m_pDocument != NULL);
	
	BOOL bFormObjChanged = FALSE;
	
	UndoEx(bFormObjChanged);
	
	if (!m_bChild && bFormObjChanged)
		m_pDocument->OnFormObjChange();
}

///////////////////////////////////////////////////////////
//����:����
///////////////////////////////////////////////////////////
void CActionObjAdd::Redo()
{
	ASSERT(m_pDocument != NULL);

	BOOL bFormObjChanged = FALSE;
	
	RedoEx(bFormObjChanged);

	if (!m_bChild && bFormObjChanged)
		m_pDocument->OnFormObjChange();
}

void CActionObjAdd::UndoEx(BOOL& bFormObjChanged)
{
	ASSERT(m_pDocument != NULL);
	CRect rect;
	
	CLayoutView* pView = m_pDocument->GetLayoutView();
	
	if (m_bAdd)
	{
		if (!m_bChild)
			pView->Select(NULL);
		
		m_pObject = FindObject(m_strObjName);
		
		rect = m_pObject->GetBoundingRect();
		pView->DocToClient(rect);
		pView->InvalidateRect(&rect);
		
		if (m_pObject->IsKindOf(RUNTIME_CLASS(CFormObj)))
			bFormObjChanged = TRUE;
		
		m_pObject->Delete();
		m_pObject = NULL;
	}
	else
	{
		LoadData();
		ASSERT(m_pObject != NULL);
		
		pView->Select(m_pObject, m_bChild);
		
		if (m_pObject->IsKindOf(RUNTIME_CLASS(CFormObj)))
			bFormObjChanged = TRUE;
	}
}

void CActionObjAdd::RedoEx(BOOL& bFormObjChanged)
{
	ASSERT(m_pDocument != NULL);
	CRect rect;
	
	CLayoutView* pView = m_pDocument->GetLayoutView();
	
	if (m_bAdd)
	{
		LoadData();
		ASSERT(m_pObject != NULL);
		
		pView->Select(m_pObject, m_bChild);
		
		if (m_pObject->IsKindOf(RUNTIME_CLASS(CFormObj)))
			bFormObjChanged = TRUE;
	}
	else
	{
		if (!m_bChild)
			pView->Select(NULL);
		
		m_pObject = FindObject(m_strObjName);
		
		rect = m_pObject->GetBoundingRect();
		pView->DocToClient(rect);
		pView->InvalidateRect(&rect);
		
		if (m_pObject->IsKindOf(RUNTIME_CLASS(CFormObj)))
			bFormObjChanged = TRUE;
		
		m_pObject->Delete();
		m_pObject = NULL;
	}
}

///////////////////////////////////////////////////////////
//����:�ж��Ƿ��������
///////////////////////////////////////////////////////////
BOOL CActionObjAdd::CanRedo()
{
	ASSERT(m_pDocument != NULL);
	return TRUE;
}

////////////////////////////////////////////////////////////
//����:���캯��
//����:actionName[in]		���˵�����
//		pDoc[in]			���˶��������ĵ�����
//		bAddOrDel[in]		��ɾ�����˻������ӻ���
////////////////////////////////////////////////////////////
CActionObjAddSet::CActionObjAddSet(LPCTSTR lpszActionName, CFormDoc* pDoc, BOOL bAdd)
	: CAction(pDoc, lpszActionName)
{
	ASSERT(pDoc != NULL);

	m_bAdd = bAdd;
}

CActionObjAddSet::~CActionObjAddSet()
{
	POSITION pos = m_listActions.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listActions.GetNext(pos);
		delete pAct;
	}
}

/////////////////////////////////////////////////////////
//����:�ڼ����м���ָ���Ķ������Ϣ
/////////////////////////////////////////////////////////
void CActionObjAddSet::AddObj(CDevObj* pObj)
{
	ASSERT(pObj != NULL);
	
	CActionObjAdd* pAct = new CActionObjAdd("SubAction", m_pDocument, pObj, m_bAdd, TRUE);
	if (pAct == NULL)
		AfxThrowMemoryException();
	m_listActions.AddTail(pAct);
}

///////////////////////////////////////////////////////////////////
//����: ������ӹ��̽���
///////////////////////////////////////////////////////////////////
void CActionObjAddSet::ObjAddComplete()
{
	POSITION pos = m_listActions.GetHeadPosition();
	CActionObjAdd* pAct;
	while (pos != NULL)
	{
		pAct = (CActionObjAdd *)m_listActions.GetNext(pos);
		pAct->SaveData();
	}
}

/////////////////////////////////////////////////////////
//����:����
/////////////////////////////////////////////////////////
void CActionObjAddSet::Redo()
{
	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Select(NULL);

	BOOL bFormObjChanged = FALSE;

	POSITION pos = m_listActions.GetHeadPosition();
	CActionObjAdd* pAct;
	while (pos != NULL)
	{
		pAct = (CActionObjAdd *)m_listActions.GetNext(pos);
		pAct->RedoEx(bFormObjChanged);
	}

	if (bFormObjChanged)
		m_pDocument->OnFormObjChange();

	theApp.NotifySelectChanged();
}

/////////////////////////////////////////////////////////////////
//����:����
/////////////////////////////////////////////////////////////////
void CActionObjAddSet::Undo()
{
	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Select(NULL);

	BOOL bFormObjChanged = FALSE;

	POSITION pos = m_listActions.GetHeadPosition();
	CActionObjAdd* pAct;
	while (pos != NULL)
	{
		pAct = (CActionObjAdd *)m_listActions.GetNext(pos);
		pAct->UndoEx(bFormObjChanged);
	}

	if (bFormObjChanged)
		m_pDocument->OnFormObjChange();

	theApp.NotifySelectChanged();
}

/////////////////////////////////////////////////////////
//����:���캯��
//����:pDoc[in]		������Ե��ĵ�
//	bGroupOrUnGroup[in]		��������ϲ������Ƿֽ����
/////////////////////////////////////////////////////////
CActionObjGroup::CActionObjGroup(LPCTSTR lpszActionName, CFormDoc* pDoc, BOOL bGroup) : 
	CAction(pDoc, lpszActionName), 
	m_setSubObj("", pDoc, !bGroup)
{
	m_bGroup = bGroup;
	m_pGroupObj = NULL;
}

CActionObjGroup::~CActionObjGroup()
{
	if (m_pGroupObj != NULL)
		delete m_pGroupObj;
}

////////////////////////////////////////////////////////////
//����: ����
////////////////////////////////////////////////////////////
void CActionObjGroup::Redo()
{
	if (m_bGroup)
	{
		m_setSubObj.Redo();
		m_pGroupObj->Redo();
	}
	else
	{
		m_pGroupObj->Redo();
		m_setSubObj.Redo();
	}
}

////////////////////////////////////////////////////////////
//����: ����
////////////////////////////////////////////////////////////
void CActionObjGroup::Undo()
{
	if (m_bGroup)
	{
		m_pGroupObj->Undo();
		m_setSubObj.Undo();
	}
	else
	{
		m_setSubObj.Undo();
		m_pGroupObj->Undo();
	}
}

////////////////////////////////////////////////////////////
//����: ������϶�����Ϣ
////////////////////////////////////////////////////////////
void CActionObjGroup::SetGroupObj(CDevObj* pObj)
{
	ASSERT(m_pGroupObj == NULL);
	m_pGroupObj = new CActionObjAdd("", m_pDocument, pObj, m_bGroup);
}

///////////////////////////////////////////////////////////
//����: �������ǰ, ��ֽ����Ӷ���
///////////////////////////////////////////////////////////
void CActionObjGroup::AddSubObj(CDevObj* pObj)
{
	m_setSubObj.AddObj(pObj);
}

///////////////////////////////////////////////////////////////////
//����: �Ӷ���������, ��������
///////////////////////////////////////////////////////////////////
void CActionObjGroup::ObjAddComplete()
{
	m_setSubObj.ObjAddComplete();
}

//////////////////////////////////////////////////////////////////////////////
//CActionObjMove
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CActionObjMove, CAction)

////////////////////////////////////////////////////////////////////
//����:�ƶ���ԭ�ƶ�λ��
////////////////////////////////////////////////////////////////////
void CActionObjMove::Redo()
{
	Undo();
}

////////////////////////////////////////////////////////////////////
//����:�ص�ԭλ��
////////////////////////////////////////////////////////////////////
void CActionObjMove::Undo()
{
	CDevObj* pObj = FindObject(m_strObjName);
	CRectF rcPosition = pObj->GetPositionRect();
	pObj->MoveTo(m_rcPosition, FALSE);
	m_rcPosition = rcPosition;

	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Select(pObj, TRUE);
}

BOOL CActionObjMove::CanUnit(CAction* pAct)
{
	if (!m_bUnit)
		return FALSE;

	if (pAct->IsKindOf(RUNTIME_CLASS(CActionObjMove)))
		if (pAct->GetActionName() == m_strName)
			return TRUE;

	return FALSE;
}

void CActionObjMove::Unit(CAction* pAct)
{
	ASSERT(pAct->IsKindOf(RUNTIME_CLASS(CActionObjMove)));

//	m_rcPosition = ((CActionObjMove *)pAct)->m_rcPosition;
}

//////////////////////////////////////////////////////////////////
//CActionObjMoveSet
//////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CActionObjMoveSet, CAction)

CActionObjMoveSet::CActionObjMoveSet(LPCTSTR lpszActionName, CFormDoc* pDoc, BOOL bChild)
	: CAction(pDoc, lpszActionName)
{
	m_bChild = bChild;
}

CActionObjMoveSet::~CActionObjMoveSet()
{
	POSITION pos = m_listActions.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listActions.GetNext(pos);
		delete pAct;
	}
	m_listActions.RemoveAll();
}

///////////////////////////////////////////////////////////
//����:�ڼ����м���һ���µĶ���
//����:pDoc[in]		�������ڵ��ĵ�
//		pObj[in]	Ҫ����Ķ���
//		rcOld[in]	ԭ���������ڵ�λ��
////////////////////////////////////////////////////////////
void CActionObjMoveSet::AddObj(CDevObj* pObj)
{
	ASSERT(pObj!= NULL);
	CActionObjMove* pAct = new CActionObjMove(m_pDocument, pObj);
	m_listActions.AddTail(pAct);
}

/////////////////////////////////////////////////////////////
//����:����
/////////////////////////////////////////////////////////////
void CActionObjMoveSet::Redo()
{
	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Select(NULL);

	POSITION pos = m_listActions.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listActions.GetNext(pos);
		pAct->Redo();
	}

	if (!m_bChild)
		theApp.NotifySelectChanged();
}

////////////////////////////////////////////////////////////
//����:����
////////////////////////////////////////////////////////////
void CActionObjMoveSet::Undo()
{
	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Select(NULL);
	
	POSITION pos = m_listActions.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listActions.GetNext(pos);
		pAct->Undo();
	}

	if (!m_bChild)
		theApp.NotifySelectChanged();
}

void CActionObjMoveSet::SetUnitState(BOOL b)
{
	CAction::SetUnitState(b);

	POSITION pos = m_listActions.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listActions.GetNext(pos);
		pAct->SetUnitState(b);
	}
}

BOOL CActionObjMoveSet::CanUnit(CAction* pAct)
{
	if (!m_bUnit)
		return FALSE;

	if (pAct->IsKindOf(RUNTIME_CLASS(CActionObjMoveSet)))
		if (pAct->GetActionName() == m_strName)
		{
			CActionObjMoveSet* pActSet = (CActionObjMoveSet *)pAct;
			if (m_listActions.GetCount() != pActSet->m_listActions.GetCount())
				return FALSE;

			POSITION pos = m_listActions.GetHeadPosition();
			POSITION posUnit = pActSet->m_listActions.GetHeadPosition();
			while (pos != NULL)
			{
				CAction* pActUint = pActSet->m_listActions.GetNext(posUnit);
				if (!m_listActions.GetNext(pos)->CanUnit(pActUint))
					return FALSE;
			}

			return TRUE;
		}

	return FALSE;
}

void CActionObjMoveSet::Unit(CAction* pAct)
{
	ASSERT(pAct->IsKindOf(RUNTIME_CLASS(CActionObjMoveSet)));

	if (pAct->IsKindOf(RUNTIME_CLASS(CActionObjMoveSet)))
		if (pAct->GetActionName() == m_strName)
		{
			CActionObjMoveSet* pActSet = (CActionObjMoveSet *)pAct;

			POSITION pos = m_listActions.GetHeadPosition();
			POSITION posUnit = pActSet->m_listActions.GetHeadPosition();
			while (pos != NULL)
			{
				CAction* pActUint = pActSet->m_listActions.GetNext(posUnit);
				m_listActions.GetNext(pos)->Unit(pActUint);
			}

			return;
		}

	ASSERT(FALSE);
}

//////////////////////////////////////////////////////////////////////////////
//CActionObj
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//����:��ar�б��������ڻ��˻��ظ�������
////////////////////////////////////////////////////////////////
BOOL CActionObj::OnSaveInfo(CArchive& ar, LPSTORAGE pStorage)
{
	ASSERT(m_pObject != NULL);	//ȷ���������

	ar << int(m_pObject->GetObjectType());

	m_pObject->CopyToClipboard(ar, pStorage);
	
	return CAction::OnSaveInfo(ar, pStorage);
}

void CActionObj::SetNewName(LPCTSTR lpszName)
{
	m_strNewName = lpszName;
}

///////////////////////////////////////////////////////////////
//����:��ar�ж������ڻ��˻��ظ�������
//����:ar[in]		�������ݵ��ĵ�
//	pStorage[in]	�������ݵĴ���
//����:�ɹ�TRUE,ʧ��FALSE
///////////////////////////////////////////////////////////////
BOOL CActionObj::OnLoadInfo(CArchive& ar, LPSTORAGE pStorage)
{
	int nObjType;
	ar >> nObjType;

	switch (nObjType)
	{
	case CDevObj::drawobj:
		m_pObject = new CDrawObj(m_pDocument);
		break;
	case CDevObj::groupobj:
		m_pObject = new CGroupObj(m_pDocument);
		break;
	case CDevObj::symbolobj:
		m_pObject = new CSymbolObj(m_pDocument);
		break;
	case CDevObj::ctrlobj:
		m_pObject = new CCtrlObj(m_pDocument);
		break;
	case CDevObj::formobj:
		m_pObject = new CFormObj(m_pDocument);
		break;
	case CDevObj::reportobj:
		m_pObject = new CReportObj(m_pDocument);
		break;
	default:
		ASSERT(FALSE);
	}

	m_pObject->CreateFromClipboard(ar, pStorage);
//	VERIFY(m_pObject->PutDisplayName(m_strNewName));
	
	return CAction::OnLoadInfo(ar, pStorage);
}

////////////////////////////////////////////////////////////////////
//����:d
////////////////////////////////////////////////////////////////////
void CActionObj::Redo()
{
	CLayoutView* pView = m_pDocument->GetLayoutView();

	if (!m_bChild)
		pView->Select(NULL);
	
	m_pObject = FindObject(m_strObjName);
	CRect rect = m_pObject->GetBoundingRect();

	m_pObject->Delete();
	m_pObject = NULL;

	IStreamPtr pStream;
	IStoragePtr pStorage = m_bufPool.OpenActionStorage(this, &pStream, 1);
	ASSERT(pStorage != NULL);

	COleStreamFile file(pStream);
	CArchive ar(&file, CArchive::load);

	VERIFY(OnLoadInfo(ar, pStorage) == TRUE);	//��������ʧ��
	pStorage->Release();

	VERIFY(m_pObject->PutDisplayName(m_strNewName));

	rect.UnionRect(&rect, m_pObject->GetBoundingRect());
	pView->DocToClient(rect);
	pView->InvalidateRect(&rect);

	pView->Select(m_pObject, m_bChild);
}

////////////////////////////////////////////////////////////////////
//����:�ص�ԭλ��
////////////////////////////////////////////////////////////////////
void CActionObj::Undo()
{
	CLayoutView* pView = m_pDocument->GetLayoutView();

	if (!m_bChild)
		pView->Select(NULL);

	m_pObject = FindObject(m_strNewName);
	CRect rect = m_pObject->GetBoundingRect();

	if (!m_bSaveRedoInfo)	
	{
		IStreamPtr pStream;
		IStoragePtr pStorage = m_bufPool.CreateActionStorage(this, &pStream, 1);
		ASSERT(pStorage != NULL);

		COleStreamFile file(pStream);
		CArchive ar(&file, CArchive::store);

		VERIFY(OnSaveInfo(ar, pStorage) == TRUE);	//��������ʧ��
		m_bSaveRedoInfo = TRUE;

	//	pStream->Commit(STGC_DEFAULT);
	//	pStorage->Commit(STGC_DEFAULT);

		pStorage->Release();
	}

	m_pObject->Delete();
	m_pObject = NULL;

	LoadData();

	VERIFY(m_pObject->PutDisplayName(m_strObjName));

	rect.UnionRect(&rect, m_pObject->GetBoundingRect());
	pView->DocToClient(rect);
	pView->InvalidateRect(&rect);

	pView->Select(m_pObject, m_bChild);
}

//////////////////////////////////////////////////////////////////
//CActionObjMoveSet
//////////////////////////////////////////////////////////////////

CActionObjSet::CActionObjSet(LPCTSTR lpszActionName, CFormDoc* pDoc) :
	CAction(pDoc, lpszActionName)
{
}

CActionObjSet::~CActionObjSet()
{
	POSITION pos = m_listActions.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listActions.GetNext(pos);
		delete pAct;
	}
	m_listActions.RemoveAll();
}

///////////////////////////////////////////////////////////
//����:�ڼ����м���һ���µĶ���
//����:pDoc[in]		�������ڵ��ĵ�
//		pObj[in]	Ҫ����Ķ���
//		rcOld[in]	ԭ���������ڵ�λ��
////////////////////////////////////////////////////////////
void CActionObjSet::AddObj(CDevObj* pObj)
{
	ASSERT(pObj!= NULL);
	CActionObj* pAct = new CActionObj("", m_pDocument, pObj, TRUE);
	m_listActions.AddTail(pAct);
}

///////////////////////////////////////////////////////////////////
//����: �Ӷ���������, ��������
///////////////////////////////////////////////////////////////////
void CActionObjSet::ObjAddComplete()
{
	POSITION pos = m_listActions.GetHeadPosition();
	CActionObjAdd* pAct;
	while (pos != NULL)
	{
		pAct = (CActionObjAdd *)m_listActions.GetNext(pos);
		pAct->SaveData();
	}
}

/////////////////////////////////////////////////////////////
//����:����
/////////////////////////////////////////////////////////////
void CActionObjSet::Redo()
{
	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Select(NULL);
	
	POSITION pos = m_listActions.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listActions.GetNext(pos);
		pAct->Redo();
	}

	theApp.NotifySelectChanged();
}

////////////////////////////////////////////////////////////
//����:����
////////////////////////////////////////////////////////////
void CActionObjSet::Undo()
{
	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Select(NULL);

	POSITION pos = m_listActions.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listActions.GetNext(pos);
		pAct->Undo();
	}

	theApp.NotifySelectChanged();
}

//////////////////////////////////////////////////////////////////
//CActionName
//////////////////////////////////////////////////////////////////

void CActionName::Redo()
{
	if (m_pSection != NULL)
	{
		m_pSection->SetName(m_strNewName);
	}
	else if (m_bObject)
	{
		CDevObj* pObj = FindObject(m_strOldName);
		pObj->PutDisplayName(m_strNewName);
	}
	else
	{
		m_pDocument->SetName(m_strNewName);
	}

	theApp.NotifySelectChanged();
}

void CActionName::Undo()
{
	if (m_pSection != NULL)
	{
		m_pSection->SetName(m_strOldName);
	}
	else if (m_bObject)
	{
		CDevObj* pObj = FindObject(m_strNewName);
		pObj->PutDisplayName(m_strOldName);
	}
	else
	{
		m_pDocument->SetName(m_strOldName);
	}

	theApp.NotifySelectChanged();
}

//////////////////////////////////////////////////////////////////////////////
//CActionDoc
//////////////////////////////////////////////////////////////////////////////

BOOL CActionDoc::OnSaveInfo(CArchive& ar, LPSTORAGE pStorage)
{
	m_pDocument->CopyToClipboard(ar, pStorage);
	
	return CAction::OnSaveInfo(ar, pStorage);
}

BOOL CActionDoc::OnLoadInfo(CArchive& ar, LPSTORAGE pStorage)
{
	m_pDocument->CreateFromClipboard(ar, pStorage);
	
	return CAction::OnLoadInfo(ar, pStorage);
}

void CActionDoc::Redo()
{
	IStreamPtr pStream;
	IStoragePtr pStorage = m_bufPool.OpenActionStorage(this, &pStream, 1);
	ASSERT(pStorage != NULL);
	
	COleStreamFile file(pStream);
	CArchive ar(&file, CArchive::load);
	
	VERIFY(OnLoadInfo(ar, pStorage) == TRUE);	//��������ʧ��
	pStorage->Release();
	
	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Invalidate();

	pView->Select(NULL);
	theApp.NotifySelectChanged(NULL);
}

void CActionDoc::Undo()
{
	if (!m_bSaveRedoInfo)	
	{
		IStreamPtr pStream;
		IStoragePtr pStorage = m_bufPool.CreateActionStorage(this, &pStream, 1);
		ASSERT(pStorage != NULL);
		
		COleStreamFile file(pStream);
		CArchive ar(&file, CArchive::store);
		
		VERIFY(OnSaveInfo(ar, pStorage) == TRUE);	//��������ʧ��
		m_bSaveRedoInfo = TRUE;
		
		pStorage->Release();
	}
	
	LoadData();
	
	CLayoutView* pView = m_pDocument->GetLayoutView();
	pView->Invalidate();

	pView->Select(NULL);
	theApp.NotifySelectChanged(NULL);
}

//////////////////////////////////////////////////////////////////////////////
//CActionDocSize
//////////////////////////////////////////////////////////////////////////////

void CActionDocSize::Redo()
{
	m_pDocument->SetDocSize(m_szNew.cx, m_szNew.cy);
	theApp.NotifySelectChanged(NULL);
}

void CActionDocSize::Undo()
{
	m_pDocument->SetDocSize(m_szOld.cx, m_szOld.cy);
	theApp.NotifySelectChanged(NULL);
}

//////////////////////////////////////////////////////////////////////////////
//CActionSection
//////////////////////////////////////////////////////////////////////////////

BOOL CActionSection::OnSaveInfo(CArchive& ar, LPSTORAGE pStorage)
{
	ASSERT(m_pSection != NULL);	

	m_pSection->Serialize(ar);
	
	return CAction::OnSaveInfo(ar, pStorage);
}

BOOL CActionSection::OnLoadInfo(CArchive& ar, LPSTORAGE pStorage)
{
	m_pSection->Serialize(ar);
	
	return CAction::OnLoadInfo(ar, pStorage);
}

void CActionSection::Redo()
{
	IStreamPtr pStream;
	IStoragePtr pStorage = m_bufPool.OpenActionStorage(this, &pStream, 1);
	ASSERT(pStorage != NULL);

	COleStreamFile file(pStream);
	CArchive ar(&file, CArchive::load);

	VERIFY(OnLoadInfo(ar, pStorage) == TRUE);	//��������ʧ��
	pStorage->Release();

	CReportView* pView = (CReportView *)m_pDocument->GetLayoutView();
	pView->Invalidate();
	pView->SelectSection(m_pSection);

	theApp.NotifySelectChanged();
}

void CActionSection::Undo()
{
	if (!m_bSaveRedoInfo)	
	{
		IStreamPtr pStream;
		IStoragePtr pStorage = m_bufPool.CreateActionStorage(this, &pStream, 1);
		ASSERT(pStorage != NULL);

		COleStreamFile file(pStream);
		CArchive ar(&file, CArchive::store);

		VERIFY(OnSaveInfo(ar, pStorage) == TRUE);	//��������ʧ��
		m_bSaveRedoInfo = TRUE;

		pStorage->Release();
	}

	LoadData();

	CReportView* pView = (CReportView *)m_pDocument->GetLayoutView();
	pView->Invalidate();
	pView->SelectSection(m_pSection);

	theApp.NotifySelectChanged();
}

//////////////////////////////////////////////////////////////////////////////
// CActionSectionLineMove
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//����:�ڼ����м���ָ���Ķ������Ϣ
/////////////////////////////////////////////////////////
void CActionSectionLineMove::AddObj(CDevObj* pObj)
{
	ASSERT(pObj!= NULL);
	
	m_actionObjectsMoved.AddObj(pObj);
}

void CActionSectionLineMove::Redo()
{
	CReportView* pView = (CReportView *)m_pDocument->GetLayoutView();
	pView->MoveSectionLineTo(m_nSectionLine, m_ptNew);
	m_actionObjectsMoved.Redo();
	pView->Invalidate();

	pView->Select(NULL);
	pView->SelectSection(pView->HitTestSection(m_ptNew));

	theApp.NotifySelectChanged();
}

void CActionSectionLineMove::Undo()
{
	CReportView* pView = (CReportView *)m_pDocument->GetLayoutView();
	pView->MoveSectionLineTo(m_nSectionLine, m_ptOld);
	m_actionObjectsMoved.Undo();
	pView->Invalidate();

	pView->Select(NULL);
	pView->SelectSection(pView->HitTestSection(m_ptOld));
	
	theApp.NotifySelectChanged();
}