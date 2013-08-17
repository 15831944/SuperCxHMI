#if !defined(AFX_PROPSHEET_H__E7FFE618_E196_4F89_95EE_C57DE56C6A99__INCLUDED_)
#define AFX_PROPSHEET_H__E7FFE618_E196_4F89_95EE_C57DE56C6A99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropSheet.h : header file
// ������Ŀ: AutoCx��CxPropSheet����
// ��������: 2001.10.8
// �� �� �ˣ���Խ�� 
// ��������������ҳ������������,���ڴ����͹�������ҳ�� 
// �޸ļ�¼��
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPropSheet

class CPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropSheet)

// Construction
public:
	CPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropSheet)
	public:
	virtual int DoModal();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	void EnableApplyButton(BOOL bEnable = TRUE);
	void OnChangeIcon();
	virtual BOOL Create(CWnd* pParentWnd=NULL, DWORD dwStyle=(DWORD)(-1), DWORD dwExStyle=0);
	BOOL AddPageCLSID(ULONG  count, LPCLSID pArray);
	BOOL AddObjects(ULONG count, LPUNKNOWN* pArray);
	virtual ~CPropSheet();

	// Generated message map functions
protected:
	void InitTabImage();
	void ShowButtons();
	void MoveButton(UINT nID, int x);
	//{{AFX_MSG(CPropSheet)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LPUNKNOWN* m_aryUnk;	//������ҳ��OLE�����Unknownָ��
	int m_nObjCounts;	//����ĸ���
	CPtrArray m_pageArray;//����ҳ��
	UINT m_iPageCnt;	//����ҳ����
	CLSID* m_ClsIDs;
	CImageList m_imageTab;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPSHEET_H__E7FFE618_E196_4F89_95EE_C57DE56C6A99__INCLUDED_)
