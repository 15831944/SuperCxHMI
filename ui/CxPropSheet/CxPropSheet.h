// CxPropSheet.h : main header file for the CXPROPSHEET DLL
// ������Ŀ: AutoCx��CxPropSheet����
// ��������: 2001.10.8
// �� �� �ˣ���Խ�� 
// ����������AutoCxϵͳ����������Ϊϵͳ�еĸ�������ҳ�ṩ�����ڡ� 
// �޸ļ�¼��
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CXPROPSHEET_H__CE30370F_FE4B_4528_AB1B_AFD7275FCD87__INCLUDED_)
#define AFX_CXPROPSHEET_H__CE30370F_FE4B_4528_AB1B_AFD7275FCD87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

/////////////////////////////////////////////////////////////////////////////
// CCxPropSheetApp
// See CxPropSheet.cpp for the implementation of this class
//
#include "..\CxOle\CxHmiObj.h"
#include "..\CxOle\CxHmiMisc.h"
class CCxPropSheetApp : public CWinApp
{
public:
	CCxPropSheetApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCxPropSheetApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCxPropSheetApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CXPROPSHEET_H__CE30370F_FE4B_4528_AB1B_AFD7275FCD87__INCLUDED_)
