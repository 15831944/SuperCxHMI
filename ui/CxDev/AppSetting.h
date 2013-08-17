// AppSetting.h: interface for the CAppSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPSETTING_H__B9D95BB1_1384_4EE9_8429_C475DA9A1F9C__INCLUDED_)
#define AFX_APPSETTING_H__B9D95BB1_1384_4EE9_8429_C475DA9A1F9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAppSetting  
{
public:
	CAppSetting(LPCTSTR lpszSection);
	virtual ~CAppSetting();

	virtual BOOL LoadSettings() = 0;
	virtual BOOL SaveSettings() = 0;

	CString m_strSection;
};

class CFormSetting: public CAppSetting
{
public:
	CFormSetting(LPCTSTR lpszSection);
	  
	virtual BOOL LoadSettings();	
	virtual BOOL SaveSettings();
	  
public:
	COLORREF m_clrBack;		//ǰ��ɫ
	COLORREF m_clrFore;		//����ɫ
	int m_nHeight;			//Ĭ�ϴ����
	int m_nWidth;			//Ĭ�ϴ����
};

class CObjectSetting : public CAppSetting
{
public:
	CObjectSetting(LPCTSTR lpszSection);
	
	virtual BOOL LoadSettings();	
	virtual BOOL SaveSettings();

public:
	OLE_COLOR m_clrEdgeColor;
	OLE_COLOR m_clrFillColor;
	OLE_COLOR m_clrFontColor;
};

class CGenSetting : public CAppSetting
{
public:
	CGenSetting(LPCTSTR lpszSection);
	
	virtual BOOL LoadSettings();	
	virtual BOOL SaveSettings();
	
public:
	BOOL m_bGrid;			//�Ƿ���ʾ����
	COLORREF m_clrGrid;		//������ɫ
	int m_nGridSpaceWidth;
	int m_nGridSpaceHeight;
	BOOL m_bAlignToGrid;
	CRect m_rectPageMargin;
	BOOL m_bShowScriptSymbol;
};


#endif // !defined(AFX_APPSETTING_H__B9D95BB1_1384_4EE9_8429_C475DA9A1F9C__INCLUDED_)
