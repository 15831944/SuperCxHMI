// TagPaser.h : ��ǩ����

#ifndef __TAGPASER_H_
#define __TAGPASER_H_

class CTagPaser
{
public:
	CComBSTR m_bstrText;


	CTagPaser()
	{
	}

	CTagPaser(BSTR bstrText)
	{
		m_bstrText = bstrText;
	}

	void PaserText(BSTR bstrText)
	{
		m_bstrText = bstrText;
	}
	static BOOL ReadStrictTag(BSTR bstrText, BSTR* pbstrNode, BSTR* pbstrBlock, BSTR* pbstrProp)
	{
		OLECHAR* p = bstrText;

		//���˿ո�
		while (*p == OLECHAR(' '))
			p++;

		if ((p[0] == OLECHAR('T') || p[0] == OLECHAR('t')) &&
			(p[1] == OLECHAR('A') || p[1] == OLECHAR('a')) &&
			(p[2] == OLECHAR('G') || p[2] == OLECHAR('g')) &&
			(p[3] == OLECHAR('S') || p[3] == OLECHAR('s')) &&
			p[4] == OLECHAR('.'))
		{
			p = p + 5;
			int n = ScanNodeName(p);
			if (n > 0)
			{
				//�����������
				if (*pbstrBlock = ScanDotString(p))
				{
					*pbstrProp = ScanDotString(p);
				}
				return TRUE;
			}
		}

		return FALSE;
	}

	BOOL ReadTag(OLECHAR*& p, BSTR* pbstrNode, BSTR* pbstrBlock, BSTR* pbstrProp)
	{
		OLECHAR* p1;
		int nTotal = ::SysStringLen(m_bstrText) - (p - m_bstrText.m_str);
		while (nTotal)
		{
			if ((p[0] == OLECHAR('T') || p[0] == OLECHAR('t')) &&
				(p[1] == OLECHAR('A') || p[1] == OLECHAR('a')) &&
				(p[2] == OLECHAR('G') || p[2] == OLECHAR('g')) &&
				(p[3] == OLECHAR('S') || p[3] == OLECHAR('s')) &&
				p[4] == OLECHAR('.'))
			{
				p1 = p + 5;
				int n = ScanNodeName(p1);
				if (n > 0)
				{
					//�����������
					if (*pbstrBlock = ScanDotString(p1))
					{
						*pbstrProp = ScanDotString(p1);
					}
					p = p1;
					return TRUE;
				}
				p++;
				nTotal--;
			}
		}

		return FALSE;
	}

	/////////////////////////////////////////////////////////////////////////
	//�� �ܣ�����������
	//�� �����ַ���ָ��
	//�� �أ����������
	/////////////////////////////////////////////////////////////////////////
	static int ScanNodeName(OLECHAR*& p)
	{
		if ((p[0] == OLECHAR('T') || p[0] == OLECHAR('t')) &&
			(p[1] == OLECHAR('H') || p[1] == OLECHAR('h')) &&
			(p[2] == OLECHAR('I') || p[2] == OLECHAR('i')) &&
			(p[3] == OLECHAR('S') || p[3] == OLECHAR('s')) &&
			(p[4] == OLECHAR('N') || p[4] == OLECHAR('n')) &&
			(p[5] == OLECHAR('O') || p[5] == OLECHAR('o')) &&
			(p[6] == OLECHAR('D') || p[6] == OLECHAR('d')) &&
			(p[7] == OLECHAR('E') || p[7] == OLECHAR('e')))
		{
			p += 8;
			return 8;
		}

		return 0;
	}

	/////////////////////////////////////////////////////////////////////////
	//�� �ܣ������Ե㿪ͷ���ַ���
	//�� �����ַ���ָ��
	//�� �أ�������.�ŵĵĺϷ������ַ���
	/////////////////////////////////////////////////////////////////////////
	static BSTR ScanDotString(OLECHAR*& p)
	{
		if (*p == OLECHAR('.'))
		{
			p++;
			if (iswalpha(*p))
			{
				int iLength = 1;
				while (iswalnum(*(p + iLength)) || *(p + iLength) == OLECHAR('_'))
				{
					iLength++;
				}
				p += iLength;
				return SysAllocStringLen(p - iLength, iLength);
			}
		}
		return NULL;
	}

};

#endif //__TAGPASER_H_