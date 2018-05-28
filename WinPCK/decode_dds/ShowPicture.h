#pragma once

#define EPSILON 0.000001

#include "DdsTgaDecoderDefine.h"
#include <Gdiplus.h>
using namespace Gdiplus;
//#pragma comment(lib,"gdiplus.lib")

class CShowPicture
{
public:
	CShowPicture(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCTSTR _lpszFileTitle, PICFORMAT _picFormat);
	~CShowPicture();

	//����ͼ��Ŀ�͸�
	UINT	GetWidth();
	UINT	GetHeight();

	//��ʼ����
	BOOL	Decode();
	LPCTSTR	GetWindowTitle(LPTSTR	lpszTitle = NULL, size_t bufSize = 0);

	//���豸����ʾͼ��
	BOOL	Paint(__int64 nXOriginDest, __int64 nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio = 1.00);

	//������ʾ��ͼƬ
	BOOL	Save(const WCHAR *lpszFilename);

//�̳��಻�ɷ���
private:

//�̳���ɷ���
protected:

	//Gdiplus
	GdiplusStartupInput m_oGdiPlusStartupInput;
	ULONG_PTR	m_pGdiPlusToken;

	//ͼƬ����dds,tga,other
	//enum { FMT_DDS, FMT_TGA, FMT_RAW} m_picFormat;
	PICFORMAT	m_picFormat;
	//������ļ��������Ը��ݺ�׺�ж�����
	LPCTSTR		m_lpszFileTitle;
	//�󶨵�������ʾ�Ĵ��ڵľ��
	HWND		m_hWndShow;
	//ͼƬ�Ŀ�͸�
	UINT		m_picWidth, m_picHeight;
	//��Ҫ��ʾ��ͼ��ử�����HDC����
	HDC			m_MemDC;
	//
	HBITMAP		m_MemBitmap;
	//�����ڴ�������ʾͼƬ��GDIPLUS����
	Image		*m_lpmyImage;
	//���ڽ�m_lpmyImage�е�ͼ�񻭵�m_MemDC��
	Graphics	*m_lpoGraph;
	//ԭʼ��ͼƬ�ļ�����
	//LPBYTE		m_resBuffer;
	//���ڲ�ʹ�õ�ԭʼ��ͼƬ�ļ����ݣ�������ΪRAWʱ��ָ��hGlobal���ڴ�
	LPBYTE		m_resBufferInClass;
	//������ʾbmp,jpg,png��ͼƬ��ʹ��Gdiplus��hGlobal�ڴ洴����IStream s��Ȼ��ͼ
	IStream*	m_stream;
	HGLOBAL		m_hGlobal;
	//������ļ������ݵĴ�С
	UINT32		m_bufsize;
	//��ʾ��ͼƬ�ľ����ʽ
	char		m_szPictureFormat[32];
	//�Ѿ������ͼƬ��DIB���ݣ�DDS��TGA��
	LPBYTE		m_DecodedDIBData;


	//�ڴ���������ʾ��͸�������ĸ���
	BOOL	DrawBlockOnDlg();
	//�����ļ����ж�����
	//

	//��m_lpmyImage�����ϴ���ͼ��image
	BOOL	BuildImage();
	//��Graphicsͼ���ϻ�ͼ
	BOOL	DrawPictureOnDC();
	//����Ŀ�괰�ڱ���
	BOOL	ShowTitleOnWindow();

	//�ж�����double���
	BOOL	isEqual(double d1, double d2);
};

