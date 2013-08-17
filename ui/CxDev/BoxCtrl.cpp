// BoxCtrl.cpp

#include "stdafx.h"
#include "DevInc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define IDS_ADDFOLDER           "�����"
#define IDS_REMOVEFOLDER		"ɾ����"
#define IDS_RENAMEFOLDER		"������"
#define IDS_ADDITEM				"���ӿؼ�"
#define IDS_REMOVEITEM			"ɾ����"
#define IDS_DEFAULT_FOLDERNAME	"�½���"

#define	cr3dFace				GetSysColor(COLOR_3DFACE)
#define crBackGroundColor		GetSysColor(COLOR_BTNFACE)
#define	crHilightBorder			GetSysColor(COLOR_3DHILIGHT)
#define	crBackGroundColor1		GetSysColor(COLOR_BACKGROUND)
#define	crDkShadowBorder		GetSysColor(COLOR_3DDKSHADOW)

/////////////////////////////////////////////////////////////////////////////

HBITMAP CreateDitherBitmap()
{
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD      bmiColors[16];
	} bmi;
	memset(&bmi, 0, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 8;
	bmi.bmiHeader.biHeight = 8;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 1;
	bmi.bmiHeader.biCompression = BI_RGB;

	COLORREF clr = ::GetSysColor(COLOR_BTNFACE);
	bmi.bmiColors[0].rgbBlue = GetBValue(clr);
	bmi.bmiColors[0].rgbGreen = GetGValue(clr);
	bmi.bmiColors[0].rgbRed = GetRValue(clr);

	clr = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	bmi.bmiColors[1].rgbBlue = GetBValue(clr);
	bmi.bmiColors[1].rgbGreen = GetGValue(clr);
	bmi.bmiColors[1].rgbRed = GetRValue(clr);

	// initialize the brushes
	long patGray[8];
	for (int i = 0; i < 8; i++)
	   patGray[i] = (i & 1) ? 0xAAAA5555L : 0x5555AAAAL;

	HDC hDC = GetDC(NULL);
	HBITMAP hbm = CreateDIBitmap(hDC, &bmi.bmiHeader, CBM_INIT,
		(LPBYTE)patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);
	ReleaseDC(NULL, hDC);

	return hbm;
}

// CBoxCtrl

IMPLEMENT_DYNCREATE(CBoxCtrl, CWnd)

///////////////////////////////////////////
//CBarItem
////////////////////////////////////////////////
//��  ��:	���캯��
//��  ��:	[in] szItem		���˵������
//			[in] nImage		���ͼ������(ͼ��λ��Folder��)
//			[in] bControl	���Ƿ�Ϊ�ؼ�
////////////////////////////////////////////////
CBoxCtrl::CBarItem::CBarItem(const char* szItem, const int nImage, DWORD dwData, BOOL bControl/*=FALSE*/)
{
	m_nImageIndex	= nImage;
	m_szItem		= NULL;
	m_bControl		= bControl;
	m_dwData		= dwData;

	if (szItem)
	{
		m_szItem = new char[lstrlen(szItem)+1];
		ASSERT(m_szItem);
		lstrcpy(m_szItem, szItem);
	}
}
////////////////////////////////////////////////
//��  ��:	��������
////////////////////////////////////////////////
CBoxCtrl::CBarItem::~CBarItem()
{
	if (m_szItem) 
		delete [] m_szItem;
	if (m_bControl)
		delete (CLSID*)m_dwData;
}

///////////////////////////////////////////
//CBarFolder

////////////////////////////////////////////////
//��  ��:	���캯��
//��  ��:	[in] szName		���������
//			[in] dwData		��������
////////////////////////////////////////////////
CBoxCtrl::CBarFolder::CBarFolder(const char * szName, DWORD dwData)
{
	m_szName		= NULL;
	m_pImageList	= NULL;
	m_dwData		= dwData;

	if (szName)
	{
		m_szName = new char[lstrlen(szName) + 1];
		ASSERT(m_szName);
		lstrcpy(m_szName, szName);
	}
}
////////////////////////////////////////////////
//��  ��:	��������
////////////////////////////////////////////////
CBoxCtrl::CBarFolder::~CBarFolder()
{
	if (m_szName) 
		delete [] m_szName;

	for (int i = 0; i < m_arItems.GetSize(); i++)
	{
		if (m_arItems.GetAt(i)) 
			delete (CBarItem*) m_arItems.GetAt(i);
	}
	m_arItems.RemoveAll();

	if (m_pImageList)
	{
		m_pImageList->DeleteImageList();
		ASSERT(m_pImageList->GetSafeHandle() == NULL);
		delete m_pImageList;
	}
}

//////////////////////////////////////////////////////////////
//��  ��:	ȡ����������ĸ���
//////////////////////////////////////////////////////////////
int CBoxCtrl::CBarFolder::GetItemCount()
{
	return m_arItems.GetSize();
}

//////////////////////////////////////////////////////////////
//��  ��:	�����в���һ����
//��  ��:	
//			[in] iItem		����λ��
//			[in] szText		�������
//			[in] nImage		ͼ������
//			[in] dwData		��������
//			[in] bControl	���Ƿ�Ϊ�ؼ�. TRUE��;FALSE����;ȱ��ΪFALSE
//����ֵ:	�������λ��
//////////////////////////////////////////////////////////////
int CBoxCtrl::CBarFolder::InsertItem(int iItem, const char* szText, const int nImage, const DWORD dwData, BOOL bControl/*=FALSE*/)
{
	if (iItem < 0 || iItem > GetItemCount()) 
		iItem = GetItemCount();

	CBarItem * pBarItem = new CBarItem(szText, nImage, dwData, bControl);
	ASSERT(pBarItem);

	if (iItem < GetItemCount()) 
		m_arItems.InsertAt(iItem, (void*) pBarItem);
	else 
		m_arItems.Add((void*) pBarItem);

	return iItem;
}

///////////////////////////////////////////
//CBoxCtrl
//////////////////////////////////////////////////////////////
//��  ��:	���캯��
//////////////////////////////////////////////////////////////
CBoxCtrl::CBoxCtrl()
{
	m_nFolderHeight = 22;
	m_nSelFolder	= -1;
	m_nSelItem		= 0;
	m_nRSelItem		= -1;
	m_bMouseTracking= FALSE;

	m_nLastFolderHighlighted	= -1;
	m_nLastItemHighlighted		= -1;
}

CBoxCtrl::~CBoxCtrl()
{
	CBarFolder* pBarFolder;
	for (int i = 0; i < m_arFolder.GetSize(); i++)
	{
		int a = 10;
		pBarFolder = (CBarFolder*)m_arFolder.GetAt(i);
		(pBarFolder != NULL) ? delete pBarFolder : NULL;
	}
	m_arFolder.RemoveAll();
}

//////////////////////////////////////////////////////////////
//��  ��:	���һ����
//��  ��:	
//			[in] szFolderName		����ӵ�����
//			[in] dwData				��������
//����ֵ:	������
//////////////////////////////////////////////////////////////
int	CBoxCtrl::AddFolder(const char* szFolderName, const DWORD dwData/*=0*/)
{
	CBarFolder * pBarFolder = new CBarFolder(szFolderName, dwData);
	ASSERT(pBarFolder);

	m_arFolder.Add((void*)pBarFolder);

	return m_arFolder.GetSize() - 1;
	return 0;
}

//////////////////////////////////////////////////////////////
//��  ��:	ɾ�������е�һ����
//��  ��:	
//			[in] nIndex		��ɾ����������
//////////////////////////////////////////////////////////////
void CBoxCtrl::RemoveFolder(const int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < GetFolderCount());
	CBarFolder * pBarFolder = (CBarFolder*)m_arFolder.GetAt(nIndex);
	if (pBarFolder)
	{
		delete pBarFolder;
		m_arFolder.RemoveAt(nIndex);
		//����ѡ�е���
		if (m_nSelFolder >= nIndex) 
			m_nSelFolder -= 1;
		if (m_nSelFolder < 0 && GetFolderCount() > 0) 
			m_nSelFolder = 0;
		Invalidate();
	}
}

//////////////////////////////////////////////////////////////
//��  ��:	���������
//��  ��:	
//			[in] iFolder		������
//			[in] szFolderName	�ַ���
//////////////////////////////////////////////////////////////
void CBoxCtrl::SetFolderName(const int iFolder, const char* szFolderName)
{
	CBarFolder * pBarFolder = (CBarFolder*)m_arFolder.GetAt(iFolder);
	if (pBarFolder)
	{
		if (pBarFolder->m_szName)
			delete [] pBarFolder->m_szName;

		pBarFolder->m_szName = new char[lstrlen(szFolderName) + 1];
		ASSERT(pBarFolder->m_szName);
		lstrcpy(pBarFolder->m_szName, szFolderName);
	}
}
//////////////////////////////////////////////////////////////
//��  ��:	ȡ�����
//��  ��:	
//			[in] iFolder		������
//����ֵ:	������ַ���
//////////////////////////////////////////////////////////////
LPCTSTR CBoxCtrl::GetFolderName(const int iFolder)
{
	LPTSTR lpszName = NULL;
	CBarFolder * pBarFolder = (CBarFolder*)m_arFolder.GetAt(iFolder);
	if (pBarFolder && pBarFolder->m_szName)
	{
		lpszName = pBarFolder->m_szName;
	}
	return (LPCTSTR)lpszName;
}

int CBoxCtrl::GetSelFolder()
{
	return m_nSelFolder;
}

//////////////////////////////////////////////////////////////
//��  ��:	���õ�ǰ��
//��  ��:	
//			[in] iFolder		������
//////////////////////////////////////////////////////////////
void CBoxCtrl::SetSelFolder(const int iFolder)
{
	ASSERT(iFolder >= 0 && iFolder < GetFolderCount());

	if (iFolder != m_nSelFolder)
	{
		CDevTool::c_drawShape = DRAW_SELECTION;
		m_nSelItem = 0;

		AnimateFolderScroll(m_nSelFolder, iFolder);
	
		m_nSelFolder = iFolder;
	}

	Invalidate();
}

//////////////////////////////////////////////////////////////
//��  ��:	ȡ�������
//����ֵ:	�������
//////////////////////////////////////////////////////////////
int CBoxCtrl::GetFolderCount() const
{
	return m_arFolder.GetSize();
}

int	CBoxCtrl::GetSelItem() const
{
	return m_nSelItem;
}

void CBoxCtrl::SetSelItem(int iItem)
{
	m_nSelItem		= iItem;
	Invalidate();
}

//////////////////////////////////////////////////////////////
//��  ��:	���һ����
//��  ��:	
//			[in] iFolder			������
//			[in] iItem				������
//			[in] szText				������
//			[in] nImage				��ͼ��
//			[in] dwData				��������
//����ֵ:	�����������
//////////////////////////////////////////////////////////////
int	CBoxCtrl::InsertItem(const int iFolder, const int iItem, const char* szText, 
						const int nImage/*=-1*/, const DWORD dwData/*=0*/, BOOL bControl/*=FALSE*/)
{
	ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
	CBarFolder* pBarFolder = (CBarFolder*) m_arFolder.GetAt(iFolder);
	return pBarFolder->InsertItem(iItem, szText, nImage, dwData, bControl);
}
//////////////////////////////////////////////////////////////
//��  ��:	ɾ����ǰ���е�һ����
//��  ��:	
//			[in] nIndex		��ɾ����������
//////////////////////////////////////////////////////////////
void CBoxCtrl::RemoveItem(const int nIndex)
{
	ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
	CBarFolder * pBarFolder = (CBarFolder *) m_arFolder.GetAt(m_nSelFolder);
	ASSERT(nIndex >= 0 && nIndex < pBarFolder->GetItemCount());

	if (IsValidItem(nIndex))
	{
		CBarItem * pBarItem = (CBarItem*)pBarFolder->m_arItems.GetAt(nIndex);
		if (pBarItem)
			delete pBarItem;
		pBarFolder->m_arItems.RemoveAt(nIndex);

		//��������
		CRect rcFolderClent;
		GetFolderClientRect(rcFolderClent);
		InvalidateRect(rcFolderClent);
	}
}
//////////////////////////////////////////////////////////////
//��  ��:	�����в���һ����,����Ϊ�ؼ�
//��  ��:	
//			[in] iFolder	�������������
//			[in] clsid		�ؼ���CLSID
//����ֵ:	TRUE ����ɹ�; FALSE ����ʧ��
//////////////////////////////////////////////////////////////
BOOL CBoxCtrl::InsertControl(int iFolder, CLSID clsid, LPCTSTR szText)
{
	CImageList* pImageList = GetFolderImageList(iFolder);
	ASSERT(pImageList != NULL);
	
	CBarFolder* pBarFolder = (CBarFolder*) m_arFolder.GetAt(iFolder);
	ASSERT(pBarFolder != NULL);

	USES_CONVERSION;
	HKEY    hKeyCLSID;

	if (RegOpenKey(HKEY_CLASSES_ROOT, _T("CLSID"), &hKeyCLSID) != ERROR_SUCCESS)
	{
		TRACE0("Can't open CLSID key!\n");
		return FALSE;
	}

	OLECHAR szClassID[39];
	int cchGuid = ::StringFromGUID2(clsid, szClassID, 39);
	LPCTSTR lpszClassID = OLE2CT(szClassID);
	ASSERT(cchGuid == 39);
	if (cchGuid != 39)
	{
		TRACE0("Error when StringFromGUID2!\n");
		return FALSE;
	}

	HKEY    hKeyClass; 
	if (RegOpenKey(hKeyCLSID, lpszClassID, &hKeyClass) != ERROR_SUCCESS)
	{
		TRACE0("Can't open ClassID key!\n");
		return FALSE;
	}
	TCHAR szBuf[_MAX_PATH];
	long cb = sizeof(szBuf) ;
	CBitmap bmp;
	//ע������Ƿ�Ǽ�ͼ����Ϣ?
	if (RegQueryValue(hKeyClass, (LPTSTR)"ToolboxBitmap32", szBuf, &cb ) == ERROR_SUCCESS ||
		RegQueryValue(hKeyClass, (LPTSTR)"ToolboxBitmap", szBuf, &cb ) == ERROR_SUCCESS)
	{
		TCHAR* pszBitmapModule	= _tcstok(szBuf, _T(","));
		TCHAR* pszBitmapId		= _tcstok(NULL, _T(","));	

		HINSTANCE hinstBitmap;
		int nBitmapId;
		if ((pszBitmapModule != NULL) && (pszBitmapId != NULL))
		{
			LPTSTR p ;
			if (*pszBitmapModule == '"')
			{
				lstrcpy( pszBitmapModule, pszBitmapModule+1 ) ;
				p = strrchr( pszBitmapModule, '"' ) ;
				if (p) *p = '\0' ;
			}
			nBitmapId = _ttoi(pszBitmapId);

			OSVERSIONINFO OSVer;
			OSVer.dwOSVersionInfoSize = sizeof(OSVer);
			::GetVersionEx(&OSVer);

			if (OSVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				hinstBitmap = LoadLibrary(pszBitmapModule);
			else
				hinstBitmap = LoadLibraryEx(pszBitmapModule, NULL, LOAD_LIBRARY_AS_DATAFILE);
			if (hinstBitmap && (nBitmapId > 0))
			{
				HBITMAP hbmp = ::LoadBitmap(hinstBitmap, MAKEINTRESOURCE(nBitmapId));
				//HBITMAP hbmp = (HBITMAP)::LoadImage(hinstBitmap, MAKEINTRESOURCE(nBitmapId),
				//							IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
				if (hbmp != NULL)
				{
					TRACE0("Can't Load Bitmap From Resource Instance!\n");
					VERIFY(bmp.Attach(hbmp));
				}
	
				FreeLibrary( hinstBitmap ) ;
			}
		}
	}

	if (bmp.m_hObject == NULL)
		bmp.LoadBitmap(IDB_CURSOR);		//ע�����û�еǼ�ͼ��ʱװ��ȱʡ��ͼ��

	RegCloseKey(hKeyClass);
	RegCloseKey(hKeyCLSID);

	int nIndex = pImageList->Add(&bmp, RGB(192,192,192));
	ASSERT(nIndex >= 0);

	CLSID* pCLSID =  new CLSID;
	*pCLSID = clsid;

	CString strText;
	if (szText != NULL)
	{
		strText = szText;
	}
	else
	{
		if (!GetDefalutControlDesc(clsid, strText))
		{
			CComPtr<IDispatch> spDispatch;
			HRESULT hr = spDispatch.CoCreateInstance(clsid);
			if (SUCCEEDED(hr))
			{
				CComPtr<IProvideClassInfo> spProvideClassInfo;
				CComPtr<ITypeInfo> spClassInfo;

				// ��ȡ����Ϣ
				HRESULT hr = spDispatch->QueryInterface(&spProvideClassInfo);
				if (SUCCEEDED(hr))
				{
					hr = spProvideClassInfo->GetClassInfo(&spClassInfo);
					if (SUCCEEDED(hr))
					{
						CComBSTR bstrName;
						spClassInfo->GetDocumentation(MEMBERID_NIL, &bstrName, NULL, NULL, NULL);
						strText = bstrName;
					}
				}
			}

//			LPOLESTR lpsz;
//			LPTSTR strName = NULL;
//			ProgIDFromCLSID(clsid, &lpsz);
//			strText = lpsz;
//			CoTaskMemFree(lpsz);
		}
	}
	
	InsertItem(iFolder, pBarFolder->m_arItems.GetSize(), strText, nIndex, (DWORD)pCLSID, TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////
//��  ��:	���������ڵĿ��
//��  ��:	
//			[in]  nOrigWidth		�����ڵĳ�ʼ���
//			[out] nSetWidth			������ĸ����ڿ��
//˵  ��:	Ϊ��ʹ���е���ͼ����ˮƽ�����ϸպ�����,�����ְ��
//			ͼ��Ŀ���,�˺������ݸ����ڴ�����ԭʼ��ȼ��㸸��
//			�ڵ�����Ӧ�����õĿ��
//////////////////////////////////////////////////////////////
void CBoxCtrl::CalcParentWndWidth(int nOrigWidth, int& nSetWidth)
{
	CImageList* pImageList = GetFolderImageList(m_nSelFolder);
	ASSERT(pImageList);
	if (pImageList)
	{
		CSize szImage(0,0);
		IMAGEINFO ImageInfo;
		pImageList->GetImageInfo(0, &ImageInfo);
		szImage = CRect(ImageInfo.rcImage).Size();
		int nImageWidth = szImage.cx + 12;

		int nBord = GetSystemMetrics(SM_CXBORDER);
		int nSizeFrame = GetSystemMetrics(SM_CXSIZEFRAME);
		
		int nBound = 2 * (nBord + nSizeFrame) + 2;	//�����ߺ���߿ͻ����ı�

		if (nOrigWidth < nImageWidth + nBound)
		{	//���ԭ���Ŀ�ȱ�һ����ݲ���,��ǿ����Ϊһ����Ŀ��
			nSetWidth = nImageWidth + nBound;
			return;
		}

		int nResidue = (nOrigWidth - nBound) % nImageWidth;

		if (nResidue != 0)
		{
			if (nResidue >= nImageWidth / 2)
				nSetWidth = nOrigWidth + (nImageWidth - nResidue);	//���������ӵ���һ��ͼ����
			else
				nSetWidth = nOrigWidth - nResidue;					//δ��������ٵ�ǰһ��ͼ����
			return;
		}
	}
	nSetWidth = nOrigWidth;
}

//////////////////////////////////////////////////////////////
//��  ��:	ȡ�õ�ǰ��Ŀͻ����ڴ��ڿͻ�����λ��
//��  ��:	
//			[out] rect	���յ�ǰ��Ŀͻ���(��������ı�������)
//////////////////////////////////////////////////////////////
void CBoxCtrl::GetFolderClientRect(CRect& rect) const
{
	GetClientRect(rect);
	if (m_arFolder.GetSize() > 0)
	{
		int nFolderCount = m_arFolder.GetSize();
		rect.top	+= m_nFolderHeight * (m_nSelFolder + 1) - 1;
		rect.bottom -= (nFolderCount - m_nSelFolder - 1) * m_nFolderHeight;
	}
}
///////////////////////////////////////////////
//��  ��:	ȡ��ָ����������ڴ��ڿͻ�����λ��
//��  ��:	[in]  iFolder		������
//			[out] rect			����������λ��
///////////////////////////////////////////////
BOOL CBoxCtrl::GetFolderRect(const int iFolder, CRect & rect) const
{
	int nFolderCount = m_arFolder.GetSize();
	ASSERT(iFolder >= 0 && iFolder < nFolderCount);

	if (iFolder >= 0 && iFolder < nFolderCount)
	{
		CRect rcClient;
		GetClientRect(rcClient);
		if (iFolder > m_nSelFolder)
			rect.SetRect(rcClient.left, 
						rcClient.bottom - ((nFolderCount - iFolder)) * m_nFolderHeight, 
						rcClient.right, 
						rcClient.bottom - (nFolderCount - iFolder - 1) * m_nFolderHeight);
		else 
			rect.SetRect(rcClient.left, 
						rcClient.top + iFolder * m_nFolderHeight - 1, 
						rcClient.right, 
						rcClient.top + (1 + iFolder) * m_nFolderHeight - 1);
		return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////
//��  ��:	ȡ�����е����ڴ��ڿͻ�����λ��
//��  ��:	
//			[in]  iFolder	������
//			[in]  iItem		������	
//			[out] rect		�������
///////////////////////////////////////////////
void CBoxCtrl::GetItemRect(const int iFolder, const int iItem, CRect& rect) const
{
	ASSERT(iItem >= 0);

	CRect rcOpenFolder;
	GetFolderClientRect(rcOpenFolder);

	CSize szItem(0, 0);

	int nLineHeight;
	CPoint ptItem;
	//��ʼ�и�Ϊ��
	nLineHeight = 0;
	ptItem = rcOpenFolder.TopLeft();

	//����������Ҫ����
	ptItem.y += 3;
	ptItem.x += 2;

	for (int i = 0; i <= iItem; i++)
	{
		szItem = GetItemSize(iFolder, i);

		if (ptItem.x + szItem.cx > rcOpenFolder.right)
		{	//����
			ptItem.x = rcOpenFolder.left + 2;
			ptItem.y += nLineHeight; 
			nLineHeight = szItem.cy;
		}
		else
		{	//����
			/*if (ptItem.x + szItem.cx == rcOpenFolder.right)
			{
				CRect rcWindow;
				GetWindowRect(rcWindow);
			}*/
			if (szItem.cy > nLineHeight)
				nLineHeight = szItem.cy;
		}
		if (i == iItem)
		{
			rect.SetRect(ptItem.x, ptItem.y, ptItem.x + szItem.cx, ptItem.y + szItem.cy);
			break;
		}
		//������һ����Ŀ�ʼ��
		ptItem.x += szItem.cx;
	}
}
///////////////////////////////////////////////
//��  ��:	ȡ�����е����ڴ��ڿͻ����Ĵ�С
//��  ��:	
//			[in]  iFolder	������
//			[in]  iItem		������	
//����ֵ:	CSize	��Ĵ�С
///////////////////////////////////////////////
CSize CBoxCtrl::GetItemSize(const int iFolder, const int iItem) const
{
	ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
	CBarFolder* pBarFolder = (CBarFolder*) m_arFolder.GetAt(iFolder);

	ASSERT(iItem >= 0 && iItem < pBarFolder->GetItemCount());
	CBarItem* pBarItem = (CBarItem*)pBarFolder->m_arItems.GetAt(iItem);

	CSize szImage(0,0);

	if (pBarItem->m_nImageIndex >= 0)
	{
		CImageList * pImageList = GetFolderImageList(iFolder);
		ASSERT(pImageList);
		if (pImageList)
		{
			IMAGEINFO ImageInfo;
			pImageList->GetImageInfo(pBarItem->m_nImageIndex, &ImageInfo);
			szImage = CRect(ImageInfo.rcImage).Size();
		}
	}
	//��ͼ����չ���̶��Ŀ�Ⱥ͸߶�
	szImage.cx += 12;
	szImage.cy += 13;

	return szImage;
}

///////////////////////////////////////////////
//��  ��:	��Ӧ�û��ı�ѡ����ĺ���
//��  ��:	
//			[in]  iItem	�ı���������
//˵  ��:	����ı������������ǵ�һ����(ָ��),
//			��Ҫ�������ڷ�����Ϣ,֪ͨ�û��Ѿ�ѡ��
//			��һ����ͬ����.
///////////////////////////////////////////////

void CBoxCtrl::OnChangeSelItem(int iItem)
{
	if (/*iItem > 0 && */iItem != m_nSelItem)
	{
		CBarFolder * pBarFolder = (CBarFolder *)m_arFolder.GetAt(m_nSelFolder);
		ASSERT(pBarFolder != NULL);
		CBarItem* pBarItem = (CBarItem*)pBarFolder->m_arItems.GetAt(iItem);
		ASSERT(pBarItem != NULL);

		CWnd* pWnd = AfxGetMainWnd();
		HighlightItem(iItem, TRUE);
		ASSERT(pWnd != NULL);
		if (pBarItem->m_bControl)
			pWnd->SendMessage(WM_INSERTCONTROL, pBarItem->m_dwData, 0);	//control
		else	
			pWnd->SendMessage(WM_COMMAND, pBarItem->m_dwData, 0);		//tools
			
	}
}
///////////////////////////////////////////////
//��  ��:	ȡ���ͼ���б�
//��  ��:	
//			[in]  iFolder	������
///////////////////////////////////////////////
CImageList* CBoxCtrl::GetFolderImageList(const int iFolder) const
{
	ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
	CBarFolder * pBarFolder = (CBarFolder *)m_arFolder.GetAt(iFolder);
	if (pBarFolder->m_pImageList != NULL)
		return pBarFolder->m_pImageList;
	else
		return NULL;
}
///////////////////////////////////////////////
//��  ��:	�������ͼ���б�
//��  ��:	
//			[in]  iFolder		������
//			[in]  pImageList	ָ��
///////////////////////////////////////////////
void CBoxCtrl::SetFolderImageList(const int iFolder, CImageList* pImageList) const
{
	ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
	CBarFolder* pBarFolder = (CBarFolder *)m_arFolder.GetAt(iFolder);
	//ֻ���ڲ�Ϊ�յ������,���������Դ©��
	if (pBarFolder->m_pImageList == NULL)
		pBarFolder->m_pImageList = pImageList;
}

//////////////////////////////////////////////////////////////
//��  ��:	�жϸ�������������Ƿ���Ч
//��  ��:	
//			[in] nIndex		������
//����ֵ:	TRUE  ��Ч	FALSE ��Ч
//////////////////////////////////////////////////////////////
BOOL CBoxCtrl::IsValidItem(const int nIndex) const
{
	ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
	CBarFolder* pBarFolder = (CBarFolder*)m_arFolder.GetAt(m_nSelFolder);
	return (nIndex >= 0 && nIndex < pBarFolder->GetItemCount());
}

//////////////////////////////////////////////////////////////
//��  ��:	���������
//��  ��:	
//			[in] pDC		CDCָ��	
//			[in] iFolder	������	
//			[in] rect		���������
//			[in] bSelected	������Ƿ񱻵���
//////////////////////////////////////////////////////////////
void CBoxCtrl::DrawFolder(CDC* pDC, const int iFolder, CRect rect, const BOOL bSelected)
{
	CBarFolder * pBarFolder = (CBarFolder*) m_arFolder.GetAt(iFolder);

	CPen pen(PS_SOLID, 1, crBackGroundColor1);
	CPen* pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(rect.left, rect.top);
	if (!bSelected)
		pDC->LineTo(rect.right, rect.top);
	else
		pDC->MoveTo(rect.left+1, rect.top);
	pDC->SelectObject(pOldPen);

	rect.top++;

	//�Ȼ���
	if (!bSelected)
	{
		pDC->Draw3dRect(rect, crHilightBorder, crBackGroundColor1);
		rect.InflateRect(-1, -1);
	}
	else
	{
		pDC->Draw3dRect(rect, crDkShadowBorder, crHilightBorder);
		rect.InflateRect(-1, -1);
		pDC->Draw3dRect(rect, crBackGroundColor1, cr3dFace);
		rect.InflateRect(-1, -1);
	}
	//�������
	pDC->FillSolidRect(rect, cr3dFace);
	int nMode = pDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
	//�������
	pDC->DrawText(CString(pBarFolder->m_szName), rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SetBkMode(nMode);
	pDC->SelectObject(pOldFont);
}

//////////////////////////////////////////////////////////////////////
//��  ��:	�������е�������
//��  ��:	
//			[in] pDC	CDCָ��	
//			[in] iFolder������	
//			[in] rc		��������
//////////////////////////////////////////////////////////////////////
void CBoxCtrl::PaintItems(CDC * pDC, const int iFolder, CRect rc)
{
	ASSERT(iFolder >= 0 && iFolder < GetFolderCount());

	CRect rcFolderClient;
	int i, nItemCount;

	CBarFolder* pBarFolder = (CBarFolder*) m_arFolder.GetAt(iFolder);
	nItemCount = pBarFolder->GetItemCount();
	
	GetFolderClientRect(rcFolderClient);

	int nSaveDC = pDC->SaveDC();
	ASSERT(nSaveDC != 0);

	//���ü�������
	CRgn itemRegion;
	itemRegion.CreateRectRgnIndirect(&rcFolderClient);
	pDC->SelectClipRgn(&itemRegion);
	itemRegion.DeleteObject();

	for (i = 0; i < nItemCount; i++)
	{
		CRect rcItem;
		//���������
		GetItemRect(iFolder, i, rcItem);
		if (rcItem.top > rcFolderClient.bottom) 
			break;
		else 
			DrawItem(pDC, iFolder, rcItem, i);
	}
	pDC->RestoreDC(nSaveDC);
}

//////////////////////////////////////////////////////////////////////
//��  ��:	�������е�ÿ����
//��  ��:	
//			[in] pDC		CDCָ��	
//			[in] iFolder	������	
//			[in] rcItem		�������
//			[in] iItem		������
//////////////////////////////////////////////////////////////////////

void CBoxCtrl::DrawItem(CDC * pDC, const int iFolder, CRect rcItem, const int iItem)
{
	CImageList * pImageList = GetFolderImageList(iFolder);

	ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
	CBarFolder * pBarFolder = (CBarFolder*)m_arFolder.GetAt(iFolder);

	ASSERT(iItem >= 0 && iItem < pBarFolder->GetItemCount());
	CBarItem * pBarItem = (CBarItem*) pBarFolder->m_arItems.GetAt(iItem);

	ASSERT(pBarItem && pImageList);

	if (pImageList)
	{
		IMAGEINFO imageinfo;
		pImageList->GetImageInfo(pBarItem->m_nImageIndex, &imageinfo);
		CSize szImage = CRect(imageinfo.rcImage).Size();

		if (iItem == m_nSelItem)
		{
		//	pDC->Draw3dRect(rcItem, RGB(128,128,128), RGB(255, 255, 255));
			pDC->Draw3dRect(rcItem, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));

			rcItem.DeflateRect(1, 1);
			pDC->FillRect(rcItem, &m_brushDither);
		}
		rcItem.InflateRect(1,1);

		CPoint pt;
		pt.x = rcItem.left + (rcItem.Width() - szImage.cx) / 2;
		pt.y = rcItem.top + (rcItem.Height() - szImage.cy) / 2;
		pImageList->Draw(pDC, pBarItem->m_nImageIndex, pt, ILD_TRANSPARENT);
	}
}
////////////////////////////////////////////////////////////////
//��  ��:	�������λ�û��д��ڵĺδ�
//��  ��:	
//			[in] point		��ǰ����
//			[out] nIndex	���е�Ԫ������(�������Ԫ��)
//����ֵ:	
//			HT_Nothing		û�н������κ�Ԫ��
//			HT_Folder		���������
//			HT_Item			������
//			HT_FolderClient	������ͻ���(������������)
////////////////////////////////////////////////////////////////
int CBoxCtrl::HitTestEx(const CPoint & point, int& nIndex) const
{
	int nFolderCount = m_arFolder.GetSize();
	if (nFolderCount == 0)
		return HT_Nothing;

	int i;
	CRect rc;
	for (i = 0; i < nFolderCount; i++)
	{
		GetFolderRect(i, rc);
		if (rc.PtInRect(point)) 
		{
			nIndex = i;
			return HT_Folder;
		}
	}

	CBarFolder * pBarFolder = (CBarFolder*)m_arFolder.GetAt(m_nSelFolder);
	int nItemCount;

	GetFolderClientRect(rc);
	nItemCount = pBarFolder->GetItemCount();
	for (i = 0; i < nItemCount; i++)
	{
		CBarItem * pBarItem = (CBarItem*) pBarFolder->m_arItems.GetAt(i);
		CRect itemRc;
		
		GetItemRect(m_nSelFolder, i, itemRc);
		if (itemRc.PtInRect(point))
		{
			nIndex = i;
			return HT_Item;
		}
		else if (itemRc.top > rc.bottom) 
			break;
	}
	
	if (rc.PtInRect(point))
	{	//������ͻ���
		return HT_FolderClient;
	}
	return HT_Nothing;
}
////////////////////////////////////////////////////////////////
//��  ��:	���������
//��  ��:	
//			[in] iFolder	������
////////////////////////////////////////////////////////////////
void CBoxCtrl::HighlightFolder(const int iFolder)
{
	CWnd * pFousWnd = GetFocus();
	if (pFousWnd != NULL && pFousWnd != this && IsChild(pFousWnd)) 
		return;

	if (m_nLastFolderHighlighted == iFolder) 
		return;

	if (m_nLastFolderHighlighted >= 0 && m_nLastFolderHighlighted < GetFolderCount())
	{
		CRect rc;
		rc.SetRectEmpty();
		if (GetFolderRect(m_nLastFolderHighlighted, rc))
		{
			CClientDC dc(this);
			CPen pen(PS_SOLID, 1, crBackGroundColor1);
			CPen * pOldPen = dc.SelectObject(&pen);
			ASSERT(pOldPen != NULL);
			dc.MoveTo(rc.left,		rc.bottom - 1);
			dc.LineTo(rc.right - 1,	rc.bottom - 1);
			dc.LineTo(rc.right - 1,	rc.top);

			CPen pen1(PS_SOLID, 1, cr3dFace);
			dc.SelectObject(&pen1);
			dc.MoveTo(rc.left + 1,	rc.bottom - 2);
			dc.LineTo(rc.right - 2,	rc.bottom - 2);
			dc.LineTo(rc.right - 2,	rc.top + 1);
			dc.SelectObject(pOldPen);
		}
	}
	m_nLastFolderHighlighted = iFolder;
	if (m_nLastFolderHighlighted >= 0)
	{
		CRect rc;
		rc.SetRectEmpty();
		if (GetFolderRect(m_nLastFolderHighlighted, rc))
		{
			CClientDC dc(this);
			CPen pen(PS_SOLID, 1, crDkShadowBorder);
			CPen * pOldPen = dc.SelectObject(&pen);
			ASSERT(pOldPen != NULL);
			dc.MoveTo(rc.left,		rc.bottom - 1);
			dc.LineTo(rc.right - 1,	rc.bottom - 1);
			dc.LineTo(rc.right - 1,	rc.top);

			CPen pen1(PS_SOLID, 1, crBackGroundColor1);
			dc.SelectObject(&pen1);
			dc.MoveTo(rc.left + 1,	rc.bottom - 2);
			dc.LineTo(rc.right - 2, rc.bottom - 2);
			dc.LineTo(rc.right - 2, rc.top + 1);
			dc.SelectObject(pOldPen);
		}
	}
}
////////////////////////////////////////////////////////////////
//��  ��:	������
//��  ��:	
//			[in] iItem		������
//			[in] bPressed	���Ƿ񱻰���
////////////////////////////////////////////////////////////////
void CBoxCtrl::HighlightItem(const int iItem, const BOOL bPressed)
{
	CRect irc;
	CClientDC dc(this);
	GetFolderClientRect(irc);
	int isdc = dc.SaveDC();
	CRgn itemRegion;
	itemRegion.CreateRectRgnIndirect(&irc);
	dc.SelectClipRgn(&itemRegion);
	itemRegion.DeleteObject();

	CRect rcItem;
	rcItem.SetRectEmpty();

	//����״̬����Ϣ
	CBarFolder * pBarFolder = NULL;
	CBarItem * pBarItem = NULL;
	if (iItem >= 0)
	{
		pBarFolder = (CBarFolder*)m_arFolder.GetAt(m_nSelFolder);
		ASSERT(pBarFolder != NULL);
		pBarItem = (CBarItem*) pBarFolder->m_arItems.GetAt(iItem);
		ASSERT(pBarItem != NULL);
		CString strTmp;
		if(pBarItem->m_bControl)
		{
			strTmp = pBarItem->m_szItem;
		}
		else
		{
			strTmp.LoadString(pBarItem->m_dwData);
			int nNext = strTmp.Find("\n", 0);
			strTmp = strTmp.Left(nNext);
		}
		((CMainFrame*)(theApp.m_pMainWnd))->m_wndStatusBar.SetPaneText(0, strTmp);
	}

	if (bPressed && iItem != m_nSelItem)
	{	//����
		CImageList * pImageList = GetFolderImageList(m_nSelFolder);
		ASSERT(pImageList != NULL);
		IMAGEINFO imageinfo;
		pImageList->GetImageInfo(pBarItem->m_nImageIndex, &imageinfo);
		CSize szImage = CRect(imageinfo.rcImage).Size();
		
		//����ǰ���µ���

		GetItemRect(m_nSelFolder, iItem, rcItem);
		//�Ȼ����
		dc.Draw3dRect(rcItem, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
		rcItem.DeflateRect(1, 1);
		//�ٻ�����
		dc.FillRect(rcItem, &m_brushDither);
		rcItem.InflateRect(1,1);

		CPoint pt;
		pt.x = rcItem.left + (rcItem.Width() - szImage.cx) / 2;
		pt.y = rcItem.top + (rcItem.Height() - szImage.cy) / 2;
		//���ͼ��
		pImageList->Draw(&dc, pBarItem->m_nImageIndex, pt, ILD_TRANSPARENT);

		pBarItem = (CBarItem*) pBarFolder->m_arItems.GetAt(m_nSelItem);
		ASSERT(pBarItem != NULL);

		pImageList->GetImageInfo(pBarItem->m_nImageIndex, &imageinfo);
		szImage = CRect(imageinfo.rcImage).Size();

		//�ٰ�ǰһ�����µ���
		GetItemRect(m_nSelFolder, m_nSelItem, rcItem);
		dc.Draw3dRect(rcItem, crBackGroundColor, crBackGroundColor);

		rcItem.DeflateRect(1, 1);
		//�ٻ�����
		dc.FillSolidRect(rcItem, crBackGroundColor);
		rcItem.InflateRect(1,1);
		pt.x = rcItem.left + (rcItem.Width() - szImage.cx) / 2;
		pt.y = rcItem.top + (rcItem.Height() - szImage.cy) / 2;
		//���ͼ��
		pImageList->Draw(&dc, pBarItem->m_nImageIndex, pt, ILD_TRANSPARENT);

		m_nSelItem = iItem;
	}
	else if (iItem != m_nLastItemHighlighted)
	{	//û��,�϶��Ǹ���
		if (iItem != m_nSelItem && iItem >= 0)
		{
			GetItemRect(m_nSelFolder, iItem, rcItem);
			dc.Draw3dRect(rcItem, RGB(255,255,255), RGB(128,128,128));
		}

		if (IsValidItem(m_nLastItemHighlighted) && m_nLastItemHighlighted != m_nSelItem)
		{
			GetItemRect(m_nSelFolder, m_nLastItemHighlighted, rcItem);
			dc.Draw3dRect(rcItem, crBackGroundColor, crBackGroundColor);
		}
		m_nLastItemHighlighted = iItem;
	}

	dc.RestoreDC(isdc);
}
////////////////////////////////////////////////////////////////
//��  ��:	��һ�����������һ����
//��  ��:	
//			[in] iFrom		������
//			[in] iTo		������
////////////////////////////////////////////////////////////////
void CBoxCtrl::AnimateFolderScroll(const int iFrom, const int iTo)
{
	ASSERT(iFrom >= 0 && iFrom < GetFolderCount());
	ASSERT(iTo >= 0 && iTo < GetFolderCount());

	CRect rcFolderClient, rc1, rcFolder, frc1;
	GetFolderClientRect(rcFolderClient);

	rc1.SetRect(0, 0, rcFolderClient.Width(), rcFolderClient.Height());
	GetFolderRect(iTo, rcFolder);
	frc1 = rcFolder;

	CClientDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmpFrom, bmpTo;
	bmpFrom.CreateCompatibleBitmap(&dc, rc1.Width(), rc1.Height() + rcFolder.Height() * 2);
	bmpTo.CreateCompatibleBitmap(&dc, rc1.Width(), rc1.Height() + rcFolder.Height() * 2);
	CDC * pDC = &memDC;

	CBitmap * obmp = pDC->SelectObject(&bmpFrom);
	if (iTo > iFrom)
	{	//�������Ϲ�
		rc1.bottom += rcFolder.Height() * 2;
		pDC->FillSolidRect(rc1, crBackGroundColor);

		PaintItems(pDC, iFrom, rc1);

		pDC->SelectObject(&bmpTo);
		rcFolder.SetRect(0, 0, rcFolder.Width(), rcFolder.Height());
		rc1.SetRect(0, rcFolder.Height(), rcFolderClient.Width(), 
			rcFolderClient.Height() + rcFolder.Height() * 2);
		pDC->FillSolidRect(rc1, crBackGroundColor);

		PaintItems(pDC, iTo, rc1);
		DrawFolder(pDC, iTo, rcFolder, FALSE);
	}
	else
	{	//�������¹�
		rcFolder.SetRect(0, 0, rcFolder.Width(), rcFolder.Height());
		rc1.top += rcFolder.Height();
		rc1.bottom += rcFolder.Height() * 2;
		pDC->FillSolidRect(rc1, crBackGroundColor);
		PaintItems(pDC, iFrom, rc1);

		DrawFolder(pDC, iFrom, rcFolder, FALSE);

		pDC->SelectObject(&bmpTo);
		rc1.SetRect(0,0,rcFolderClient.Width(), rcFolderClient.Height() + rcFolder.Height() * 2);
		pDC->FillSolidRect(rc1, crBackGroundColor);

		PaintItems(pDC, iTo, rc1);
	}

	if (iTo > iFrom)
	{	//�������Ϲ�
		CRect rcFrom, rcTo;
		GetFolderRect(iFrom, rcFrom);
		GetFolderRect(iTo, rcTo);
		int fh = rcFrom.Height();
		for (int y = rcTo.top - fh; y > rcFrom.bottom; y -= fh)
		{
			pDC->SelectObject(&bmpFrom);
			dc.BitBlt(rcFolderClient.left, rcFrom.bottom + 1, rcFolderClient.Width(), y - rcFrom.bottom - 1, pDC, 0,fh, SRCCOPY);
			pDC->SelectObject(&bmpTo);
			dc.BitBlt(rcFolderClient.left, y, rcFolderClient.Width(), rcFolderClient.bottom - y + fh, pDC, 0,0, SRCCOPY);
			Sleep(10);
		}
	}
	else
	{	//�������¹�
		CRect rcFrom, rcTo;
		GetFolderRect(iFrom, rcFrom);
		int fh = rcFrom.Height();
		rcTo.SetRect(rcFolderClient.left, rcFolderClient.bottom, rcFolderClient.right, rcFolderClient.bottom - fh);
		for (int y = rcFrom.top + 1; y < rcTo.top - fh; y += fh)
		{
			pDC->SelectObject(&bmpTo);
			dc.BitBlt(rcFolderClient.left, rcFrom.top + 1, rcFolderClient.Width(), y - rcFrom.top - 1, pDC, 0, fh*2, SRCCOPY);
			pDC->SelectObject(&bmpFrom);
			dc.BitBlt(rcFolderClient.left, y, rcFolderClient.Width(), rcFolderClient.bottom - y, pDC, 0,0, SRCCOPY);
		
			Sleep(10);
		}
	}

	pDC->SelectObject(obmp);
}

static UINT BASED_CODE toolbox[] =
{
	ID_DRAW_SELECT,
	ID_DRAW_LINE,
	ID_DRAW_RECT,
	ID_DRAW_ROUNDRECT,
	ID_DRAW_ELLIPSE,
	ID_DRAW_POLYLINE,
	ID_DRAW_PIPE,
	ID_DRAW_POLYGON,
	ID_DRAW_ARC,
	ID_DRAW_PIE,
	ID_DRAW_CHORD,
	ID_DRAW_TEXT,
	ID_DRAW_DATALINK,
	ID_DRAW_VARIABLE,
	ID_DRAW_BUTTON,
	ID_DRAW_MENU,
	ID_DRAW_TIMER,
	ID_DRAW_CHART,
	ID_DRAW_ALMSUM,
	ID_INSERT_BITMAP,
};

////////////////////////////////////////////////////////////////
//��  ��:	��ʼ�������е�����
//˵  ��:	��һ�����е������ǳ����ڶ�,����������ݴ�ע�����װ��
////////////////////////////////////////////////////////////////

void CBoxCtrl::InitWindow()
{
	CImageList* pImageList1 = new CImageList();
	VERIFY(pImageList1->Create(IDB_TOOLBOX, 16, 0, RGB(192,192,192)));

	AddFolder("����");
	SetFolderImageList(0, pImageList1);

	int nItemCount = sizeof(toolbox)/sizeof(UINT);
	for (int i = 0; i < nItemCount; i++)
	{
		InsertItem(0, i, NULL, i, toolbox[i]);
	}

	LoadWindow();

	m_nSelFolder	= 0;
	m_nSelItem		= 0;
}

const CLSID CLSID_Label = {0x978C9E23,0xD4B0,0x11CE,{0xBF,0x2D,0x00,0xAA,0x00,0x3F,0x40,0xD0}};
const CLSID CLSID_TextBox = {0x8BD21D10,0xEC42,0x11CE,{0x9E,0x0D,0x00,0xAA,0x00,0x60,0x02,0xF3}};
const CLSID CLSID_ComboBox = {0x8BD21D30,0xEC42,0x11CE,{0x9E,0x0D,0x00,0xAA,0x00,0x60,0x02,0xF3}};
const CLSID CLSID_ListBox = {0x8BD21D20,0xEC42,0x11CE,{0x9E,0x0D,0x00,0xAA,0x00,0x60,0x02,0xF3}};
const CLSID CLSID_CheckBox = {0x8BD21D40,0xEC42,0x11CE,{0x9E,0x0D,0x00,0xAA,0x00,0x60,0x02,0xF3}};
const CLSID CLSID_OptionButton = {0x8BD21D50,0xEC42,0x11CE,{0x9E,0x0D,0x00,0xAA,0x00,0x60,0x02,0xF3}};
const CLSID CLSID_ToggleButton = {0x8BD21D60,0xEC42,0x11CE,{0x9E,0x0D,0x00,0xAA,0x00,0x60,0x02,0xF3}};
const CLSID CLSID_CommandButton = {0xD7053240,0xCE69,0x11CD,{0xA7,0x77,0x00,0xDD,0x01,0x14,0x3C,0x57}};
const CLSID CLSID_ScrollBar = {0xDFD181E0,0x5E2F,0x11CE,{0xA4,0x49,0x00,0xAA,0x00,0x4A,0x80,0x3D}};
const CLSID CLSID_SpinButton = {0x79176FB0,0xB7F2,0x11CE,{0x97,0xEF,0x00,0xAA,0x00,0x6D,0x27,0x76}};

const CLSID CLSID_ProgressBar = {0x35053A22,0x8589,0x11D1,{0xB1,0x6A,0x00,0xC0,0xF0,0x28,0x36,0x28}};
const CLSID CLSID_Slider = {0xF08DF954,0x8592,0x11D1,{0xB1,0x6A,0x00,0xC0,0xF0,0x28,0x36,0x28}};
const CLSID CLSID_DTPicker = {0x20DD1B9E,0x87C4,0x11D1,{0x8B,0xE3,0x00,0x00,0xF8,0x75,0x4D,0xA1}};
const CLSID CLSID_MSFlexGrid = {0x6262D3A0,0x531B,0x11CF,{0x91,0xF6,0xC2,0x86,0x3C,0x38,0x5E,0x30}};
const CLSID CLSID_Adodc = {0x67397AA3,0x7FB1,0x11D0,{0xB1,0x48,0x00,0xA0,0xC9,0x22,0xE8,0x20}};
const CLSID CLSID_DataGrid = {0xCDE57A43,0x8B86,0x11D0,{0xB3,0xC6,0x00,0xA0,0xC9,0x0A,0xEA,0x82}};

const CLSID CLSID_CxTrendCtrl = {0x5B3622F5,0xEBAA,0x4DAE,{0x80,0xD0,0xDE,0xE4,0xF2,0x51,0xF9,0x69}};
const CLSID CLSID_CxAlmSumCtrl = {0x203E598C,0x03CD,0x4FD5,{0x8D,0xBB,0x90,0xE1,0x0D,0x21,0x0F,0x8C}};
const CLSID CLSID_CxRunCtrl = {0xC162A5CC,0x6BB1,0x4C0A,{0x90,0x81,0xC5,0x20,0xA4,0xEA,0xDD,0xA7}};

void CBoxCtrl::AddDefaultControlFolder()
{
	AddFolder(_T("�ؼ�"));

	int iFolder = GetFolderCount() - 1;

	CImageList* pImageList = new CImageList();
	VERIFY(pImageList->Create(IDB_CURSOR, 16, 0, RGB(192,192,192)));
	SetFolderImageList(iFolder, pImageList);

	InsertItem(iFolder, 0, NULL, 0, toolbox[0]);
//	InsertControl(1, CLSID_Label);
	InsertControl(1, CLSID_TextBox);
	InsertControl(1, CLSID_ComboBox);
	InsertControl(1, CLSID_ListBox);
	InsertControl(1, CLSID_CheckBox);
	InsertControl(1, CLSID_OptionButton);
	InsertControl(1, CLSID_ToggleButton);
	InsertControl(1, CLSID_CommandButton);
	InsertControl(1, CLSID_ScrollBar);
	InsertControl(1, CLSID_SpinButton);

	InsertControl(1, CLSID_ProgressBar);
	InsertControl(1, CLSID_Slider);
	InsertControl(1, CLSID_DTPicker);
	InsertControl(1, CLSID_MSFlexGrid);
	InsertControl(1, CLSID_Adodc);
	InsertControl(1, CLSID_DataGrid);

	InsertControl(1, CLSID_CxTrendCtrl);
//	InsertControl(1, CLSID_CxAlmSumCtrl);
	InsertControl(1, CLSID_CxRunCtrl);
}

BOOL CBoxCtrl::GetDefalutControlDesc(CLSID clsid, CString& strText)
{
	USES_CONVERSION;
	static CMapStringToString map;
	LPOLESTR ppsz;
	BOOL bRet;
	
	if (map.GetCount() == 0)
	{
		if (StringFromCLSID(CLSID_TextBox, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("�ı���"));
			CoTaskMemFree(ppsz);
		}
		
		if (StringFromCLSID(CLSID_ComboBox, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("���Ͽ�"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_ListBox, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("�б��"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_CheckBox, &ppsz) == S_OK)
		map.SetAt(OLE2T(ppsz), _T("��ѡ��"));
		CoTaskMemFree(ppsz);

		if (StringFromCLSID(CLSID_OptionButton, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("ѡ�ť"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_ToggleButton, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("�л���ť"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_CommandButton, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("���ť"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_ScrollBar, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("������"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_SpinButton, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("��ת��ť"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_ProgressBar, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("������"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_Slider, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("����"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_DTPicker, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("����ѡ��"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_MSFlexGrid, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("��������"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_Adodc, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("ADO ��������"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_DataGrid, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("��������"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_CxTrendCtrl, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("����"));
			CoTaskMemFree(ppsz);
		}

		if (StringFromCLSID(CLSID_CxRunCtrl, &ppsz) == S_OK)
		{
			map.SetAt(OLE2T(ppsz), _T("��������"));
			CoTaskMemFree(ppsz);
		}
	}

	bRet = FALSE;
	if (StringFromCLSID(clsid, &ppsz) == S_OK)
	{
		bRet = map.Lookup(OLE2T(ppsz), strText);
		CoTaskMemFree(ppsz);
	}

	return bRet;
}

////////////////////////////////////////////////////////////
//��  ��: ��ע����ж������е�����Ϣ
////////////////////////////////////////////////////////////
void CBoxCtrl::LoadWindow()
{
	USES_CONVERSION;

    TCHAR szSection[256];
	TCHAR szSubSection[256];
	TCHAR szItem[256];
	
	CString strValue, strFolderName;

	CToolboxBar* pParentWnd = (CToolboxBar*)GetParent();
	ASSERT(pParentWnd != NULL);
    wsprintf(szSection, _T("%s-SCBar-%d"), _T("BarState"),
			pParentWnd->GetDlgCtrlID());

	int iFolder = m_arFolder.GetSize();

	BOOL bFirstItem, bLastFolder; 
	bLastFolder = FALSE;

	do	
	{	//Folder
		wsprintf(szSubSection, _T("%s\\%s%d"), szSection, _T("Folder"), iFolder);
		int iItem = 1;
		bFirstItem = TRUE;
		do
		{	//Item
			if (bFirstItem)
			{	//ÿ�鿪ʼ����ָ��
				strFolderName = theApp.GetProfileString(szSubSection, "FolderName"); 
				if (strFolderName.IsEmpty())
				{
					bLastFolder = TRUE;
					break;
				}
				CImageList* pImageList = new CImageList();
				VERIFY(pImageList->Create(IDB_CURSOR, 16, 0, RGB(192,192,192)));
				AddFolder(strFolderName);
				SetFolderImageList(iFolder, pImageList);
				//ָ��
				InsertItem(iFolder, 0, NULL, 0, toolbox[0]);

				bFirstItem = FALSE;
			}

			wsprintf(szItem, _T("%s%d"), _T("Control"), iItem);
			strValue = theApp.GetProfileString(szSubSection, szItem);
			if (strValue.IsEmpty())
				break;
			//insert control to folder
			CLSID clsid;
			if (NOERROR != CLSIDFromString(T2OLE(strValue), &clsid))
				break;
		
			InsertControl(iFolder, clsid);
			iItem++;
		}
		while(TRUE);

		if (bLastFolder)
			break;
		iFolder++;
	}
	while(TRUE);

	if (theApp.IsFirstRun())
		AddDefaultControlFolder();
}
////////////////////////////////////////////////////////////
//��  ��: �����е�����Ϣд��ע���
////////////////////////////////////////////////////////////
void CBoxCtrl::SaveWindow()
{
	int nFolderCount = m_arFolder.GetSize();
	ASSERT(nFolderCount > 0);

	CWinApp* pApp = AfxGetApp();
    TCHAR szSection[256];
	TCHAR szSubSection[256];
	TCHAR szItem[256];

	CToolboxBar* pParentWnd = (CToolboxBar*)GetParent();
	ASSERT(pParentWnd != NULL);
    wsprintf(szSection, _T("%s-SCBar-%d"), _T("BarState"),
        pParentWnd->GetDlgCtrlID());

	int nIndexKey;
	TCHAR    szSubKey[64];
	TCHAR    szKey[1024];
	HKEY    hkAppKey;
	//HKEY_CURRENT_USER\Software\Local AppWizard-Generated Applications\CxDev\BarState-SCBar-111\Folder1

	wsprintf(szKey, 
		_T("Software\\SuperCx\\SuperCx ���濪������\\%s"),
		szSection);

	if (RegOpenKey( HKEY_CURRENT_USER, 
		szKey, 
		&hkAppKey) == ERROR_SUCCESS)
	{	//ÿ�α���ʱ��Ҫ������е�ע�������
		for ( nIndexKey = 0 ;
			RegEnumKey(hkAppKey, nIndexKey, szSubKey, sizeof(szSubKey)) == ERROR_SUCCESS ;
			++nIndexKey)
		{
			if (szSubKey == strstr(szSubKey, _T("Folder")))
			{
				RegDeleteKey(hkAppKey, szSubKey);
			}
		}
		RegCloseKey(hkAppKey);
	}

	int cchGuid;
	OLECHAR szClassID[39];
	LPCTSTR lpszClassID;

	USES_CONVERSION;

	for (int i = 1; i < nFolderCount; i++)
	{
		CBarFolder* pBarFolder = (CBarFolder*)m_arFolder.GetAt(i);
		int nItemCount = pBarFolder->m_arItems.GetSize();

		wsprintf(szSubSection, _T("%s\\%s%d"), szSection, _T("Folder"), i);

		for (int j = 0; j < nItemCount; j++)
		{
			if (j == 0)
			{	//д������
				pApp->WriteProfileString(szSubSection, "FolderName", pBarFolder->m_szName);
				continue;
			}
			CBarItem* pBarItem = (CBarItem*)pBarFolder->m_arItems.GetAt(j);
			if (pBarItem->m_bControl)
			{
				CLSID clsid = *((CLSID*)(pBarItem->m_dwData));
				cchGuid = ::StringFromGUID2(clsid, szClassID, 39);
				lpszClassID = OLE2CT(szClassID);
				ASSERT(cchGuid == 39);    

				wsprintf(szItem, _T("%s%d"), _T("Control"), j);
				pApp->WriteProfileString(szSubSection, szItem, lpszClassID);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////

BOOL CBoxCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL bRet = CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, ID_BOXCTRL_WND, NULL);
	EnableToolTips(TRUE);
	return bRet;
}


BEGIN_MESSAGE_MAP(CBoxCtrl, CWnd)
	//{{AFX_MSG_MAP(CBoxCtrl)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_COMMAND(ID_ADD_FOLDER, OnAddFolder)
	ON_COMMAND(ID_REMOVE_FOLDER, OnRemoveFolder)
	ON_COMMAND(ID_RENAME_FOLDER, OnReNameFolder)
	ON_COMMAND(ID_ADD_ITEM, OnAddItem)
	ON_COMMAND(ID_REMOVE_ITEM, OnRemoveItem)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CBoxCtrl message handlers
//////////////////////////////////////
//��  ��:	���ƴ���
//////////////////////////////////////
void CBoxCtrl::OnPaint() 
{
	CRect rcClient;
	GetClientRect(&rcClient);

	CPaintDC dcPaint(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dcPaint);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dcPaint, rcClient.Width(), rcClient.Height());
	CBitmap * pOldbmp = dcMem.SelectObject(&bmp);

	CDC * pMemDC = &dcMem;

	//�����ͻ�������ɫ
	CRect rcFolderClient;
	GetFolderClientRect(rcFolderClient);
	pMemDC->FillSolidRect(rcFolderClient, crBackGroundColor);

	//���Ƹ��������
	int i;	
	CRect rcFolder;
	int nFolderCount = m_arFolder.GetSize();
	for (i = 0; i < nFolderCount; i++)
	{
		GetFolderRect(i, rcFolder);
		DrawFolder(pMemDC, i, rcFolder, FALSE);
	}

	//�������еĸ�����(����ͨ��Ϊѡ�е���)
	if (m_nSelFolder >= 0)
		PaintItems(pMemDC, m_nSelFolder, rcFolderClient);

	dcPaint.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), 
						&dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldbmp);
}

//////////////////////////////////////
//��  ��:	�������ͼ��
//////////////////////////////////////
BOOL CBoxCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint pt(GetMessagePos());
	ScreenToClient(&pt);
	int nIndex, nHit;

	nHit = HitTestEx(pt, nIndex);

	if (nHit == HT_Folder)
	{
		SetCursor(AfxGetApp()->LoadCursor(IDC_HANDCUR));
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
//////////////////////////////////////
//��  ��:	��Ӧ�����ƶ�
//˵  ��:	��������:
//			1�����ü�����
//			2������������������
//////////////////////////////////////
void CBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
 		if (_TrackMouseEvent(&tme))
			m_bMouseTracking = TRUE;
	}
	
	int nIndex;
	int nHit = HitTestEx(point, nIndex);

	if (nHit == HT_Folder)
		HighlightFolder(nIndex);
	else
		HighlightFolder(-1);

	if (nHit == HT_Item)
		HighlightItem(nIndex);
	else
		HighlightItem(-1);
}
//////////////////////////////////////
//��  ��:	��Ӧ�����Ƴ��ͻ���
//˵  ��:	ʹ���е�����ڷǸ���״̬
//////////////////////////////////////

LRESULT CBoxCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	HighlightItem(-1);
	m_bMouseTracking = FALSE;

	return 0;
}
//////////////////////////////////////
//��  ��:	��Ӧ�������
//˵  ��:	�����е����顢���������,
//			Ȼ��������Ӧ�Ĳ���
//////////////////////////////////////
void CBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
 	if (GetFocus() != this)
 		SetFocus();
 
 	int nIndex, nHit = HitTestEx(point, nIndex);
 
 	CRect rcFolderClient;
 	GetFolderClientRect(rcFolderClient);
 
 	if (nHit == HT_Folder)
 	{	//���������
 		BOOL bHigh = TRUE;
 		CRect rcFolder;
 		GetFolderRect(nIndex, rcFolder);
 
 		if (::GetCapture() == NULL)
 		{
 			SetCapture();
 			ASSERT(this == GetCapture());
 			CClientDC dc(this);
 			//���������Ϊ����״̬
 			DrawFolder(&dc, nIndex, rcFolder, TRUE);
 			//��׽����������Ϣ
 			AfxLockTempMaps();
 			for (;;)
 			{
 				MSG msg;
 				VERIFY(::GetMessage(&msg, NULL, 0, 0));
 
 				if (CWnd::GetCapture() != this) 
 					break;
 
 				switch (msg.message)
 				{
 				case WM_MOUSEMOVE:
 					{
 						CPoint pt(msg.lParam);
 						int idx, ht1 = HitTestEx(pt, idx);
 						if (ht1 == HT_Folder && idx == nIndex)
 						{
 							if (!bHigh)
 							{
 								DrawFolder(&dc, nIndex, rcFolder, TRUE);
 								bHigh = TRUE;
 							}
 						}
 						else
 						{
 							if (bHigh)
 							{
 								DrawFolder(&dc, nIndex, rcFolder, FALSE);
 								bHigh = FALSE;
 							}
 						}
 					}
 					break;
 
 				case WM_LBUTTONUP:
 					{	
 						if (bHigh)
 						{
 							DrawFolder(&dc, nIndex, rcFolder, FALSE);
 							bHigh = FALSE;
 						}
 						CPoint pt(msg.lParam);
 						int idx, ht1 = HitTestEx(pt, idx);
 						if (ht1 == HT_Folder && idx != m_nSelFolder)
 							SetSelFolder(idx);
 					}
 					goto ExitLoop2;
 
 				case WM_KEYDOWN:	
 					if (msg.wParam != VK_ESCAPE) 
 						break;
 				default:
 					DispatchMessage(&msg);
 					break;
 				}
 			}
 
 		ExitLoop2:
 			ReleaseCapture();
 			//�ͷŲ�׽��Ϣ
 			AfxUnlockTempMaps(FALSE);
 		}
 		if (bHigh) 
 			InvalidateRect(rcFolder, FALSE);
 	}
 	else if (nHit == HT_Item)
 	{	//������
 		OnChangeSelItem(nIndex);
 	}
}

//////////////////////////////////////
//��  ��:	��Ӧ�����һ�
//˵  ��:	��Ҫ�ǵ�����������ز˵�
//////////////////////////////////////
void CBoxCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int nHit, nIndex;
	nHit = HitTestEx(point, nIndex);
	m_nRSelItem = nIndex;

	CMenu Menu;
	Menu.CreatePopupMenu();

	SendMessage(WM_MOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y));

	if (nHit == HT_Folder)
	{	//���������
		Menu.AppendMenu(MF_STRING, ID_ADD_FOLDER,		IDS_ADDFOLDER);
		if (m_nRSelItem > 0)
		{
			Menu.AppendMenu(MF_STRING, ID_REMOVE_FOLDER,	IDS_REMOVEFOLDER);
			Menu.AppendMenu(MF_STRING, ID_RENAME_FOLDER,	IDS_RENAMEFOLDER);
		}
	}
	else
	{	//���пͻ���
		Menu.AppendMenu(MF_STRING, ID_ADD_FOLDER,		IDS_ADDFOLDER);
		if (m_nSelFolder > 0)
		{
			Menu.AppendMenu(MF_STRING, ID_ADD_ITEM,		IDS_ADDITEM);
			if (nHit == HT_Item)	//������
				Menu.AppendMenu(MF_STRING, ID_REMOVE_ITEM,	IDS_REMOVEITEM);
		}
	}

	ClientToScreen(&point);
	Menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);

	Menu.DestroyMenu();
}
//////////////////////////////////////
//��  ��:	��Ӧ��ݲ˵���"�����"
//////////////////////////////////////
void CBoxCtrl::OnAddFolder()
{
	CFolderNameDlg dlg;

	dlg.m_strFolderName = IDS_DEFAULT_FOLDERNAME;
	if(IDOK != dlg.DoModal())
		return;

	CImageList* pImageList = new CImageList();
	VERIFY(pImageList->Create(IDB_CURSOR, 16, 0, RGB(192,192,192)));

	int nIndex = AddFolder(dlg.m_strFolderName);
	SetFolderImageList(nIndex, pImageList);
	InsertItem(nIndex, 0, "Item 1", 0, 0);

	m_nSelFolder	= nIndex;
	m_nSelItem		= 0;

	m_nRSelItem = nIndex;
	Invalidate();
}
//////////////////////////////////////
//��  ��:	��Ӧ��ݲ˵���"ɾ����"
//////////////////////////////////////
void CBoxCtrl::OnRemoveFolder()
{
	RemoveFolder(m_nRSelItem);
}
//////////////////////////////////////
//��  ��:	��Ӧ��ݲ˵���"��������"
//////////////////////////////////////
void CBoxCtrl::OnReNameFolder()
{
	CFolderNameDlg dlg;
	ASSERT(m_nRSelItem != -1);
	dlg.m_strFolderName = GetFolderName(m_nRSelItem);
	if(IDOK == dlg.DoModal())
	{
		SetFolderName(m_nRSelItem, dlg.m_strFolderName);
		Invalidate();
	}
}
//////////////////////////////////////
//��  ��:	��Ӧ��ݲ˵���"�����"
//////////////////////////////////////
void CBoxCtrl::OnAddItem()
{
	CInsertControlDlg dlg;
	CBarFolder* pBarFolder = (CBarFolder*) m_arFolder.GetAt(m_nSelFolder);
	if(pBarFolder == NULL)
		return;

	//������ѡ��
	CLSID tmpCLSID;
	int count = pBarFolder->GetItemCount();
	for(int i = 0; i < count; i++)
	{
		CBarItem* item = (CBarItem *)(pBarFolder->m_arItems[i]);
		if (item->m_bControl)
		{
			tmpCLSID = *((CLSID *)(item->m_dwData));
			dlg.m_aryClsid.AddTail(tmpCLSID);
		}
	}

	//��������
	int nResult;
	nResult = dlg.DoModal();
	if (nResult != IDOK)
		return;
	POSITION pos = dlg.m_aryClsid.GetHeadPosition();
	while(pos != NULL)
	{
		tmpCLSID = dlg.m_aryClsid.GetNext(pos);
		pBarFolder = (CBarFolder*) m_arFolder.GetAt(m_nSelFolder);

		if (pBarFolder->FindItemByCLSID(&tmpCLSID) == -1)
			InsertControl(m_nSelFolder, tmpCLSID);
	}

	//ɾ��ûѡ����
	pBarFolder->RemoveUnusedCtrlItem(&(dlg.m_aryClsid));

	Invalidate();
}
//////////////////////////////////////
//��  ��:	��Ӧ��ݲ˵���"��������"
//////////////////////////////////////
void CBoxCtrl::OnRemoveItem()
{
	if (m_nRSelItem > 0)
		RemoveItem(m_nRSelItem);
}

////////////////////////////////////////////////////////////////
//����:�ж�ָ����CLSID�Ƿ��Ѵ����ڹ�������
//����:pClsid[in]		Ҫ�ж���CLSIDָ��
//����:�Ѵ���TRUE,������FALSE
////////////////////////////////////////////////////////////////
int CBoxCtrl::CBarFolder::FindItemByCLSID(CLSID *pClsid)
{
	BOOL bRet = FALSE;

	int nCount = m_arItems.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CBarItem* pItem = (CBarItem *)(m_arItems[i]);
		if (pItem == NULL)
			continue;

		if (pItem->m_bControl)
		{
			CLSID* pClsidT = (CLSID *)(pItem->m_dwData);
			if (*pClsid == *pClsidT)
				return i;
		}
	}

	return -1;
}

//////////////////////////////////////////////////////////////
//����: ��plist�б�����Ŀؼ����ôӱ�������ɾ��
//����: plist[in]		ָ��Ҫ����������б�ָ��,����ΪCList<CLSID, CLSID&>
//////////////////////////////////////////////////////////////////
void CBoxCtrl::CBarFolder::RemoveUnusedCtrlItem(void *plist)
{
	CList<CLSID, CLSID&>& listID = *((CList<CLSID, CLSID&>*)plist);
	int count = GetItemCount();

	//�����ж��Ƿ񱣴�
	CPtrArray tmpArray;
	CPtrArray delArray;
	CBarItem* pItem = NULL;
	for (int i = 0; i < count; i++)
	{
		pItem = (CBarItem *)m_arItems[i];
		if(pItem->m_bControl)
		{	//�ǿؼ�
			POSITION pos = listID.GetHeadPosition();
			BOOL flagKeep = FALSE;
			while(pos != NULL)
			{	//���Ҹÿؼ��Ƿ���listID��
				CLSID tmpCLSID = listID.GetNext(pos);
				if(tmpCLSID == *((CLSID*)(pItem->m_dwData)) )
				{
					flagKeep = TRUE;
					break;
				}
			}

			if(flagKeep)
				tmpArray.Add(pItem);
			else
				delArray.Add(pItem);
		}
		else
		{	//���ǿؼ�
			tmpArray.Add(pItem);
		}
	}

	//ɾ�����õĿؼ�
	count = delArray.GetSize();
	for(i = 0; i < count; i++)
	{
		pItem = (CBarItem *)delArray[i];
		int nIcon = pItem->m_nImageIndex;
		delete pItem;
		RemoveIcon(nIcon);
	}
	
	//������Ҫ�Ķ���
	m_arItems.RemoveAll();
	count = tmpArray.GetSize();
	for(i = 0; i < count; i++)
	{
		m_arItems.Add(tmpArray[i]);
	}
}

/////////////////////////////////////////////////////////////
//����:��ɾ��һ�����,ɾ����Ӧ��ICON��Դ
//����:nIndex[in]		ͼ�������
//˵��:һ��Ҫ�ڶ���ɾ����,��ɾ����ͼ��, ����ͼ�������Ķ�ӳ���λ
//////////////////////////////////////////////////////////////
void CBoxCtrl::CBarFolder::RemoveIcon(int nIndex)
{
	if(nIndex < 1)	//��ɾ��Ĭ�ϵ�һ��ͼ��
		return;
	if (!m_pImageList->Remove(nIndex))
	{
		TRACE("ɾ���ؼ���ͼ��ʧ��");
		return;
	}

	//�޸����������ͼ��������
	int count = m_arItems.GetSize();
	for(int i = 0; i < count; i++)
	{
		CBarItem* pItem = (CBarItem*)(m_arItems[i]);
		if(pItem->m_nImageIndex > nIndex)
			pItem->m_nImageIndex--;
	}
}

int CBoxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HBITMAP hbmGray = ::CreateDitherBitmap();
	ASSERT(hbmGray != NULL);

	m_brushDither.CreatePatternBrush(CBitmap::FromHandle(hbmGray));
	::DeleteObject(hbmGray);
	
	EnableToolTips(TRUE);
	return 0;
}

int CBoxCtrl::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
	int nIndex;
	if(HitTestEx(point, nIndex) != HT_Item)
	{
		return CWnd::OnToolHitTest(point, pTI);
	};

	CRect rect;
	GetItemRect(m_nSelFolder, nIndex, rect);
	pTI->hwnd = m_hWnd;
	pTI->uId = nIndex;
	pTI->rect.left = rect.left;
	pTI->rect.top = rect.top;
	pTI->rect.right = rect.right;
	pTI->rect.bottom = rect.bottom;
	pTI->lpszText = LPSTR_TEXTCALLBACK;//ID_DRAW_LINE;
	return nIndex + 1;
};

BOOL CBoxCtrl::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

	const MSG* pMessage;
	CPoint pt;
	pMessage = GetCurrentMessage();
	ASSERT ( pMessage );
	pt = pMessage->pt;
	ScreenToClient( &pt );

	int nIndex;
	if(HitTestEx(pt, nIndex) != HT_Item)
		return FALSE;

	CBarFolder * pBarFolder = (CBarFolder*)m_arFolder.GetAt(m_nSelFolder);
	ASSERT(pBarFolder != NULL);
	CBarItem * pBarItem = (CBarItem*) pBarFolder->m_arItems.GetAt(nIndex);
	ASSERT(pBarItem != NULL);
	CString strTipText;
	if(!pBarItem->m_bControl)
	{	//�ڲ�����
		strTipText.LoadString(pBarItem->m_dwData);
		int nNext = strTipText.Find("\n", 0);
		strTipText = strTipText.Right(strTipText.GetLength() - nNext - 1);
	}
	else
	{	//���
		strTipText = pBarItem->m_szItem;
	}

#ifndef _UNICODE
		lstrcpyn(pTTT->szText, strTipText, 80);
#else
		_mbstowcsz(pTTT->szText, strTipText, 80);
#endif

	*pResult = 0;

	return TRUE;    // message was handled
};
