//////////////////////////////////////////////////////////////////////
// UndoStack.cpp: implementation of the CUndoStack class.

#include "stdafx.h"
#include "DevInc.h"
#include "UndoStack.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoStack::CUndoStack()
{

}

CUndoStack::~CUndoStack()
{
	CAction* pAct;
	while(!m_listUndo.IsEmpty())
	{
		pAct = m_listUndo.RemoveHead();
		delete pAct;
	}

	while(!m_listRedo.IsEmpty())
	{
		pAct = m_listRedo.RemoveHead();
		delete pAct;
	}
}

//////////////////////////////////////////////////////////////////////
//����:����
//����:��ˢ�µ��ĵ�����
//////////////////////////////////////////////////////////////////////
void CUndoStack::Undo()
{
	if (m_listUndo.IsEmpty())
		return;

	CAction* pAct = m_listUndo.RemoveTail();
	if(pAct->CanRedo())
	{
		m_listRedo.AddTail(pAct);
		pAct->Undo();
	}
	else
	{
		pAct->Undo();
		delete pAct;
	}
}

/////////////////////////////////////////////////////////////////////
//����:����
//����:��ˢ�µ��ĵ�����
/////////////////////////////////////////////////////////////////////
void CUndoStack::Redo()
{
	if(m_listRedo.IsEmpty())
		return;

	CAction* pAct = m_listRedo.RemoveTail();
	m_listUndo.AddTail(pAct);
	pAct->Redo();
}

/////////////////////////////////////////////////////////////////////
//����:�����µ�Action
/////////////////////////////////////////////////////////////////////
BOOL CUndoStack::Push(CAction* pAction)
{
	ASSERT(pAction != NULL);

	// ���Ķ���
	CAction* pLastAction = NULL;
	if (!m_listUndo.IsEmpty() && m_listRedo.IsEmpty())
		pLastAction = m_listUndo.GetTail();

	// �໥�ܺϲ�
	if (pLastAction && pLastAction->CanUnit(pAction) && pAction->CanUnit(pLastAction))
	{
		pLastAction->Unit(pAction);
		delete pAction;
		return TRUE;
	}
		
	m_listUndo.AddTail(pAction);

	//ֻ�������100�β���
	if (m_listUndo.GetCount() > 100)
	{
		CAction* pAct = m_listUndo.GetHead();
		delete pAct;
		m_listUndo.RemoveHead();
	}
	
	POSITION pos = m_listRedo.GetHeadPosition();
//	if (pos == NULL)
//		return TRUE;

	while (pos != NULL)
	{
		CAction* pAct = m_listRedo.GetNext(pos);
		delete pAct;
	}
	m_listRedo.RemoveAll();
//	CAction::CommitBufferPool();

	return TRUE;
}

////////////////////////////////////////////////////////////////////
//����:��λ
////////////////////////////////////////////////////////////////////
BOOL CUndoStack::Reset()
{
	POSITION pos = m_listRedo.GetHeadPosition();
	while (pos != NULL)
	{
		CAction* pAct = m_listRedo.GetNext(pos);
		delete pAct;
	}
	m_listRedo.RemoveAll();

	pos = m_listUndo.GetHeadPosition();
	while(pos != NULL)
	{
		CAction* pAct = m_listUndo.GetNext(pos);
		delete pAct;
	}
	m_listUndo.RemoveAll();

	return TRUE;
}

////////////////////////////////////////////////////////////////////
//����:ȡ��һ���ɻ��˵�������
////////////////////////////////////////////////////////////////////
CString& CUndoStack::GetNextUndoCmdStr()
{
	static CString strRet;
	if (m_listUndo.IsEmpty())
		return strRet;

	CAction* pAct = m_listUndo.GetTail();
	return pAct->GetActionName();
}

////////////////////////////////////////////////////////////////////
//����:ȡ��һ����������������
////////////////////////////////////////////////////////////////////
CString& CUndoStack::GetNextRedoCmdStr()
{
	static CString strRet;
	if(m_listRedo.IsEmpty())
		return strRet;

	CAction* pAct = m_listRedo.GetTail();
	return pAct->GetActionName();
}

////////////////////////////////////////////////////////////////////
//����:�ж��Ƿ���Ի���
////////////////////////////////////////////////////////////////////
BOOL CUndoStack::CanUndo()
{
	if (m_listUndo.IsEmpty())
		return FALSE;
	else
		return TRUE;
}

////////////////////////////////////////////////////////////////////
//����:�б��Ƿ��������
////////////////////////////////////////////////////////////////////
BOOL CUndoStack::CanRedo()
{
	if (m_listRedo.IsEmpty())
		return FALSE;
	else
		return TRUE;
}
