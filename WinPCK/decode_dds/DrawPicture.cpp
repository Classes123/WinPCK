#include <Windows.h>
#include <assert.h>
#include "ShowPicture.h"
#include "DdsTgaDecoder.h"


BOOL CShowPicture::BuildImage()
{
	//��dds header
	CDdsTgaDecoder cDdsTgaDecoder;
	switch(m_picFormat) {
	case FMT_DDS:

		if(!cDdsTgaDecoder.DecodeDDS(m_resBufferInClass, m_bufsize, m_DecodedDIBData, m_picWidth, m_picHeight, m_szPictureFormat)) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}
		{
			free(m_resBufferInClass);
			m_resBufferInClass = NULL;
			m_lpmyImage = new Bitmap(m_picWidth, m_picHeight, cDdsTgaDecoder.GetStride(), cDdsTgaDecoder.GetPixelFormat(), m_DecodedDIBData);
		}
		break;

	case FMT_TGA:

		if(!cDdsTgaDecoder.DecodeTGA(m_resBufferInClass, m_bufsize, m_DecodedDIBData, m_picWidth, m_picHeight, m_szPictureFormat)) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}

		free(m_resBufferInClass);
		m_resBufferInClass = NULL;

		m_lpmyImage = new Bitmap(m_picWidth, m_picHeight, cDdsTgaDecoder.GetStride(), cDdsTgaDecoder.GetPixelFormat(), m_DecodedDIBData);
		break;

	case FMT_RAW:	//bmp,jpg,png..

		CreateStreamOnHGlobal(m_hGlobal, FALSE, &m_stream);
		if(!m_stream) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}

		//show picture
		m_lpmyImage = new Image(m_stream);

		//����ͼƬ��С����ʾ���ڴ�С
		m_picWidth = m_lpmyImage->GetWidth();
		m_picHeight = m_lpmyImage->GetHeight();

		break;

	default:

		MessageBoxA(NULL, TEXT_NOT_DEFINE, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL CShowPicture::DrawPictureOnDC()
{
	DrawBlockOnDlg();

	m_lpoGraph = new Graphics(m_MemDC);

	m_lpoGraph->DrawImage(m_lpmyImage, 0, 0, m_picWidth, m_picHeight);

	if(NULL != m_lpoGraph)
		delete m_lpoGraph;
	m_lpoGraph = NULL;

	if(NULL != m_lpmyImage)
		delete m_lpmyImage;
	m_lpmyImage = NULL;

	//������Դ�������ǵ�FMT_RAW == iFormatʱִ�е�
	if (FMT_RAW == m_picFormat) {

		if (NULL != m_stream)
			m_stream->Release();
		m_stream = NULL;

		if (NULL != m_hGlobal)
			GlobalFree(m_hGlobal);
		m_hGlobal = NULL;
	}

	return TRUE;
}