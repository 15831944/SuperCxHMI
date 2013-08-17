// PropInfo.h
//���ļ����ڶ����ڱ༭��̬����ʱ��Ҫ���ݵ�������Ϣ

#ifndef __CXDYNAMICINFO_H_
#define __CXDYNAMICINFO_H_

#include "CxHmiMisc.h"

#define CSTATE_DISABLE -1
#define CSTATE_DISCONNECT_NOTSUPPORTMOUSEINPUT	0x0  //û�й�������֧���������
#define CSTATE_DISCONNECT_SUPPORTMOUSEINPUT	0x1	 //û�й�����֧���������
#define CSTATE_CONNECT_NOTSUPPORTMOUSEINPUT	0x2	 //��������֧���������
#define CSTATE_CONNECT_SUPPORTMOUSEINPUT		0x3  //������֧���������
#define CSTATE_CONNECT_ENABLEMOUSEINPUT		0x7  //������֧��������벢ʹ��


//iFlag == 0; //�������ʽ
//iFlag == 1; //��ԭʼ���ʽ����ʾ���ʽ
//iFlag == 2; //����ʾ���ʽ��ԭʼ���ʽ
typedef void (*ExpressionCreatorProc)(BSTR* pbstrExpression, int iFlag = 0);

class CPropInfo
{
public:
	CComBSTR m_bstrName;
	CComBSTR m_bstrDesc;
	enumPropType m_proptype;
	DISPID m_dispid;
	CComVariant m_value;
public:
	CPropInfo()
	{
		m_dispid = 0;
		m_proptype = PROPTYPE_UNKNOWN;
	}

	CPropInfo(const CPropInfo& propinfo)
	{
		InternalCopy(propinfo);
	}
	
	CPropInfo& operator = (CPropInfo& propinfo)
	{
		InternalCopy(propinfo);
		return *this;
	}

	void InternalCopy(const CPropInfo& propinfo)
	{
		m_bstrName = propinfo.m_bstrName;
		m_bstrDesc = propinfo.m_bstrDesc;
		m_proptype = propinfo.m_proptype;
		m_dispid = propinfo.m_dispid;
		m_value = propinfo.m_value;
	}
};

typedef CSimpleArray<CPropInfo> CPropInfoArray;

class CEnumPropItem
{
public:
	CComBSTR m_bstrName;
	CComBSTR m_bstrDesc;
	CComVariant m_varValue;
	
	CEnumPropItem()
	{
	}
	
	CEnumPropItem(const CEnumPropItem& item)
	{
		InternalCopy(item);
	}
	
	CEnumPropItem& operator = (CEnumPropItem& item)
	{
		InternalCopy(item);
		return *this;
	}
	
	void InternalCopy(const CEnumPropItem& item)
	{
		m_bstrName = item.m_bstrName;
		m_bstrDesc = item.m_bstrDesc;
		m_varValue = item.m_varValue;
	}
};

typedef CSimpleArray<CEnumPropItem> CEnumPropItemArray;

class CDynamicPropInfo
{
public:
	CComPtr<IUnknown> spUnkObject; //����
	CComPtr<IUnknown> spUnkDynamic; //��̬���Զ���
	ExpressionCreatorProc pfnExpression;
	CComBSTR bstrPropertyName;	//��������
	CComVariant varCurrentValue; //��ǰֵ
	BOOL bSupportMouseInput;
	enumPropType nType;
	CEnumPropItemArray m_arrEnumPropItems;
};


#endif //__CXDYNAMICINFO_H_