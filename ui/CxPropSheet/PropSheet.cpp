// PropSheet.cpp : implementation file
// ������Ŀ: AutoCx��CxPropSheet����
// ��������: 2001.10.8
// �� �� �ˣ���Խ�� 
// ��������������ҳ������������,���ڴ����͹�������ҳ�� 
// �޸ļ�¼��
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CxPropSheet.h"
#include "PropSheet.h"

#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropSheet

IMPLEMENT_DYNAMIC(CPropSheet, CPropertySheet)

CPropSheet::CPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPropSheet::CPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_aryUnk = NULL;
	m_nObjCounts = 0;
	m_ClsIDs = NULL;
	m_iPageCnt = 0;
	m_psh.dwFlags |= PSH_HASHELP;
}

CPropSheet::~CPropSheet()
{
	for (UINT i = 0; i < m_iPageCnt; i++)
	{
		delete ((CPropPage *)m_pageArray[i]);			
	}

	for(i = 0; i < (UINT)m_nObjCounts; i++)
	{
		m_aryUnk[i]->Release();
	}
	delete [] m_aryUnk;

	if(m_ClsIDs != NULL)
		delete [] m_ClsIDs;
}


BEGIN_MESSAGE_MAP(CPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPropSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropSheet message handlers

////////////////////////////////////////////////////////////////
//����:�ڶ����м�������õ���OLE����ָ������
//����:count[in]	Ҫ���뵽���ڵ�����ҳ����ĸ���
//	    pArray[in]	��Ҫ��ʾ���ԵĶ�����IUnknowָ������
//����:�ɹ�TRUE,ʧ��FALSE
////////////////////////////////////////////////////////////////
BOOL CPropSheet::AddObjects(ULONG count, LPUNKNOWN *pArray)
{
	m_aryUnk = new LPUNKNOWN[count];
	if(m_aryUnk == NULL)
		return FALSE;

	for(ULONG i = 0; i < count; i++)
	{
		m_aryUnk[i] = pArray[i];
		m_aryUnk[i]->AddRef();
	}

	m_nObjCounts = (int)count;
	return TRUE;
}

//////////////////////////////////////////////////////////////////
//����:��������ҳ��CLSID����
//����:count[in]	 pArray�����еĳ�Ա����
//		pArray[in]	 Ҫ���������ҳ��CLSID����
//����:�ɹ�TRUE,ʧ��FALSE
//////////////////////////////////////////////////////////////////
BOOL CPropSheet::AddPageCLSID(ULONG count, LPCLSID pArray)
{
	m_ClsIDs = new CLSID[count];
	if(m_ClsIDs == NULL)
		return FALSE;

	for(ULONG i = 0; i < count; i++)
	{
		m_ClsIDs[i] = pArray[i];
	}

	m_iPageCnt = (int)count;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//����:������ģ������ҳ��
//����:pParentWnd[in]		ĸ���ڶ���
//		dwStyle[in]			���ڵķ��
//		dwExStyle[in]		��չ���
////////////////////////////////////////////////////////////////////////////
BOOL CPropSheet::Create(CWnd* pParentWnd, DWORD dwStyle, DWORD dwExStyle)
{
	// TODO: Add your specialized code here and/or call the base class
	ASSERT(m_aryUnk != NULL);

	//��������ҳ
	for (UINT i = 0; i < m_iPageCnt; i++)
	{
		CPropPage * pPage = new CPropPage(m_ClsIDs[i], m_aryUnk, m_nObjCounts);
		if (pPage == NULL)
			return FALSE;
		m_pageArray.Add(pPage);
		AddPage((CPropPage*)m_pageArray[i]);			
	}
	
	return CPropertySheet::Create(pParentWnd, dwStyle, dwExStyle);
}

////////////////////////////////////////////////////////////////
//����:	�ƶ�����������ָ��λ��
//����: nID[in]		��ť���ڵ�ID��
//		x[in]		��ť��x����
/////////////////////////////////////////////////////////////////
void CPropSheet::MoveButton(UINT nID, int x)
{
	CWnd* pWnd = GetDlgItem(nID);
	if(pWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect);
	CRect tmpRect;
	pWnd->GetWindowRect(&tmpRect);
	ScreenToClient(&tmpRect);

	int h = tmpRect.bottom - tmpRect.top;
	int w = tmpRect.Width();
	tmpRect.left = x;
	tmpRect.right = x + w;
	tmpRect.top = rect.bottom - 35;
	tmpRect.bottom = tmpRect.top + h;

	pWnd->MoveWindow(&tmpRect);
	pWnd->ShowWindow(SW_SHOW);
}

//////////////////////////////////////////////////////////////////
//����:��ʼ��������ҳ��,����ҳ����,�������ڴ�С�Ϳؼ�λ��
//////////////////////////////////////////////////////////////////
BOOL CPropSheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
	
	int count = GetPageCount();
	SIZE size = {0, 0};
	for(int i = 0; i < count; i++)
	{
		CPropPage* pPage = (CPropPage *)GetPage(i);
		SIZE tmpSize = pPage->GetPageSize();
		if(tmpSize.cx > size.cx)
			size.cx = tmpSize.cx;
		if(tmpSize.cy > size.cy)
			size.cy = tmpSize.cy;
	}

	//���������ڴ�С
	CRect rect;
	GetTabControl()->GetItemRect(0, &rect);
	int tabHeight = rect.Height();
	int tabUnusedBottom = 45;
	int dxdy = GetSystemMetrics(SM_CXEDGE) * 2;
	GetWindowRect(&rect);
	rect.right = rect.left + size.cx + 24;
	rect.bottom = rect.top + size.cy + tabHeight + tabUnusedBottom + dxdy * 2;
	MoveWindow(&rect);

	//��ʾ�ؼ�
	ShowButtons();

	//����TAB�Ĵ�С
	GetClientRect(&rect);
	rect.DeflateRect(dxdy, dxdy, dxdy, tabUnusedBottom);
	GetTabControl()->MoveWindow(&rect);	

	//����ҳ��Ĵ�С
	GetTabControl()->GetClientRect(rect);
	rect.left += dxdy;
	rect.right += dxdy;
	rect.top += dxdy;
	rect.bottom += dxdy;
	rect.DeflateRect(dxdy, tabHeight + dxdy, dxdy, dxdy);
	for(i = 0; i < count; i++)
	{
		CPropPage* pPage = (CPropPage *)GetPage(i);
		if(pPage->m_hWnd == NULL)
			continue;
		
		pPage->MoveWindow(&rect);
		pPage->ResetPropRect(&rect);

		TCITEM itemInfo;
		itemInfo.mask = TCIF_TEXT;
		CString str = pPage->GetPageTitle();
		LPTSTR pBuffer = str.LockBuffer();
		itemInfo.pszText = pBuffer;
		GetTabControl()->SetItem(i, &itemInfo);
		str.UnlockBuffer();
	}

	CenterWindow();
	InitTabImage();
	OnChangeIcon();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////////////////////////////////
//����:ɾ��������
///////////////////////////////////////////////////////////////////////
void CPropSheet::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet::PostNcDestroy();

	delete this;
}

/////////////////////////////////////////////////////////////
//����:��ʾ���е�ProperSheet�ϵ�Button
/////////////////////////////////////////////////////////////
void CPropSheet::ShowButtons()
{
	CRect rect;
	CRect tmpRect;
	CWnd* pWnd = GetDlgItem(IDOK);
	if(pWnd == NULL)
		return;
	pWnd->GetClientRect(&tmpRect);
	GetClientRect(&rect);


	int step = tmpRect.Width();
	int ntmp = (rect.Width() - 4 * tmpRect.Width()) / 5 ;
	int x = rect.right - ntmp;

	x -= step;
	MoveButton(0x9, x);
	x -= step + ntmp;
	MoveButton(0x3021, x);
	x -= step + ntmp;
	MoveButton(IDCANCEL, x);
	x -= step + ntmp;
	MoveButton(IDOK, x);
}

///////////////////////////////////////////////////////////////////////
//����:������ҳ���е�tab�ؼ��ϼ�����Ҫ��ͼ�����
///////////////////////////////////////////////////////////////////////
void CPropSheet::InitTabImage()
{
	m_imageTab.Create( IDB_TAB, 13, 1, RGB(255,255,255) );
	CTabCtrl *pTab = GetTabControl();
	pTab->SetImageList( &m_imageTab );

	OnChangeIcon();
}

//////////////////////////////////////////////////////////////////////
//����:ҳ���ϵĽӿں����ڸı����Ӧ��ͼ��
//////////////////////////////////////////////////////////////////////
void CPropSheet::OnChangeIcon()
{
	CTabCtrl *pTab = GetTabControl();

	TC_ITEM tcItem;
	tcItem.mask = TCIF_IMAGE;
	int count = GetPageCount();
	for( int i = 0; i < count; i++ )
	{
		CPropPage* pPage = (CPropPage*)GetPage(i);
		tcItem.iImage = pPage->m_lImageIndex;
		pTab->SetItem( i, &tcItem );
	}
}

///////////////////////////////////////////////////////////////////////
//����:������ģ������ҳ����
///////////////////////////////////////////////////////////////////////
int CPropSheet::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	ASSERT(m_aryUnk != NULL);

	//��������ҳ
	for (UINT i = 0; i < m_iPageCnt; i++)
	{
		CPropPage * pPage = new CPropPage(m_ClsIDs[i], m_aryUnk, m_nObjCounts);
		if (pPage == NULL)
			return FALSE;
		m_pageArray.Add(pPage);
		AddPage((CPropPage*)m_pageArray[i]);			
	}
	
	return CPropertySheet::DoModal();
}

void CPropSheet::EnableApplyButton(BOOL bEnable)
{
	CWnd* pWnd = GetDlgItem(0x3021);
	if (pWnd == NULL)
		return;

	pWnd->EnableWindow(bEnable);
}
