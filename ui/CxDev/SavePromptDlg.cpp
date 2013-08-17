// SavePromptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DevInc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg dialog


CSavePromptDlg::CSavePromptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSavePromptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSavePromptDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSavePromptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSavePromptDlg)
	DDX_Control(pDX, IDC_LIST, m_ctlListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSavePromptDlg, CDialog)
	//{{AFX_MSG_MAP(CSavePromptDlg)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg message handlers

BOOL CSavePromptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	POSITION posDecDoc = CDevDoc::c_listDevDoc.GetHeadPosition();
	while (posDecDoc != NULL)
	{
		//���빤���ĵ���
		CDevDoc* pDoc = (CDevDoc*)CDevDoc::c_listDevDoc.GetNext(posDecDoc);
		CDevDoc::CSubDoc* pSubDoc;
		int index;
		CItemInfo* pInfo;

		CPtrList& listSubDoc = pDoc->m_listSubDoc;
		pSubDoc = (CDevDoc::CSubDoc *)listSubDoc.GetHead();

		BOOL bHasAddDevDoc = FALSE;
		//��������ļ��������ĵ���Ŀ������1 ���Ǿ��ĵ������ĵ��ı�
		if ((listSubDoc.GetCount() > 1 && pDoc->GetPathName().IsEmpty()) || 
			(pDoc->IsModified() && !pDoc->GetPathName().IsEmpty()))
		{
			bHasAddDevDoc = TRUE;
			pDoc->m_bNeedSave = TRUE;
			index = m_ctlListBox.AddString(pDoc->GetTitle());
			pInfo = new CItemInfo(enumSaveDevDoc, (DWORD)pDoc);	
			m_ctlListBox.SetItemData(index, (DWORD)pInfo);	//�Ƕ�Ӧ����
		}
			
		POSITION posSubDoc = listSubDoc.GetHeadPosition();
		while (posSubDoc != NULL)
		{
			pSubDoc = (CDevDoc::CSubDoc *)listSubDoc.GetNext(posSubDoc);
			pSubDoc->m_bNeedSave = TRUE;	//init
			if (pSubDoc->m_pChildDoc != NULL)
			{
				if (!(pSubDoc->m_pChildDoc->IsModified()))
					continue;
				if (!bHasAddDevDoc)
				{
					bHasAddDevDoc = TRUE;
					pDoc->m_bNeedSave = TRUE;
					index = m_ctlListBox.AddString(pDoc->GetTitle());
					pInfo = new CItemInfo(enumSaveDevDoc, (DWORD)pDoc);	
					m_ctlListBox.SetItemData(index, (DWORD)pInfo);	//�Ƕ�Ӧ����
				}

				CString strTmp = "   ";
				if (pSubDoc->m_strName == "")
					strTmp += "δ����";
				else
					strTmp += pSubDoc->m_strName; //m_pChildDoc->GetTitle();
				index = m_ctlListBox.AddString(strTmp);
				pInfo = new CItemInfo(enumSaveSubDoc, (DWORD)pSubDoc);	
				m_ctlListBox.SetItemData(index, (DWORD)pInfo);//��¼�����ĵ���ָ��
			}
		}
	}

	int count = m_ctlListBox.GetCount();
	if (count == 0)
	{
		EndDialog(IDNO);
		return FALSE;
	}
	for (int i = 0; i < count; i++)
	{
		m_ctlListBox.SetSel(i);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////////////////////////
//����:�����û���ѡ�����ö�Ӧ�����ĵ��������Ҫ�����־
////////////////////////////////////////////////////////////////////
void CSavePromptDlg::OnYes() 
{
	int count = m_ctlListBox.GetCount();
	for (int i = 0; i < count; i++)
	{
		int ifSel = m_ctlListBox.GetSel(i);
		CItemInfo* pInfo = (CItemInfo*)m_ctlListBox.GetItemData(i);
		
		if (ifSel == LB_ERR || ifSel <= 0 )
		{	//�û�δѡ��
			if (pInfo->m_type == enumSaveDevDoc)
				((CDevDoc *)pInfo->m_dwData)->m_bNeedSave = FALSE;
			else
				((CDevDoc::CSubDoc *)pInfo->m_dwData)->m_bNeedSave = FALSE;
		}
	}

	EndDialog(IDYES);	
}

////////////////////////////////////////////////////////////////////////
//����:�趨���õ����ĵ�Ϊ������
////////////////////////////////////////////////////////////////////////
void CSavePromptDlg::OnNo() 
{
	int count = m_ctlListBox.GetCount();
	for(int i = 0; i < count; i++)
	{
		CItemInfo* pInfo = (CItemInfo*)m_ctlListBox.GetItemData(i);
		if (pInfo->m_type == enumSaveDevDoc)
			((CDevDoc *)pInfo->m_dwData)->m_bNeedSave = FALSE;
		else
			((CDevDoc::CSubDoc *)pInfo->m_dwData)->m_bNeedSave = FALSE;
	}
	
	EndDialog(IDNO);			
}


void CSavePromptDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	int count = m_ctlListBox.GetCount();
	for(int i = 0; i < count; i++)
		delete (CItemInfo*)m_ctlListBox.GetItemData(i);

}

void CSavePromptDlg::OnSelchangeList() 
{
	int iDevDocItem;
	int count = m_ctlListBox.GetCount();
	for(int i = 0; i < count; i++)
	{
		CItemInfo* pInfo = (CItemInfo*)m_ctlListBox.GetItemData(i);
		if (pInfo->m_type == enumSaveDevDoc)
		{
			iDevDocItem = i;
		}
		else
		{
			int iSel = m_ctlListBox.GetSel(i);
			if (iSel > 0)
				m_ctlListBox.SetSel(iDevDocItem);
		}
	}
}
