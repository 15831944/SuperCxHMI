////////////////////////////////////////////////////////////////////////////////////////////////////
//CxComVariant.h : user-defined VARIANTC class, the define and implement file
//������Ŀ: AutoCx��̬����ҳ
//��������: 2001��11��
//�� �� ��: ¬�Ľ�
//����˵��: ����CComVariant��ʵ��VARIANT�����Ĺ���
//�޸ļ�¼:
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "atlbase.h"

class CxComVariant : public CComVariant
{
public:
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//����: ����<���÷�
	//˵��: �˲��������з�����,��var1 < var2,�ǽ�var2.vtת��Ϊvar1.vt�ٱȽ���Ӧ��ֵ
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	bool operator<=(const VARIANT& varSrc) const
	{
		//���Ϊpointer�����������򷵻�false
		if ((vt & VT_BYREF) == VT_BYREF || (vt & VT_ARRAY) == VT_ARRAY)
			return false;
		
		//�����������
		if (this == &varSrc)
			return false;
				
		switch (vt)
		{
		
		case VT_ERROR:
		case VT_UNKNOWN:
		case VT_DISPATCH:
		case VT_EMPTY:
		case VT_NULL:
			return false;

		case VT_I1:
		case VT_I2:
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_CY:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_BOOL:
		case VT_DATE:
			{
				CComVariant varLeft(*this);
				CComVariant varRight(varSrc);
				::VariantChangeType(&varLeft, &varLeft, 0, VT_R8);
				::VariantChangeType(&varRight, &varRight, 0, VT_R8);
				return varLeft.dblVal <= varRight.dblVal;
			}
		case VT_BSTR:
			{
				if (bstrVal == NULL && varSrc.bstrVal == NULL)
					return false;
				if (bstrVal != NULL && varSrc.bstrVal != NULL)
					return wcscmp(bstrVal, varSrc.bstrVal) <= 0;
			}

		default:
			ATLASSERT(false);
			// fall through
		}
		
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//����: ����>���÷�
	//˵��: �˲��������з�����,��var1 > var2,�ǽ�var2.vtת��Ϊvar1.vt�ٱȽ���Ӧ��ֵ
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	bool operator>=(const VARIANT& varSrc) const
	{
		//���Ϊpointer�����������򷵻�false
		if ((vt & VT_BYREF) == VT_BYREF || (vt & VT_ARRAY) == VT_ARRAY)
			return false;
		
		//�����������
		if (this == &varSrc)
			return false;
				
		switch (vt)
		{
		
		case VT_ERROR:
		case VT_UNKNOWN:
		case VT_DISPATCH:
		case VT_EMPTY:
		case VT_NULL:
			return false;

		case VT_I1:
		case VT_I2:
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_CY:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_BOOL:
		case VT_DATE:
			{
				CComVariant varLeft(*this);
				CComVariant varRight(varSrc);
				::VariantChangeType(&varLeft, &varLeft, 0, VT_R8);
				::VariantChangeType(&varRight, &varRight, 0, VT_R8);
				return varLeft.dblVal >= varRight.dblVal;
			}
		case VT_BSTR:
			{
				if (bstrVal == NULL && varSrc.bstrVal == NULL)
					return false;
				if (bstrVal != NULL && varSrc.bstrVal != NULL)
					return wcscmp(bstrVal, varSrc.bstrVal) >= 0;
			}

		default:
			ATLASSERT(false);
			// fall through
		}
		
		return false;
	}

	bool operator==(const VARIANT& varSrc) const
	{
		//���Ϊpointer�����������򷵻�false
		if ((vt & VT_BYREF) == VT_BYREF || (vt & VT_ARRAY) == VT_ARRAY)
			return false;
		
		//�����������
		if (this == &varSrc)
			return false;
				
		switch (vt)
		{
		
		case VT_ERROR:
		case VT_UNKNOWN:
		case VT_DISPATCH:
		case VT_EMPTY:
		case VT_NULL:
			return false;

		case VT_I1:
		case VT_I2:
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_CY:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_BOOL:
		case VT_DATE:
			{
				CComVariant varLeft(*this);
				CComVariant varRight(varSrc);
				::VariantChangeType(&varLeft, &varLeft, 0, VT_R8);
				::VariantChangeType(&varRight, &varRight, 0, VT_R8);
				return varLeft.dblVal == varRight.dblVal;
			}
		case VT_BSTR:
			{
				if (bstrVal == NULL && varSrc.bstrVal == NULL)
					return false;
				if (bstrVal != NULL && varSrc.bstrVal != NULL)
					return wcscmp(bstrVal, varSrc.bstrVal) == 0;
			}

		default:
			ATLASSERT(false);
			// fall through
		}
		
		return false;
	}
};