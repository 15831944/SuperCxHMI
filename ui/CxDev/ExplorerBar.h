// ExplorerBar.h: interface for the CExplorerBar class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __EXPLORERBAR_H__
#define __EXPLORERBAR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���빤�߰���˳�����
#define EXPBAR_EXPTREE	0		//���̹��ߴ���
#define EXPBAR_MODULE	2		//ģʽ���ߴ���
#define EXPBAR_SYMBOL	1		//���Ź��ߴ���
#define EXPBAR_NULL		3		//��Ч���ߴ���

class CExplorerBar : public CTabBar  
{
public:
	CExplorerBar();
	virtual ~CExplorerBar();

// Attributes
public:
	void DestroyAllChild();
	CWnd* GetToolWnd(int nWhich);
//	CExplorerTree m_wndExplorerTree;
//	CSymbolLibWnd m_wndSymbolLib;
//	CModuleWnd m_wndModuleLib;

// Overridables


protected:
    //{{AFX_MSG(CExplorerBar)
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
protected:
	virtual void OnAddPages();
};

#endif // !__EXPLORERBAR_H__
