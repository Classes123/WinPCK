//////////////////////////////////////////////////////////////////////
// ShowPicture.cpp: ��ʾdds��tgaͼ��
// ����dds��tga����ʾ
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ShowPicture.h"
#include <tchar.h>

#include <math.h>

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

BOOL CShowPicture::isEqual(double d1, double d2)
{
	return (EPSILON > fabs((d1-d2)));
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


void CalcZoomInAreaAndDrawPosition(__int64 &dst_xy, __int64 &dst_wh, __int64 &src_xy, __int64 &src_wh, double dZoomRatio, LONG iClientWidthHeight, UINT uiPicWidthHeight)
{
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

	__int64		left_invisible = -dst_xy;
	double	left_invisible_real_double = left_invisible / dZoomRatio;
	__int64		left_invisible_real = (__int64)left_invisible_real_double;
	__int64		left_invisible_rem = (__int64)(left_invisible - left_invisible_real * dZoomRatio);
	__int64		right_invisible = dst_wh - left_invisible - iClientWidthHeight;
	__int64		right_invisible_real_double = (__int64)(right_invisible / dZoomRatio);
	__int64		right_invisible_real = (__int64)right_invisible_real_double;
	__int64		right_invisible_rem = (__int64)(right_invisible - right_invisible_real * dZoomRatio);
	__int64		visible_width_real = uiPicWidthHeight - left_invisible_real - right_invisible_real;

	dst_xy = -left_invisible_rem;
	src_xy = left_invisible_real;
	src_wh = visible_width_real;
	dst_wh = iClientWidthHeight + right_invisible_rem + left_invisible_rem;

}

//���豸����ʾͼ��
BOOL CShowPicture::Paint(__int64 nXOriginDest, __int64 nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio)
{
	/*
	�趨�������Ͻ�����Ϊ 0,0
	pic_x:����ʾͼ�� �����Ͻ�����ڴ��ڵ�λ��
	*/
	RECT rectDlg;

	HDC pDC = ::GetDC(m_hWndShow);
	GetClientRect(m_hWndShow, &rectDlg);

	//�ؼ���Դ��㡢��ߺ�Ŀ����㡢���
	__int64 src_x = nXOriginSrc;
	__int64 src_y = nYOriginSrc;
	__int64 src_w = m_picWidth;
	__int64 src_h = m_picHeight;
	__int64 dst_x = nXOriginDest;
	__int64 dst_y = nYOriginDest;
	__int64 dst_w = m_picWidth;
	__int64 dst_h = m_picHeight;

	PAINTSTRUCT ps;
	HDC pDC1 = BeginPaint(m_hWndShow, &ps);

	if(((1.0 - EPSILON) < dZoomRatio) && ((1.0 + EPSILON) > dZoomRatio)) {
		//if(isEqual(1.0, dZoomRatio)) {
			//������
		if(nXOriginDest < 0) {

			dst_x = 0;
			src_x = nXOriginSrc - nXOriginDest;
			dst_w = (((__int64)m_picWidth - src_x) > (__int64)rectDlg.right) ? (__int64)rectDlg.right : ((__int64)m_picWidth - src_x);
		}
		if(nYOriginSrc < 0) {

			dst_y = 0;
			src_y = nYOriginSrc - nYOriginDest;
			dst_h = (((__int64)m_picHeight - src_y) >(__int64)rectDlg.bottom) ? (__int64)rectDlg.bottom : ((__int64)m_picHeight - src_y);
		}

		BitBlt(	
			pDC,				// Ŀ�� DC ���
			(int)dst_x,				// Ŀ�����Ͻ�X����
			(int)dst_y,				// Ŀ�����Ͻ�Y����
			(int)dst_w,				// Ŀ����
			(int)dst_h,				// Ŀ��߶�
			m_MemDC, 			// Դ DC ���
			(int)src_x,				// Դ���Ͻ�X����
			(int)src_y,				// Դ���Ͻ�Y����
			SRCCOPY);

	} else {
		//����
		dst_w = (__int64)(m_picWidth * dZoomRatio + 0.5);
		dst_h = (__int64)(m_picHeight * dZoomRatio + 0.5);

		if(rectDlg.right < dst_w)
			CalcZoomInAreaAndDrawPosition(dst_x, dst_w, src_x, src_w, dZoomRatio, rectDlg.right, m_picWidth);

		if(rectDlg.bottom < dst_h)
			CalcZoomInAreaAndDrawPosition(dst_y, dst_h, src_y, src_h, dZoomRatio, rectDlg.bottom, m_picHeight);

		SetStretchBltMode(pDC, COLORONCOLOR);
		StretchBlt(
			pDC,				// Ŀ�� DC ���
			(int)dst_x,				// Ŀ�����Ͻ�X����
			(int)dst_y,				// Ŀ�����Ͻ�Y����
			(int)dst_w,				// Ŀ����
			(int)dst_h,				// Ŀ��߶�
			m_MemDC,			// Դ DC ���
			(int)src_x,				// Դ���Ͻ�X����
			(int)src_y,				// Դ���Ͻ�Y����
			(int)src_w,				// Դ���
			(int)src_h,				// Դ�߶�
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

	for(unsigned int j = 0;j < m_picHeight;j += 8) {
		for(unsigned int i = 0;i < m_picWidth;i += 16) {
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