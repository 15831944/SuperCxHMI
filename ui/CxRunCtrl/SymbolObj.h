// SymbolObj.h: interface for the CSymbolObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYMBOLOBJ_H__2DE353FE_62D2_4143_85B4_03128C98E88E__INCLUDED_)
#define AFX_SYMBOLOBJ_H__2DE353FE_62D2_4143_85B4_03128C98E88E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPropInfo
{
public:
	CString m_strName;
	CString m_strDescription;
	enumPropType m_proptype;
	DISPID m_dispid;
	_variant_t m_value;
public:
	CPropInfo()
	{
		m_dispid = 0;
		m_proptype = PROPTYPE_UNKNOWN;
	}

	void operator = (CPropInfo& source)
	{
		m_strName = source.m_strName;
		m_strDescription = source.m_strDescription;
		m_proptype = source.m_proptype;
		m_dispid = source.m_dispid;
	}
};

_COM_SMARTPTR_TYPEDEF(ICxDynamicObject, __uuidof(ICxDynamicObject));

class CSymbolProp : public CPropInfo
{
public:
	CSymbolProp()
	{
		m_bNeedUpdateValue = TRUE;
	}
	
	class CRelatedProp
	{
	public:
		CRelatedProp()
		{
			m_nSubIndex = -1;
		}
		CRelatedProp& operator = (const CRelatedProp& source)
		{
			m_nSubIndex = source.m_nSubIndex;
			m_strPropName = source.m_strPropName;
			m_pDynamicObject = source.m_pDynamicObject;
			return *this;
		}

	public:
		int m_nSubIndex;
		CString m_strPropName;	//�Ӷ����еĶ�Ӧ������
		ICxDynamicObjectPtr m_pDynamicObject;
	};

public:
	CArray<CRelatedProp, CRelatedProp&> m_arrObjNum;

	CString m_strValidScript;

	BOOL m_bNeedUpdateValue; //��Ҫ����������
	
	void Serialize(CArchive& ar);
	void operator = (const CSymbolProp& source)
	{
		m_arrObjNum.RemoveAll();

		m_value = source.m_value;
		m_strName = source.m_strName;
		m_strDescription = source.m_strDescription;
		m_proptype = source.m_proptype;
		m_dispid = source.m_dispid;
		m_strValidScript = source.m_strValidScript;

		m_arrObjNum.RemoveAll();
		for(int i = 0; i < source.m_arrObjNum.GetSize(); i++)
		{
			m_arrObjNum.Add(source.m_arrObjNum[i]);
		}
	}
protected:
	void SerializeExtendInterface(CArchive& ar, ICxDynamicObjectPtr& pIn, ICxDynamicObjectPtr& pOutk);
	void SerializeExData(CArchive& ar, _variant_t& data);
};

class CSymbolObj;

class CSymbolScrEng : public CScriptEngine  
{
public:
	CSymbolScrEng(CSymbolObj* pObj)
	{
		ASSERT(pObj != NULL);
		m_pObject = pObj;
	}

// Attributes
public:
	CSymbolObj* m_pObject;
	
// Operations
public:
	virtual HRESULT AddItemsName();
	virtual HRESULT GetItemDispatch(LPCOLESTR pszName, IUnknown** ppDispatch);
};

class CSymbolObj : public CGroupObj  
{
	DECLARE_SERIAL(CSymbolObj)

public:
	CSymbolObj(CDevObjDoc* pContainerDoc = NULL);
	virtual ~CSymbolObj();
		
protected:
	CArray<CSymbolProp, CSymbolProp&> m_arrExProp;	//��չ����
	CSymbolScrEng* m_pScriptEngine;
	
public:
	virtual void Release();
	virtual CRect GetBoundingRect();

	BOOL m_bParsingDynamic;
	virtual void ParseDynamic();


	BOOL InitScriptEngine();

	BOOL IsValidExPropValue(CSymbolProp* pProp, VARIANT* pvaValue);
	HRESULT UpdateAllExPropValue();

	BOOL HasMouseInputDynamic(DISPID dispid = DISPID_UNKNOWN);
	BOOL ProcessMouseInput(CDevObj* pObj, LPCSTR szPropName, BOOL bProcess);
	
public:
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CSymbolObj)
	afx_msg VARIANT_BOOL IsConnected(LPCTSTR pszProertyName);
	afx_msg void ConnectObject(LPCTSTR lpszProertyName, LPUNKNOWN punkObject);
	afx_msg void ConnectDirect(LPCTSTR lpszProertyName, LPCTSTR pszDataSource);
	afx_msg LPUNKNOWN GetConnectObject(LPCTSTR lpszProertyName);
	afx_msg void Disconnect(LPCTSTR lpszProertyName);
	afx_msg int GetConnectState(DISPID dispid);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	
	//��չ�����Զ����ӿ�
	BEGIN_INTERFACE_PART(DispatchExtend, IDispatch)
		STDMETHOD(GetIDsOfNames)(REFIID iid, LPOLESTR* ppszNames, UINT nNames,
			LCID lcid, DISPID* pDispIDs);
		STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID lcid,
			ITypeInfo** ppTypeInfo);
		STDMETHOD(GetTypeInfoCount)(UINT* pnInfoCount);
		STDMETHOD(Invoke)(DISPID dispidMember, REFIID iid, LCID lcid,
			WORD wFlags, DISPPARAMS* pdpParams, VARIANT* pvarResult,
			EXCEPINFO* pExceptionInfo, UINT* piArgError);
	END_INTERFACE_PART(DispatchExtend)

	BEGIN_INTERFACE_PART(ScriptEngineSite, ICxScriptEngineSite)
		STDMETHOD(EvaluateExpression)(BSTR bstrSource, VARIANT* pvaValidObjects);
		STDMETHOD(Execute)(BSTR bstrStatement);
		STDMETHOD(PreTreatExpression)(BSTR bstrSource, BSTR* pbstrSource);
		STDMETHOD(PreTreatExpressionEx)(BSTR bstrSource, BSTR* pbstrSource, VARIANT* pvaMin, VARIANT* pvaMax);
	END_INTERFACE_PART(ScriptEngineSite)

	DECLARE_INTERFACE_MAP()

protected:
	virtual void Serialize(CArchive& ar);

	virtual BOOL GetDispatchIID(IID* piid);
	
	HRESULT GetExtendProperty(DISPID dwDispID, VARIANT* pVar);
	HRESULT PutExtendProperty(DISPID dwDispID, VARIANT* pVar);
};

#endif // !defined(AFX_SYMBOLOBJ_H__2DE353FE_62D2_4143_85B4_03128C98E88E__INCLUDED_)
