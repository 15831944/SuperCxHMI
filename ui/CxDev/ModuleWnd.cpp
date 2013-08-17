// ModuleWnd.cpp : implementation file

#include "stdafx.h"
#include "DevInc.h"
#include "devobjdoc.h"
#include "BarListCtrl.h"
#include "ModuleWnd.h"

#include "DevDoc.h"
#include "ModuleDoc.h"
#include "FolderNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MLW_ICON_FOLDER	0
#define MLW_ICON_FORM	1
#define MLW_HEADERSIZE	20
#define MLW_TOOLWIDTH	16
#define MLW_TOOLSTEP	4

/////////////////////////////////////////////////////////////////////////////
// CModuleWnd

CModuleWnd::CModuleWnd()
{
	m_pCurrentDocument = NULL;
	m_isDragBeginer = FALSE;
}

CModuleWnd::~CModuleWnd()
{
}


BEGIN_MESSAGE_MAP(CModuleWnd, CBarListCtrl)
	//{{AFX_MSG_MAP(CModuleWnd)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_LIB_NEW, OnLibNew)
	ON_COMMAND(ID_LIB_RENAME, OnLibRename)
	ON_COMMAND(ID_LIB_DELETE, OnLibDelete)
	ON_COMMAND(ID_LIB_OPEN, OnLibOpen)
	ON_COMMAND(ID_EDIT_CUT, OnLibCut)
	ON_COMMAND(ID_EDIT_COPY, OnLibCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnLibPaste)
	ON_NOTIFY(LVN_KEYDOWN, CBARLISTCTRL_LISTID, OnListKeyDown)
	ON_NOTIFY(NM_DBLCLK, CBARLISTCTRL_LISTID, OnItemdblclick)
	ON_WM_DESTROY()
	ON_COMMAND(ID_ADD_FORM, OnAddForm)
	ON_COMMAND(ID_ADD_SUBMODULE, OnAddSubmodule)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_BEGINDRAG, CBARLISTCTRL_LISTID, OnBegindrag)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModuleWnd message handlers

//////////////////////////////////////////////////////////////
//����:ִ�а�ť����
//����:nButton[in]		�ڼ�����ť
//////////////////////////////////////////////////////////////
void CModuleWnd::DoCommand(int nButton)
{
	switch(nButton)
	{
	case 0:		//���ؿ�Ŀ¼
		OnBackToRoot();
		break;
	case 1:		//�½�ͼ��
		OnLibNew();
		break;
	case 2:		//��ͼ��
		GetListCtrl()->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_REPORT, LVS_ICON);
		break;
	case 3:		//Сͼ��
		GetListCtrl()->ModifyStyle(LVS_LIST | LVS_REPORT | LVS_ICON, LVS_SMALLICON);
		break;
	case 4:		//������ʽ
		GetListCtrl()->ModifyStyle(LVS_REPORT | LVS_ICON | LVS_SMALLICON, LVS_LIST);
		break;
	case 5:		//��ϸ����
		GetListCtrl()->ModifyStyle(LVS_ICON | LVS_SMALLICON, LVS_REPORT | LVS_LIST);
		break;
	default:	//δ֪
		break;
	}
}


/////////////////////////////////////////////////////////////////////////
//����:ִ�зŲ���
//����:pWnd[in]			��ǰ������ڴ���
//		pDataObject[in]	��ǰҪ׼�����µ�����
//		dropDefault[in]	��ǰ��Ĭ�϶���
//		dropList[in]	����ִ�еĶ����б�
//		point[in]		��ǰ�����Ļ�Ĺ��λ��
//����: �ɹ�TRUE,ʧ��FALSE
/////////////////////////////////////////////////////////////////////////
BOOL CModuleWnd::DoDrop(CWnd *pWnd, COleDataObject *pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
{
	BOOL bRet = FALSE;
	return bRet;
}

/////////////////////////////////////////////////////////////////////////
//����:���Ϸ��ڼ�ȷ��������״
//����:pWnd[in]			��ǰ������ڴ���
//		pDataObject[in]	��ǰҪ׼�����µ�����
//		dropDefault[in]	��ǰ��Ĭ�϶���
//		dropList[in]	����ִ�еĶ����б�
//		point[in]		��ǰ�����Ļ�Ĺ��λ��
//����:�������״��ֵ
/////////////////////////////////////////////////////////////////////////
DROPEFFECT CModuleWnd::FindDragEffect(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dfRet = DROPEFFECT_NONE;
	if (m_isDragBeginer)	//���Լ���ʼ�ϷŲ�����
		return dfRet;
	
	if (m_pCurrentDocument != NULL && pDataObject->IsDataAvailable(CDevDoc::m_cfDocData))
		dfRet = DROPEFFECT_COPY;

	return dfRet;
}

///////////////////////////////////////////////////////
//����:��ǰ��ʾ���ݼӵ���Ŀ¼
///////////////////////////////////////////////////////
void CModuleWnd::OnBackToRoot()
{
	if (m_pCurrentDocument != NULL)
	{
		if (!m_pCurrentDocument->CloseLib())
			AfxMessageBox(_T("�ر�ģ����ļ�����"));
		delete m_pCurrentDocument;
		m_pCurrentDocument = NULL;
	}

	//ת��ͼ��
	GetListCtrl()->SetImageList(&m_listImage, LVSIL_SMALL);
	GetListCtrl()->SetImageList(&m_listImageLarge, LVSIL_NORMAL);

	OnUpdate();
}
//////////////////////////////////////////////////////////////
//����:ͬ��ͼ������ʾ����
//////////////////////////////////////////////////////////////
void CModuleWnd::OnUpdate()
{
	GetListCtrl()->DeleteAllItems();
	CString strName;

	if (m_pCurrentDocument == NULL) //��ʾ��Ŀ¼
	{	
		strName = CModuleDoc::GetDefaultPath() + _T("\\*.");
		strName += CModuleDoc::m_szModuleLibFileExt;
		CFileFind finder;
		BOOL bOk = finder.FindFile(strName);

		while(bOk)
		{
			bOk = finder.FindNextFile();
			strName = finder.GetFileTitle();
			GetListCtrl()->InsertItem(1, strName, MLW_ICON_FOLDER);
		}
	}
	else //��ʾ������
	{
		int nCount = m_pCurrentDocument->GetModuleCount();
		for (int i = 0; i < nCount; i++)
		{
			CModuleDoc::CModule* pModule = m_pCurrentDocument->GetModule(i);
			GetListCtrl()->InsertItem(i, pModule->m_strName, i);
		}
	}
}

//////////////////////////////////////////////////////////
//����:����һ����ͼ��
//////////////////////////////////////////////////////////
void CModuleWnd::OnLibNew()
{
	if (m_pCurrentDocument != NULL)	//�Ѿ���ģ����˳�
		return;

	CFolderNameDlg dlg;
	dlg.m_strTitle = _T("����ģ�������");

	while(dlg.DoModal() == IDOK)
	{
		LVFINDINFO info;
		info.flags = LVFI_STRING;
		info.psz = dlg.m_strFolderName.LockBuffer();

		if (GetListCtrl()->FindItem(&info) != -1)	//��ͬ���ļ�
		{
			dlg.m_strFolderName.UnlockBuffer();
			continue;
		}
		dlg.m_strFolderName.UnlockBuffer();

		m_pCurrentDocument = CModuleDoc::NewLib(dlg.m_strFolderName);
		break;
	}

	if (m_pCurrentDocument != NULL) 
	{
		m_pCurrentDocument->SaveLib();
		m_pCurrentDocument->CloseLib();
	}

	OnBackToRoot();
}

void CModuleWnd::OnLibOpen() 
{
	ASSERT(m_pCurrentDocument == NULL);	//��ǰӦ���ڿ�Ŀ¼��ʾ״̬��
	AfxGetApp()->DoWaitCursor(1);

	//�򿪿��ĵ�
	POSITION pos = GetListCtrl()->GetFirstSelectedItemPosition();
	int index = GetListCtrl()->GetNextSelectedItem(pos);
	if (index < 0)
		return;
	TCHAR buffer[40];
	GetListCtrl()->GetItemText(index, 0, buffer, 40);
	CString strName = buffer;

	CModuleDoc* pDoc = new CModuleDoc();
	if (pDoc == NULL)
		AfxThrowMemoryException();
	pDoc->OpenLib(strName);
	
	//������ͼ���С
//	GetListCtrl()->SetImageList(&m_ImageContentSmall, LVSIL_SMALL);
//	GetListCtrl()->SetImageList(&m_ImageContentLarge, LVSIL_NORMAL);

	//������ʾ
	m_pCurrentDocument = pDoc;

	//�ı�ͼ��
	GetListCtrl()->SetImageList(&m_pCurrentDocument->m_listImage, LVSIL_SMALL);
	GetListCtrl()->SetImageList(&m_pCurrentDocument->m_listImageLarge, LVSIL_NORMAL);

	OnUpdate();
	DoCommand(-1);

	AfxGetApp()->DoWaitCursor(-1);

}

void CModuleWnd::OnLibRename()
{
	ASSERT(GetListCtrl()->GetSelectedCount() == 1);

	POSITION pos = GetListCtrl()->GetFirstSelectedItemPosition();
	int nItem = GetListCtrl()->GetNextSelectedItem(pos);
	
	TCHAR szBuf[32];
	GetListCtrl()->GetItemText(nItem, 0, szBuf, 32);
	CString strName = szBuf;

	CFolderNameDlg dlg;
	if (m_pCurrentDocument == NULL)
		dlg.m_strTitle = _T("����ģ�������");
	else
		dlg.m_strTitle = _T("����ģ������");

	dlg.m_strFolderName = GetListCtrl()->GetItemText(nItem, 0);

	while (dlg.DoModal() == IDOK)
	{
		LVFINDINFO info;
		info.flags = LVFI_STRING;
		info.psz = dlg.m_strFolderName.LockBuffer();

		int nFindItem = GetListCtrl()->FindItem(&info);
		if (nFindItem != -1 && nFindItem != nItem)	//��ͬ���ļ�
		{
			dlg.m_strFolderName.UnlockBuffer();
			continue;
		}
		dlg.m_strFolderName.UnlockBuffer();
		if (m_pCurrentDocument == NULL)
		{
			CModuleDoc::RenameLib(strName, dlg.m_strFolderName);
			OnUpdate();
		}
		else
		{
			m_pCurrentDocument->SetModuleName(strName, dlg.m_strFolderName);
			GetListCtrl()->SetItemText(nItem, 0, dlg.m_strFolderName);
		}

		break;
	}
}

void CModuleWnd::LibDelete(BOOL bPrompt)
{
	POSITION pos = GetListCtrl()->GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	
	int nItem;
	
	if (m_pCurrentDocument == NULL)
	{	
		if (bPrompt && MessageBox(_T("ȷʵҪɾ��ѡ�е�ģ�����"), _T("ȷ��ģ���ɾ��"), MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;

		//ɾ����
		TCHAR buffer[MAX_PATH];
		CString strName;
		while (pos)
		{
			nItem = GetListCtrl()->GetNextSelectedItem(pos);
			GetListCtrl()->GetItemText(nItem, 0, buffer, MAX_PATH);
			strName = buffer;
			CModuleDoc::DeleteLib(strName);
		}
		OnUpdate();
	}
	else
	{	
		if (bPrompt && MessageBox(_T("ȷʵҪɾ��ѡ�е�ģ����"), _T("ȷ��ģ��ɾ��"), MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;

		//ɾ����
		TCHAR buffer[MAX_PATH];
		CString strName;
		while (pos)
		{
			nItem = GetListCtrl()->GetNextSelectedItem(pos);
			GetListCtrl()->GetItemText(nItem, 0, buffer, MAX_PATH);
			strName = buffer;
			m_pCurrentDocument->DeleteModule(strName);
		}

		m_pCurrentDocument->SaveLib();

		OnUpdate();
	}
}

void CModuleWnd::OnLibDelete()
{
	LibDelete();
}

void CModuleWnd::OnListKeyDown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
	if (pLVKeyDow->wVKey == VK_DELETE)
		OnLibDelete();

	pResult = 0;
}

int CModuleWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBarListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	OnUpdate();

	return 0;
}

void CModuleWnd::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	menu.CreatePopupMenu();

	int nSelectCount = GetListCtrl()->GetSelectedCount();

	//���ſ��ĵ�û�д�
	if (m_pCurrentDocument == NULL)
	{
		menu.AppendMenu(MF_STRING, ID_LIB_NEW, _T("�½�(&N)"));
		if (nSelectCount == 0)
		{
		}
		else if (nSelectCount == 1)
		{
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_LIB_OPEN, _T("��(&O)"));
			menu.AppendMenu(MF_STRING, ID_LIB_DELETE, _T("ɾ��(&D)"));
			menu.AppendMenu(MF_STRING, ID_LIB_RENAME, _T("������(&R)"));
		}
		else
		{
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_LIB_DELETE, _T("ɾ��(&D)"));
		}
	}
	else
	{
		if (nSelectCount > 0)
		{
			menu.AppendMenu(MF_STRING, ID_EDIT_CUT, _T("����(&T)"));
			menu.AppendMenu(MF_STRING, ID_EDIT_COPY, _T("����(&C)"));
		}
		if (IsClipboardFormatAvailable(CDevDoc::m_cfDocData))
			menu.AppendMenu(MF_STRING, ID_EDIT_PASTE, _T("ճ��(&P)"));
		if (nSelectCount > 0)
			menu.AppendMenu(MF_STRING, ID_LIB_DELETE, _T("ɾ��(&D)"));
		
		if (nSelectCount == 1)
		{
			menu.AppendMenu(MF_SEPARATOR);

			menu.AppendMenu(MF_STRING, ID_FILE_NEW, _T("�½�����(&N)"));
			CMDIFrameWnd* pParentFrame = (CMDIFrameWnd*)AfxGetMainWnd();
			CMDIChildWnd* pMDIActive = pParentFrame->MDIGetActive();
			if (pMDIActive != NULL)
			{
				menu.AppendMenu(MF_STRING, ID_ADD_FORM, _T("�����Ӵ���(&F)"));
				CView* pView = pMDIActive->GetActiveView();
				if (pView->IsKindOf(RUNTIME_CLASS(CLayoutView)))
					menu.AppendMenu(MF_STRING, ID_ADD_SUBMODULE, _T("����ģ��(&M)"));
			}

			menu.AppendMenu(MF_SEPARATOR);

			menu.AppendMenu(MF_STRING, ID_LIB_RENAME, _T("������(&R)"));
		}

	}

	menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CModuleWnd::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW * phdn = (NMLISTVIEW *) pNMHDR;
	
	if (m_pCurrentDocument != NULL)
	{
		COleDataSource* pData = PrepareDataSource();
		if (pData != NULL)
		{
			m_isDragBeginer = TRUE;
			DROPEFFECT dropEffect = pData->DoDragDrop(DROPEFFECT_COPY);
			delete pData;
			m_isDragBeginer = FALSE;
		}
	}
	
	*pResult = 0;
}

void CModuleWnd::GetDragInfoData(LPSTGMEDIUM lpStgMedium)
{
	ASSERT_VALID(this);
	ASSERT(AfxIsValidAddress(lpStgMedium, sizeof(STGMEDIUM)));
	
	int nItem;
	CModuleDoc::CModule* pModule;
	CRect rcBound;
	rcBound.SetRectEmpty();
	
	CClientDC dc(this);
	dc.BeginPath();
	
	POSITION pos = GetListCtrl()->GetFirstSelectedItemPosition();
	while (pos)
	{
		nItem = GetListCtrl()->GetNextSelectedItem(pos);
		pModule = m_pCurrentDocument->GetModule(nItem);

		CRect rcObj = pModule->m_rcPosition.GetRECT();
		rcObj.NormalizeRect();
		rcBound.UnionRect(rcBound, rcObj);

		dc.MoveTo(rcObj.left, rcObj.top);
		dc.LineTo(rcObj.right, rcObj.top);
		dc.LineTo(rcObj.right, rcObj.bottom);
		dc.LineTo(rcObj.left, rcObj.bottom);
		dc.LineTo(rcObj.left, rcObj.top);
	}
	
	dc.EndPath();
	
	
	CPoint ptStart = rcBound.CenterPoint();
	theApp.AlignToGrid(ptStart);
	
	LPSTREAM lpStream;
	if (::CreateStreamOnHGlobal(NULL, TRUE, &lpStream) != S_OK)
		AfxThrowMemoryException();
	
	ASSERT(lpStream != NULL);
	
	lpStream->Write(&rcBound, sizeof(CRect), NULL);
	lpStream->Write(&ptStart, sizeof(CPoint), NULL);
	
	int iNumPoints = dc.GetPath(NULL, NULL, 0);
	
	CPoint* pPoints = new CPoint[iNumPoints];
	if (pPoints == NULL)
		AfxThrowMemoryException();
	BYTE* pTypes = new BYTE[iNumPoints];
	if (pTypes == NULL)
		AfxThrowMemoryException();
	
	iNumPoints = dc.GetPath(pPoints, pTypes, iNumPoints);
	
	lpStream->Write(&iNumPoints, sizeof(int), NULL);
	lpStream->Write(pPoints, sizeof(CPoint) * iNumPoints, NULL);
	lpStream->Write(pTypes, sizeof(BYTE) * iNumPoints, NULL);
	
	if (pPoints != NULL)
		delete pPoints;
	if (pTypes != NULL)
		delete pTypes;
	
	// setup the STGMEDIUM
	lpStgMedium->tymed = TYMED_ISTREAM;
	lpStgMedium->pstm = lpStream;
	lpStgMedium->pUnkForRelease = NULL;
}

BOOL CModuleWnd::GetSelectModuleData(LPSTGMEDIUM lpStgMedium)
{
	TCHAR buffer[MAX_PATH];
	CString strName;
	POSITION pos = GetListCtrl()->GetFirstSelectedItemPosition();
	int nItem;
	
	CPtrArray arrSubDoc;
	while (pos)
	{
		nItem = GetListCtrl()->GetNextSelectedItem(pos);
		GetListCtrl()->GetItemText(nItem, 0, buffer, MAX_PATH);
		strName = buffer;
		CDevDoc::CSubDoc* pSubDoc = m_pCurrentDocument->GetSubDocument(strName);
		arrSubDoc.Add(pSubDoc);
	}

	CDevDoc::GetSubDocumentData((CDevDoc::CSubDoc **)arrSubDoc.GetData(), arrSubDoc.GetSize(), lpStgMedium);
		
	for (int i = 0; i < arrSubDoc.GetSize(); i++)
		delete (CDevDoc::CSubDoc *)arrSubDoc[i];
	arrSubDoc.RemoveAll();
	
	return TRUE;
}

COleDataSource* CModuleWnd::PrepareDataSource()
{
	ASSERT(m_pCurrentDocument != NULL);
	
	int nSelectedObjects = GetListCtrl()->GetSelectedCount();
	if (nSelectedObjects < 1)
		return NULL;
	
	STGMEDIUM stgMedium;
	COleDataSource* pDataSource;
	
	pDataSource = new COleDataSource;
	if(pDataSource == NULL)
	{
		AfxThrowMemoryException();
	}
	
	if (!GetSelectModuleData(&stgMedium))
	{
		delete pDataSource;
		return NULL;
	}
	
	pDataSource->CacheData(CDevDoc::m_cfDocData, &stgMedium);
	
	GetDragInfoData(&stgMedium);
	pDataSource->CacheData(CLayoutView::m_cfDragInfo, &stgMedium);
	
	return pDataSource;
}

void CModuleWnd::OnLibCopy()
{
	COleDataSource* pDataSource = PrepareDataSource();

	ASSERT_VALID(pDataSource);
	pDataSource->SetClipboard();
}

void CModuleWnd::OnLibCut()
{
	OnLibCopy();
	LibDelete(FALSE);
}

void CModuleWnd::OnLibPaste() 
{
	ASSERT(m_pCurrentDocument != NULL);

	if (m_pCurrentDocument->DoPaste(NULL))
	{
		GetListCtrl()->EnsureVisible(GetListCtrl()->GetItemCount() - 1, FALSE);
		OnUpdate();
	}
}

void CModuleWnd::OnDestroy() 
{
	CBarListCtrl::OnDestroy();

	if (m_pCurrentDocument != NULL)
	{
		if (!m_pCurrentDocument->CloseLib())
			AfxMessageBox(_T("�ر�ģ����ļ�����"));
		delete m_pCurrentDocument;
		m_pCurrentDocument = NULL;
	}
}

void CModuleWnd::OnItemdblclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMITEMACTIVATE *phdn = (NMITEMACTIVATE *) pNMHDR;

	*pResult = 0;
	if (phdn->iItem < 0)
		return;

	if (m_pCurrentDocument == NULL)
		OnLibOpen();
	else
	{
//		OnTobeMain();
	}
}

//////////////////////////////////////////////////////////////
//����:�����Ƿ�ǰ��ģ����
///////////////////////////////////////////////////////////////
BOOL CModuleWnd::CanAddModule()
{
	return (m_pCurrentDocument != NULL);
}

void CModuleWnd::AddFormModule(CFormDoc* pFormDoc)
{
	if (m_pCurrentDocument != NULL)
	{
		m_pCurrentDocument->AddFormModule(pFormDoc);
		OnUpdate();

		GetListCtrl()->EnsureVisible(GetListCtrl()->GetItemCount() - 1, FALSE);
	}
}

void CModuleWnd::OnAddForm() 
{
	CMDIFrameWnd* pParentFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	CMDIChildWnd* pMDIActive = pParentFrame->MDIGetActive();
	if (pMDIActive != NULL)	
	{
		CFormDoc* pActiveDoc = (CFormDoc *)pMDIActive->GetActiveDocument();
		OnLibCopy();
		pActiveDoc->m_pDocument->DoPasteSubDocument(NULL);
	}
}

void CModuleWnd::OnAddSubmodule()
{
	CMDIFrameWnd* pParentFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	CMDIChildWnd* pMDIActive = pParentFrame->MDIGetActive();
	if (pMDIActive != NULL)	
	{
		CFormDoc* pActiveDoc = (CFormDoc *)pMDIActive->GetActiveDocument();
		if (pActiveDoc == NULL)
			return;
		CLayoutView* pView = pActiveDoc->GetLayoutView();
		if (pView == NULL)
			return;
		OnLibCopy();
		pView->DoPasteSubmodule(NULL, NULL);
	}
}

void CModuleWnd::OnFileNew() 
{
	CDevDoc* pDoc = CDevDoc::CreateNewDocument(FALSE);

	OnLibCopy();
	pDoc->DoPasteSubDocument(NULL);
}
