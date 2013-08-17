// ActionParser.cpp: implementation of the CActionParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ActionParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActionParser::CActionParser(LPCTSTR lpszScript)
{
	m_strScript = lpszScript;
}

CActionParser::~CActionParser()
{

}

// ����ָ��λ��������
void CActionParser::GetPosLine(int nPos, CString& strLine)
{
	LPCTSTR pBuf = m_strScript.GetBuffer(m_strScript.GetLength());

	// ����ͷ
	LPCTSTR p1 = pBuf + nPos;
	while (--p1 >= pBuf)
	{
		if (*p1 == '\n')
		{
			p1++;
			break;
		}
	}  

	// ����β
	LPCTSTR p2 = pBuf + nPos;
	while (*++p2 != '\0')
	{
		if (*p2 == '\n') 
		{
			p2--;
			break;
		}
	} 

	strLine = CString(p1, p2 - p1 + 1);
}

static TCHAR szEventExpert[] = _T("����ר��");

// �����Ƿ���ר�Ҳ���
BOOL CActionParser::IsExpertGenerate(CString& strAction)
{
	int nFind = m_strScript.Find(szEventExpert);
	if (nFind != -1)
	{
		LPCTSTR pch = m_strScript;
		pch += nFind;
		pch += lstrlen(szEventExpert);
		if (*pch++ == _T('('))
		{
			LPCTSTR p1 = pch;																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									
											
			if ((pch = _tcschr(pch, ')')) != NULL)
			{
				strAction = CString(p1, pch - p1);
				strAction.TrimLeft();
				strAction.TrimRight();																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																							
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

// �Ƿ�ʵ�ֺ���p
BOOL CActionParser::HasFunction(LPCTSTR lpszFunc, CStringArray& arrParams)
{
	int nRet = FALSE;

	// ���Դ�Сд
	CString strScipt = m_strScript;
	strScipt.MakeUpper();
	CString strFind = lpszFunc;
	strFind.MakeUpper();

	int nFind = strScipt.Find(strFind);
	if (nFind != -1)
	{
		CString strLine;
		GetPosLine(nFind, strLine);
		strLine.TrimLeft();
		// ��һ���ַ�������δ��������
		if (strFind.CompareNoCase(strLine.Left(strFind.GetLength())) != 0)
			return FALSE;

		LPCTSTR pch = strLine;
		pch += lstrlen(lpszFunc);

		// ���˵��ո�
		while (*pch == _T(' ') || *pch == _T('\t'))
			pch++;
		
		if (*pch++ == _T('('))
		{
			LPCTSTR pPreSeperator = pch;
			BOOL bString = FALSE;
			int nBraceCount = 1;
			while (*pch != _T('\0'))
			{
				if (*pch == _T('"'))
				{
					bString = !bString;
				}
				else if (!bString)
				{
					if (*pch == _T('('))
					{
						nBraceCount++;
					}
					else if (*pch == _T(')'))
					{
						nBraceCount--;
						if (nBraceCount < 1)
						{
							CString strWord(pPreSeperator, pch - pPreSeperator);
							strWord.TrimLeft();
							strWord.TrimRight();
							arrParams.Add(strWord);
							if (!strWord.IsEmpty())
							{
								pch++;
								// ���˵��ո�
								while (*pch == _T(' ') || *pch == _T('\t'))
									pch++;
								// ����ע��
								if (*pch == _T(' ') || *pch == _T('\'') || *pch == _T('\r') || *pch == _T('\t') || *pch == _T('\0'))
									nRet = TRUE;
							}
							break;
						}
					}
					else if (nBraceCount == 1)
					{
						if (*pch == _T(','))
						{
							CString strWord(pPreSeperator, pch - pPreSeperator);
							strWord.TrimLeft();
							strWord.TrimRight();
							arrParams.Add(strWord);
							if (strWord.IsEmpty())
								break;
							pPreSeperator = pch + 1;
						}
					}
				}

				pch++;
			}
		}
	}

	if (!nRet)
		arrParams.RemoveAll();

	return nRet;
}

// �Ƿ��и�ֵ���
BOOL CActionParser::HasAssignment(LPCTSTR lpszLeft, CString& strRight)
{
	int nRet = FALSE;

	// ���Դ�Сд
	CString strScipt = m_strScript;
	strScipt.MakeUpper();
	CString strFind = lpszLeft;
	strFind.MakeUpper();

	int nFind = strScipt.Find(strFind);
	if (nFind != -1)
	{
		CString strLine;
		GetPosLine(nFind, strLine);
		strLine.TrimLeft();
		// ��һ���ַ�������Ϊ��ֵ�������
		if (strFind.CompareNoCase(strLine.Left(strFind.GetLength())) != 0)
			return FALSE;
		
		LPCTSTR pch = strLine;
		pch += lstrlen(lpszLeft);

		// ���˵��ո�
		while (*pch == _T(' ') || *pch == _T('\t'))
			pch++;

		// �ҵ�'='����
		if (*pch++ == _T('='))
		{
			// ���˵��ո�
			while (*pch == _T(' ') || *pch == _T('\t'))
				pch++;
			
			LPCTSTR pPreSeperator = pch;
			BOOL bString = FALSE;
			while (1)
			{
				if (*pch == _T('"'))
				{
					bString = !bString;
				}
				else if (!bString)
				{
					if (*pch == _T(' ') || *pch == _T('\'') || *pch == _T('\r') || *pch == _T('\t') || *pch == _T('\0'))
					{
						CString strWord(pPreSeperator, pch - pPreSeperator);
						strWord.TrimLeft();
						strWord.TrimRight();
						if (!strWord.IsEmpty())
						{
							strRight = strWord;
							nRet = TRUE;
						}
						break;
					}
				}
				pch++;
			}
		}
	}
	
	return nRet;
}