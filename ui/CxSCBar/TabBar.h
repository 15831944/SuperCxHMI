#if !defined(AFX_TABBAR_H__A38BBD66_A6E7_452A_B1A3_FA4CAEA18BDD__INCLUDED_)
#define AFX_TABBAR_H__A38BBD66_A6E7_452A_B1A3_FA4CAEA18BDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// TabBar.h : header file
//����:��ҳ�Ŀɸ���������
//����:��Խ��
//ʱ��:2002.1.5
//��������:CxScBar
//˵��:ʹ���ߴ���̳�һ����, Ȼ����OnAddPages()����AddPage()�����ҳ�洰��,
//	�粻ʹ��new����CWnd����, ����Ҫ��OnDestroyAllChild()����(������������Ķ�Ӧ����),
//	�����Լ���ʽɾ����ҳ�洰�ڶ���, �����m_listChildWnd. ���ڵ�ͼ�꽫��ʾ��ҳTab��.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CTabBar window

class AFX_EXT_CLASS CTabBar : public CSizingControlBar
{
	class CTabPageInfo
	{
	public:
		CWnd* pPageWnd;		//һҳ��Ӧ�Ĵ���
		int	 indexIcon;		//ͼ���Ӧ������
		CString str;		//��ʾ����
	};

// Construction
public:
	CTabBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTabMode(int nMode);
	void SetImageList(CImageList* pNewImage);
	virtual int AddPage(CWnd* pPage,  const CString& strName, int nIconIndex);
	void ActivatePage(int index);
	virtual ~CTabBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabBar)
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
protected:
	void UpdateScrollArrows();
	void TabScroll(BOOL bLeftOrRight);
	virtual void InitSize();
	int HiTest(CPoint point);
	void GetTabBarRect(CRect* rect);
	BOOL GetArrowRect(CRect& rectBound, CRect &rectLeft, CRect& rectRight);
	void EnSurePageVisible(int nPage);
	void DrawTabCtrl(CDC* pDC, CRect rect);
	void DrawTab(CDC* pDC, CRect rect, CString& strTitle, int indexImage, BOOL bSelected, int nLeftMost);
	void DrawScrollArrows(CDC *pDC, CRect rect, BOOL bWhich, int nState);
	void DrawArrowState(int nWhich, int nState);
	virtual void OnDestroyAllChild();
	virtual void OnAddPages();

protected:
	int m_nTabHeight;
	int m_nTabWidth;
	int m_nImageWidth;
	int m_firstTabID;	//��һ����ʾ������Tab��ID��(��0��ʼ, ��Ϊ-1���ʾû��)
	int m_nArrowSize;	//��ͷ�����
	CArray<CTabPageInfo, CTabPageInfo> m_listChildWnd;
	int m_nActivatePage;
	CImageList* m_pIconList;

private:
	int m_nStateLArrow;
	int m_nStateRArrow;
	RECT m_clientRect;	//�ͻ����������ڵ�λ��
	BOOL m_bMouseTracking;
	int m_nBarStyle;	//���
};

#define TABBAR_MODE_DYNAMIC		0	//�ɹ�����TAB
#define TABBAR_MODE_STATIC		1	//�����Ŵ�С�����ɹ���
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBAR_H__A38BBD66_A6E7_452A_B1A3_FA4CAEA18BDD__INCLUDED_)
