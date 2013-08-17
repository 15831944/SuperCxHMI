// TypeChanger.cpp: implementation of the CTypeChanger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DevInc.h"
#include "CxDevIF.h"
#include "TypeChanger.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

struct EXTYPEENTRY
{
	LPTSTR typeName;
	enumPropType type;
};

static struct EXTYPEENTRY staticAryExType[] = 
{
	{_T("��ɫ"), PROPTYPE_COLOR},	{_T("����"), PROPTYPE_BOOL}, 
	{_T("�ַ���"), PROPTYPE_STRING},{_T("����"), PROPTYPE_NUMBER},
//	{_T("ö��"), PROPTYPE_ENUM}
};


///////////////////////////////////////////////////////////////
//����:����enumType��ָ��չ�������͵�����
///////////////////////////////////////////////////////////////
CString CTypeChanger::GetExTypeName(enumPropType enumType)
{
	CString strRet = _T("δ֪����");

	int nCount = GetExTypeCount();
	for (int i = 0; i < nCount; i++)
	{
		if (staticAryExType[i].type == enumType)
		{
			strRet = staticAryExType[i].typeName;
			break;
		}
	}

	return strRet;
}

//////////////////////////////////////////////////////////////
//����:����֧�ֵ���չ���Ե�����
//plan, realtime, ctrl, info,
//////////////////////////////////////////////////////////////
int CTypeChanger::GetExTypeCount()
{
	return sizeof(staticAryExType) / sizeof(EXTYPEENTRY);
}

//////////////////////////////////////////////////////////////
//����:���ص�nIndex����չ�������͵�ö��ֵ
//////////////////////////////////////////////////////////////
enumPropType CTypeChanger::GetExTypeID(int nIndex)
{
	if(nIndex >= GetExTypeCount())
		return PROPTYPE_UNKNOWN;

	return staticAryExType[nIndex].type;
}

//////////////////////////////////////////////////////////////
//����:����չ�������͵�����ת��Ϊ�����ö��
//////////////////////////////////////////////////////////////
enumPropType CTypeChanger::GetExType(CString &strTypeName)
{
	int nCount = sizeof(staticAryExType) / sizeof(EXTYPEENTRY);
	for(int i = 0; i < nCount; i++)
	{
		CString str = staticAryExType[i].typeName;
		if(str == strTypeName)
			return staticAryExType[i].type;
	}

	return PROPTYPE_UNKNOWN;
}


HRESULT CTypeChanger::GetPropertyByName(IUnknown* pUnknown, LPCSTR strProp, VARIANT* pVar)
{
	USES_CONVERSION;
	
	IDispatchPtr pDisp = pUnknown;
	
	LPOLESTR lpszName = T2OLE(strProp);
	
	DISPID dwDispID;
	pDisp->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpszName, 1, LOCALE_USER_DEFAULT, &dwDispID);
	
	DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
	return pDisp->Invoke(dwDispID, IID_NULL,
		LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
		&dispparamsNoArgs, pVar, NULL, NULL);
}

HRESULT CTypeChanger::PutPropertyByName(IUnknown* pUnknown, LPCSTR strProp, VARIANT* pVar)
{
	USES_CONVERSION;
	
	IDispatchPtr pDisp = pUnknown;
	
	LPOLESTR lpszName = T2OLE(strProp);
	
	DISPID dwDispID;
	pDisp->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpszName, 1, LOCALE_USER_DEFAULT, &dwDispID);
	
	DISPPARAMS dispparams = {NULL, NULL, 1, 1};
	dispparams.rgvarg = pVar;
	DISPID dispidPut = DISPID_PROPERTYPUT;
	dispparams.rgdispidNamedArgs = &dispidPut;
	
	if (pVar->vt == VT_UNKNOWN || pVar->vt == VT_DISPATCH || 
		(pVar->vt & VT_ARRAY) || (pVar->vt & VT_BYREF))
	{
		HRESULT hr = pDisp->Invoke(dwDispID, IID_NULL,
			LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF,
			&dispparams, NULL, NULL, NULL);
		if (SUCCEEDED(hr))
			return hr;
	}
	
	return pDisp->Invoke(dwDispID, IID_NULL,
		LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
		&dispparams, NULL, NULL, NULL);
}