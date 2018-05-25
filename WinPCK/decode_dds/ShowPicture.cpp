#include <Windows.h>
#include "ShowPicture.h"
#include <tchar.h>


CShowPicture::CShowPicture(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCTSTR _lpszFileTitle, PICFORMAT _picFormat) :
	m_hWndShow(hWndShow),
	m_lpszFileTitle(_lpszFileTitle),
	m_picFormat(_picFormat),
	m_bufsize(_bufsize),
	m_stream(NULL),
	m_hGlobal(NULL),
	m_lpmyImage(NULL),
	m_lpoGraph(NULL),
	m_DecodedDIBData(NULL)
{


	if(FMT_RAW != (m_picFormat))	//dds,tga
	{
		m_resBufferInClass = (LPBYTE)malloc(m_bufsize);
	} else {							//bmp,jpg,png..
		m_resBufferInClass = (LPBYTE)(m_hGlobal = GlobalAlloc(GMEM_FIXED, m_bufsize));
	}

	if(NULL != m_resBufferInClass) {
		//��������
		memcpy(m_resBufferInClass, _buffer, _bufsize);
	}

	//������ɺ��ͷŴ�����ڴ�
	free(_buffer);
	_buffer = NULL;
	
	GdiplusStartup(&m_pGdiPlusToken, &m_oGdiPlusStartupInput, NULL);
}


CShowPicture::~CShowPicture()
{
	if(NULL != m_lpoGraph)
		delete m_lpoGraph;

	if(NULL != m_lpmyImage)
		delete m_lpmyImage;

	if(NULL != m_stream)
		m_stream->Release();

	if(FMT_RAW != m_picFormat) {
		if(NULL != m_resBufferInClass) {
			free(m_resBufferInClass);
		}
	} else {

		if(NULL != m_hGlobal)
			GlobalFree(m_hGlobal);
	}

	if(NULL != m_DecodedDIBData) {
		free(m_DecodedDIBData);
	}

	DeleteObject(m_MemBitmap);
	DeleteDC(m_MemDC);

	GdiplusShutdown(m_pGdiPlusToken);
}

LPCTSTR	CShowPicture::GetWindowTitle(LPTSTR	lpszTitle, size_t bufSize)
{
	static TCHAR szTitle[MAX_PATH];

	if(NULL == lpszTitle)
		lpszTitle = szTitle;

	//��������
	if(FMT_RAW != m_picFormat) {
#ifdef UNICODE
		_stprintf_s(lpszTitle, MAX_PATH, TEXT("ͼƬ�鿴 - %s, %dx%d, %S"), m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
#else
		_stprintf_s(lpszTitle, MAX_PATH, TEXT("ͼƬ�鿴 - %s, %dx%d, %s"), m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
#endif
	} else {
		_stprintf_s(lpszTitle, MAX_PATH, TEXT("ͼƬ�鿴 - %s, %dx%d"), m_lpszFileTitle, m_picWidth, m_picHeight);
	}

	//SetWindowText(m_hWndShow, lpszTitle);
	return lpszTitle;
}

BOOL CShowPicture::ShowTitleOnWindow()
{
	TCHAR szTitle[MAX_PATH];

	//��������
	if(FMT_RAW != m_picFormat) {
#ifdef UNICODE
		_stprintf_s(szTitle, MAX_PATH, TEXT("ͼƬ�鿴 - %s, %dx%d, %S"), m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
#else
		_stprintf_s(szTitle, MAX_PATH, TEXT("ͼƬ�鿴 - %s, %dx%d, %s"), m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
#endif
	} else {
		_stprintf_s(szTitle, MAX_PATH, TEXT("ͼƬ�鿴 - %s, %dx%d"), m_lpszFileTitle, m_picWidth, m_picHeight);
	}

	SetWindowText(m_hWndShow, szTitle);
	return TRUE;
}

//����ͼ��Ŀ�͸�
UINT CShowPicture::GetWidth()
{
	return m_picWidth;
}

UINT CShowPicture::GetHeight()
{
	return m_picHeight;
}

//���豸����ʾͼ��
BOOL CShowPicture::Paint(int nXOriginDest, int nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio)
{
	/*
	�趨�������Ͻ�����Ϊ 0,0
	pic_x:����ʾͼ�� �����Ͻ�����ڴ��ڵ�λ��
	*/
	RECT rectDlg;

	HDC pDC = ::GetDC(m_hWndShow);
	GetClientRect(m_hWndShow, &rectDlg);

	//�ؼ���Դ��㡢��ߺ�Ŀ����㡢���
	int src_x = nXOriginSrc;
	int src_y = nYOriginSrc;
	int src_w = m_picWidth;
	int src_h = m_picHeight;
	int dst_x = nXOriginDest;
	int dst_y = nYOriginDest;
	int dst_w = m_picWidth;
	int dst_h = m_picHeight;

	PAINTSTRUCT ps;
	HDC pDC1 = BeginPaint(m_hWndShow, &ps);

	if((0.999 < dZoomRatio) && (1.0001 > dZoomRatio)) {
		//������
		if(nXOriginDest < 0) {

			dst_x = 0;
			src_x = nXOriginSrc - nXOriginDest;
			dst_w = ((m_picWidth - src_x) > rectDlg.right) ? rectDlg.right : (m_picWidth - src_x);
		}
		if(nYOriginSrc < 0) {

			dst_y = 0;
			src_y = nYOriginSrc - nYOriginDest;
			dst_h = ((m_picHeight - src_y) > rectDlg.bottom) ? rectDlg.bottom : (m_picHeight - src_y);
		}

		BitBlt(	
			pDC,				// Ŀ�� DC ���
			dst_x,				// Ŀ�����Ͻ�X����
			dst_y,				// Ŀ�����Ͻ�Y����
			dst_w,				// Ŀ����
			dst_h,				// Ŀ��߶�
			m_MemDC, 			// Դ DC ���
			src_x,				// Դ���Ͻ�X����
			src_y,				// Դ���Ͻ�Y����
			SRCCOPY);

	} else {
		//����
		int tmp_dst_w = m_picWidth * dZoomRatio, tmp_dst_h = m_picHeight * dZoomRatio;
		dst_w = m_picWidth * dZoomRatio;
		dst_h = m_picHeight * dZoomRatio;

		//	/* StretchBlt() Modes */
		//	#define BLACKONWHITE                 1
		//	#define WHITEONBLACK                 2
		//	#define COLORONCOLOR                 3
		//	#define HALFTONE                     4
		//	#define MAXSTRETCHBLTMODE            4
		//	#if(WINVER >= 0x0400)
		//	/* New StretchBlt() Modes */
		//	#define STRETCH_ANDSCANS    BLACKONWHITE
		//	#define STRETCH_ORSCANS     WHITEONBLACK
		//	#define STRETCH_DELETESCANS COLORONCOLOR
		//	#define STRETCH_HALFTONE    HALFTONE
#if 1
		/*
		�����ź�ͼ����ڴ��ں��趨�������Ͻ�����Ϊ 0,0
		���δ��ʾ���������趨Ϊleft_invisible(����ֵ),�Ҷ�Ϊright_invisible����������ΪRatio��
		�����δ��ʾ��ʵ��ͼ���С����Ϊ left_invisible_real = (int)left_invisible/Ratio������Ϊleft_invisible_rem
		�Ҳ���ͬΪright_invisible = tmp_dst_w - left_invisible - rectDlg.right��right_invisible_real = (int)right_invisible/Ratio��right_invisible_rem
		ͬʱ����������(left_invisible_rem)ΪҪȡ���������ź���ʾ�ڴ����ϵ�ƫ��
		��ʵ��ͼ����Ϊm_picWidthʵ��Ҫȡ������Ϊ��visible_width_real = m_picWidth - left_invisible_real - right_invisible_real, 
		Ŀ�����Ͻ���ʾ����dst_x = left_invisible_rem
		Դ���Ͻ�X����src_x = left_invisible_real
		*/
		if(rectDlg.right < tmp_dst_w) {

			int		left_invisible = -nXOriginDest;
			double	left_invisible_real_double = left_invisible / dZoomRatio;
			int		left_invisible_real = (int)left_invisible_real_double;
			int		left_invisible_rem = left_invisible - left_invisible_real * dZoomRatio;
			int		right_invisible = tmp_dst_w - left_invisible - rectDlg.right ;
			int		right_invisible_real_double = right_invisible / dZoomRatio;
			int		right_invisible_real = (int)right_invisible_real_double;
			int		right_invisible_rem = right_invisible - right_invisible_real * dZoomRatio;
			int		visible_width_real = m_picWidth - left_invisible_real - right_invisible_real;


			dst_x = -left_invisible_rem;
			src_x = left_invisible_real;
			src_w = visible_width_real;
			dst_w = rectDlg.right + right_invisible_rem + left_invisible_rem;

			double dZoomRatio_2 = dst_w / (double)src_w;
			dZoomRatio_2 = 1;
		}

		if(rectDlg.bottom < tmp_dst_h) {
			int		top_invisible = -nYOriginDest;
			double	top_invisible_real_double = top_invisible / dZoomRatio;
			int		top_invisible_real = (int)top_invisible_real_double;
			int		top_invisible_rem = top_invisible - top_invisible_real * dZoomRatio;
			int		bottom_invisible = tmp_dst_h - top_invisible - rectDlg.bottom;
			int		bottom_invisible_real_double = bottom_invisible / dZoomRatio;
			int		bottom_invisible_real = (int)bottom_invisible_real_double;
			int		bottom_invisible_rem = bottom_invisible - bottom_invisible_real * dZoomRatio;
			int		visible_height_real = m_picHeight - top_invisible_real - bottom_invisible_real;


			dst_y = -top_invisible_rem;
			src_y = top_invisible_real;
			src_h = visible_height_real;
			dst_h = rectDlg.bottom + bottom_invisible_rem + top_invisible_rem;

			double dZoomRatio_2 = dst_h / (double)src_h;
			dZoomRatio_2 = 1;
		}
#endif
		SetStretchBltMode(pDC, COLORONCOLOR);
		StretchBlt(
			pDC,				// Ŀ�� DC ���
			dst_x,				// Ŀ�����Ͻ�X����
			dst_y,				// Ŀ�����Ͻ�Y����
			dst_w,				// Ŀ����
			dst_h,				// Ŀ��߶�
			m_MemDC,			// Դ DC ���
			src_x,				// Դ���Ͻ�X����
			src_y,				// Դ���Ͻ�Y����
			src_w,				// Դ���
			src_h,				// Դ�߶�
			SRCCOPY);
	}

	ReleaseDC(m_hWndShow, pDC);
	EndPaint(m_hWndShow, &ps);

	return TRUE;
}

//�ڴ���������ʾ��͸�������ĸ���
BOOL CShowPicture::DrawBlockOnDlg()
{
	//HDC pDC = ::GetDC(GetDlgItem(IDC_STATIC_PIC));
	HDC pDC = ::GetDC(m_hWndShow);
	//GetClientRect(hWnd, &rect);

	//CDC MemDC; //���ȶ���һ����ʾ�豸����
	HDC MemDCTemp;
	//CBitmap MemBitmap;//����һ��λͼ����
	HBITMAP MemBitmapTemp;
	//���������Ļ��ʾ���ݵ��ڴ���ʾ�豸  
	m_MemDC = CreateCompatibleDC(NULL);
	MemDCTemp = CreateCompatibleDC(NULL);
	//��ʱ�����ܻ�ͼ����Ϊû�еط���
	//���潨��һ������Ļ��ʾ���ݵ�λͼ��λͼ�Ĵ�С�����ô��ڵĴ�С 
	m_MemBitmap = CreateCompatibleBitmap(pDC, m_picWidth, m_picHeight);
	MemBitmapTemp = CreateCompatibleBitmap(pDC, 24, 8);

	ReleaseDC(m_hWndShow, pDC);

	//��λͼѡ�뵽�ڴ���ʾ�豸�� 
	//ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ�� 
	HBITMAP pOldBit = (HBITMAP)SelectObject(m_MemDC, m_MemBitmap);
	HBITMAP pOldBit1 = (HBITMAP)SelectObject(MemDCTemp, MemBitmapTemp);

	//���ñ���ɫ��λͼ����ɾ����������õ��ǰ�ɫ��Ϊ���� 
	//��Ҳ�������Լ�Ӧ���õ���ɫ 
	SetBkColor(MemDCTemp, RGB(204, 204, 204));
	RECT thisrect = { 0, 0, 24, 8 };
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(255, 255, 255));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(204, 204, 204));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	for(int j = 0;j < m_picHeight;j += 8) {
		for(int i = 0;i < m_picWidth;i += 16) {
			if(j & 8)
				BitBlt(m_MemDC, i, j, i + 16, j + 8, MemDCTemp, 0, 0, SRCCOPY);
			else
				BitBlt(m_MemDC, i, j, i + 16, j + 8, MemDCTemp, 8, 0, SRCCOPY);
		}
	}
	//MemBitmap.DeleteObject(); 
	SelectObject(MemDCTemp, pOldBit1);
	//a = DeleteObject(MemBitmap);
	DeleteObject(MemBitmapTemp);
	//MemDC.DeleteDC();
	DeleteDC(MemDCTemp);

	return TRUE;
}

BOOL CShowPicture::Decode()
{
	if(NULL == m_resBufferInClass)return FALSE;
	if (!BuildImage())return FALSE;
	if (!DrawPictureOnDC())return FALSE;
	if (!ShowTitleOnWindow())return FALSE;
	return TRUE;
}