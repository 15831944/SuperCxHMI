// ChangePasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RunInc.h"
#include "ChangePasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDlg dialog


CChangePasswordDlg::CChangePasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePasswordDlg)
	m_strUserName = _T("");
	m_strOldPassword = _T("");
	m_strNewPassword = _T("");
	m_strVerifyPassword = _T("");
	//}}AFX_DATA_INIT
}


void CChangePasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePasswordDlg)
	DDX_Text(pDX, IDC_USER_NAME, m_strUserName);
	DDX_Text(pDX, IDC_PASSWORD, m_strOldPassword);
	DDX_Text(pDX, IDC_PASSWORD2, m_strNewPassword);
	DDX_Text(pDX, IDC_PASSWORD3, m_strVerifyPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangePasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CChangePasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDlg message handlers

void CChangePasswordDlg::OnOK() 
{
	USES_CONVERSION;

	UpdateData();

	m_strUserName.TrimLeft();
	m_strUserName.TrimRight();
	if (m_strUserName.IsEmpty())
	{
		MessageBox(_T("�û�������Ϊ�ա�"), _T("�û�������"), MB_ICONERROR);
		return;
	}

	if (m_strNewPassword != m_strVerifyPassword)
	{
		MessageBox(_T("�������ȷ�����벻ƥ�䡣"), _T("�������"), MB_ICONERROR);
		return;
	}

	LPWSTR szDesc = NULL;
	LPWSTR szPassword = NULL;
	LPWSTR szGroup = NULL;
	DWORD dwPrivilege;
	LPWSTR szAccessSecrityAreas = NULL;

	HRESULT hr = m_pUserManager->GetUserInfo(
		T2CW(m_strUserName), 
		&szDesc,
		&szPassword,
		&szGroup,
		&dwPrivilege,
		&szAccessSecrityAreas);
	if (hr != S_OK)
	{
		MessageBox(_T("ָ�����û������ڻ��޷��ӷ�������ȡ�û���Ϣ��"), _T("��ȡ�û���Ϣʧ��"), MB_ICONERROR);
		CDialog::OnOK();
		return;
	}

	CString strDesc = szDesc;
	CString strGroup = szGroup;
	CString strPassword = szPassword;
	CString strAccessSecrityAreas = szAccessSecrityAreas;

	if (szDesc != NULL)
		CoTaskMemFree(szDesc);
	if (szPassword != NULL)
		CoTaskMemFree(szPassword);
	if (szGroup != NULL)
		CoTaskMemFree(szGroup);
	if (szAccessSecrityAreas != NULL)
		CoTaskMemFree(szAccessSecrityAreas);

	if (m_strOldPassword != strPassword)
	{
		MessageBox(_T("��������ȷ�ľ����롣"), _T("�������"), MB_ICONERROR);
		return;
	}
	
	hr = m_pUserManager->SetUserInfo(
		T2CW(m_strUserName),
		T2CW(m_strUserName),
		T2CW(strDesc),
		T2CW(m_strNewPassword),
		T2CW(strGroup),
		dwPrivilege,
		T2CW(strAccessSecrityAreas));
	if (FAILED(hr))
		return;

	if (FAILED(m_pUserManager->SaveUsersInfo()))
		MessageBox(_T("����������������ʹ���û���Ϣ��"), _T("�û���Ϣ����ʧ��"), MB_ICONERROR);
	
	CDialog::OnOK();
}
