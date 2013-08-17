// TestPage.h : Declaration of the CTestPage
// ������Ŀ: AutoCx��TestPages����
// ��������: 2001.10.8
// �� �� �ˣ���Խ�� 
// ����������û���õ����� 
// �޸ļ�¼��
/////////////////////////////////////////////////////////////////////////////

#ifndef __TESTPAGE_H_
#define __TESTPAGE_H_

#include "resource.h"       // main symbols
#include <atlhost.h>

/////////////////////////////////////////////////////////////////////////////
// CTestPage
class CTestPage : 
	public CAxDialogImpl<CTestPage>
{
public:
	CTestPage()
	{
	}

	~CTestPage()
	{
	}

	enum { IDD = IDD_TESTPAGE };

BEGIN_MSG_MAP(CTestPage)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;  // Let the system set the focus
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
};

#endif //__TESTPAGE_H_
