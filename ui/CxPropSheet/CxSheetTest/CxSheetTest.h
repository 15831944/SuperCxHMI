// CxSheetTest.h : main header file for the CXSHEETTEST application
// ������Ŀ: AutoCx��CxSheetTest����
// ��������: 2001.10.8
// �� �� �ˣ���Խ�� 
// �������������ڶ�����ҳ���̽��в��Ե���������,���ڲ��ԡ� 
// �޸ļ�¼��
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CXSHEETTEST_H__B4385C7E_27A0_40B8_A63B_1F05A647157E__INCLUDED_)
#define AFX_CXSHEETTEST_H__B4385C7E_27A0_40B8_A63B_1F05A647157E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCxSheetTestApp:
// See CxSheetTest.cpp for the implementation of this class
//

class CCxSheetTestApp : public CWinApp
{
public:
	CCxSheetTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCxSheetTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCxSheetTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CXSHEETTEST_H__B4385C7E_27A0_40B8_A63B_1F05A647157E__INCLUDED_)
