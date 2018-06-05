//////////////////////////////////////////////////////////////////////
// ShowPictureWithZoom.cpp: ��ʾ���ŵ�dds��tgaͼ��
// �������dds��tgaͼ��������ź������ʾ
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include "ShowPictureWithZoom.h"

#define CHANGE_RATIO	1.2
#define MAX_ZOOM_RATIO	32.0

CShowPictureWithZoom::CShowPictureWithZoom(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat) : 
	CShowPicture( hWndShow, _buffer,  _bufsize, _lpszFileTitle, _picFormat),
	m_fixedZoomRatio(1.0),
	m_dZoomRatioShow(1.0),
	m_dZoomRatio(1.0),
	m_ShowX(0),
	m_ShowY(0)
{
}


CShowPictureWithZoom::~CShowPictureWithZoom()
{}

double CShowPictureWithZoom::GetZoomRatio()
{
	return m_dZoomRatioShow;
}

void CalcZoomPictureAtOneDimensional(int iClientPointXY, __int64 &_showXY, UINT64 &_picWidthHeight, double _dZoomRatio, UINT _RealPicWidthHeight, LONG	lClientWidthHeight)
{
	/*
	����m_ShowX,m_ShowY
	��ǰ��ʾ��С���ڴ���ʱ��
	�����Ե�ǰ���λ��Ϊ��������
	�������ĵ���ͼƬ�ϵ�λ��Ϊrx,ry, ����ڴ����ϵ�λ��Ϊmx,my��ͼƬ0,0�ڴ����ϵ���ʾ��Ϊsx,sy����rx=mx-sx
	�����Ե�ǰ���λ��Ϊ�������ţ��������ͼƬ�ϵĵ㲻������������ֵ��������x�����ϵ�����ֵΪp,�赱ǰͼƬ���Ϊsw����p=rx/sw=(mx-sx)/sw
	���ź�ͼƬ��ȱ仯Ϊsw1,��ʾ��仯Ϊsx1��p���䣬p=(mx-sx1)/sw1���õ�sx1 = mx - (mx-sx)*sw1/sw
	*/

	int mouseXY = iClientPointXY;
	//��������ͼ��֮��ʱ����ͼ��߽�Ϊ׼
	if(_showXY > mouseXY)mouseXY = _showXY;
	__int64 iShowRightBottom = _showXY + _picWidthHeight;
	if(iShowRightBottom < mouseXY)mouseXY = iShowRightBottom;

	//�������ź��m_ShowX, m_ShowY
	UINT64	dwShowPicWidthHeight = _dZoomRatio * _RealPicWidthHeight;

	//����ʾ��ͼƬ��ȴ��ڵ�ǰ����ʱ

	if(lClientWidthHeight < dwShowPicWidthHeight) {
		_showXY = mouseXY - (mouseXY - _showXY) * dwShowPicWidthHeight / _picWidthHeight;
		if(0 < _showXY)_showXY = 0;
	} else {

		_showXY = (lClientWidthHeight - dwShowPicWidthHeight) / 2;
	}

	_picWidthHeight = dwShowPicWidthHeight;
}

void CShowPictureWithZoom::ZoomPictureAtPoint(double dZoomRatio, __int64 pos_x, __int64 pos_y)
{

	//�ֱ����X��Y�������Ե�ǰ��������ŵĽ��

	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	CalcZoomPictureAtOneDimensional(pos_x, m_ShowX, m_ShowPicWidth, dZoomRatio, m_picWidth, rectWin.right);
	CalcZoomPictureAtOneDimensional(pos_y, m_ShowY, m_ShowPicHeight, dZoomRatio, m_picHeight, rectWin.bottom);

	Paint();
}

void CShowPictureWithZoom::ZoomWithMousePoint(UINT nFlags, short zDelta, __int64 pos_x, __int64 pos_y)
{

	//���Ŵ���Ϊ8������СΪ��Ӧ��ǰ���ڴ�С��1.0
	double dZoomChangeRatio = 1.0;
	double dNextZoomRatio;
	//�Ŵ�
	if(0 < zDelta) {

		dZoomChangeRatio *= (zDelta / 120.0 * CHANGE_RATIO);

		if((MAX_ZOOM_RATIO + EPSILON) < m_dZoomRatio)
			return;

		 dNextZoomRatio = m_dZoomRatio * dZoomChangeRatio;

		if(dNextZoomRatio > MAX_ZOOM_RATIO) {

			m_dZoomRatioShow = MAX_ZOOM_RATIO;
		} else {

			m_dZoomRatioShow = dNextZoomRatio;
		}

		//��С
	} else if(0 > zDelta) {
		dZoomChangeRatio /= (zDelta / (-120.0) * CHANGE_RATIO);

		//�������ŷ�Χ
		double dMinZoomRatio = (m_fixedZoomRatio > (1.0 - EPSILON)) ? 1.0 : m_fixedZoomRatio;

		if(dMinZoomRatio > (m_dZoomRatio + EPSILON))
			return;

		 dNextZoomRatio = m_dZoomRatio * dZoomChangeRatio;

		if(dNextZoomRatio < dMinZoomRatio) {

			m_dZoomRatioShow = dMinZoomRatio;
		} else {

			m_dZoomRatioShow = dNextZoomRatio;
		}

	}

	m_dZoomRatio = dNextZoomRatio;

	ZoomPictureAtPoint(m_dZoomRatioShow, pos_x, pos_y);
}

void CShowPictureWithZoom::ZoomToOriginalResolution(__int64 pos_x, __int64 pos_y)
{
	m_dZoomRatioShow = m_dZoomRatio = 1.0;
	ZoomPictureAtPoint(1.0, pos_x, pos_y);
}


void CShowPictureWithZoom::MovePicture(int xOffset, int yOffset)
{

	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	//����ʾ��ͼƬ��ȴ��ڵ�ǰ����ʱ
	if(rectWin.right < m_ShowPicWidth) {

		int min_x = (int)rectWin.right - (int)m_ShowPicWidth;

		//����ʾ������ڴ���ʱ��ͼƬ���Ͻǲ��ܴ���0
		//�������
		if(xOffset > 0) {

			if(min_x <= m_ShowX) {
				m_ShowX -= xOffset;
				if(m_ShowX < min_x) m_ShowX = min_x;
			}
		} else {

			if(0 >= m_ShowX) {
				m_ShowX -= xOffset;
				if(m_ShowX > 0)m_ShowX = 0;
			}
		}

	}

	//����ʾ��ͼƬ�߶ȴ��ڵ�ǰ����ʱ
	if(rectWin.bottom < m_ShowPicHeight) {

		int min_y = (int)rectWin.bottom - (int)m_ShowPicHeight;

		//������϶�
		if(yOffset > 0) {

			if(min_y <= m_ShowY) {
				m_ShowY -= yOffset;
				if(m_ShowY < min_y) m_ShowY = min_y;
			}
		} else {

			if(0 >= m_ShowY) {
				m_ShowY -= yOffset;
				if(m_ShowY > 0)m_ShowY = 0;
			}
		}

	}

	Paint();
}


void CShowPictureWithZoom::CalcMinZoomRatio()
{
	double dZoomRatio;

	for(dZoomRatio = 1.0; dZoomRatio >= m_dZoomRatioShow; dZoomRatio /= CHANGE_RATIO) {
	}

	m_dZoomRatio = dZoomRatio;
}

BOOL CShowPictureWithZoom::CalcFixedRatioAndSizeOnInit(__int64 &_inout_cx, __int64 &_inout_cy, int _in_min_cx, int _in_min_cy)
{

	//��ǰ�ֱ���
	const int width = GetSystemMetrics(SM_CXSCREEN);
	const int height = GetSystemMetrics(SM_CYSCREEN);

	double screenx_d_realx = width / (double)m_picWidth;
	double screeny_d_realy = height / (double)m_picHeight;

	BOOL bNeedShowMax = TRUE;
	//�Ƚ�screenx_d_realx��screeny_d_realy���ĸ�С˵���ĸ�����Խ��
	if(screenx_d_realx < screeny_d_realy) {

		if(width < m_picWidth) {
			m_dZoomRatio = screenx_d_realx;

		} else {
			bNeedShowMax = FALSE;

		}
	} else {

		if(height < m_picHeight) {
			m_dZoomRatio = screeny_d_realy;

		} else {
			bNeedShowMax = FALSE;

		}
	}
	m_dZoomRatioShow = m_dZoomRatio;

	if((1.0 - EPSILON) > m_dZoomRatioShow) {
		CalcMinZoomRatio();
	}


	m_ShowPicWidth = m_dZoomRatioShow * m_picWidth;
	m_ShowPicHeight = m_dZoomRatioShow * m_picHeight;

	if(bNeedShowMax) {
		return TRUE;
	} else {
		_inout_cx = (m_picWidth > _in_min_cx) ? m_picWidth + 10 : _in_min_cx + 10;
		_inout_cy = (m_picHeight > _in_min_cy) ? m_picHeight + 30 : _in_min_cy + 30;
		return FALSE;
	}

}



double CShowPictureWithZoom::CalcFixedRatioByClientSize(UINT uRealX, UINT uRealY, UINT uToX, UINT uToY)
{
	//����һ�¶�������õ�������
	double zoomx = uToX / (double)uRealX;
	double zoomy = uToX / (double)uRealX;

	return zoomx > zoomy ? zoomy : zoomx;
}

void CShowPictureWithZoom::ChangeClientSize(WORD nWidth, WORD nHeight)
{
	m_ShowX = ((int)nWidth - (int)m_ShowPicWidth) / 2;
	m_ShowY = ((int)nHeight - (int)m_ShowPicHeight) / 2;

	//����һ�¶�������õ�������
	m_fixedZoomRatio = CalcFixedRatioByClientSize(m_picWidth, m_picHeight, nWidth, nHeight);

	Paint();
}

void CShowPictureWithZoom::Paint()
{
	CShowPicture::Paint(m_ShowX, m_ShowY, 0, 0, m_dZoomRatioShow);

	//������Ч����
	//x
	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	if(m_ShowX > 0) {

		RECT rectInvalid = { 0, 0, m_ShowX, rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	if(m_ShowY > 0) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, 0, rectWin.right, m_ShowY };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	int iShowBottom = m_ShowY + m_ShowPicHeight;

	if(iShowBottom < rectWin.bottom) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, iShowBottom, rectWin.right, rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	int iShowRight = m_ShowX + m_ShowPicWidth;
	if(iShowRight < rectWin.right) {

		RECT rectInvalid = { iShowRight, m_ShowY >0 ? m_ShowY : 0,rectWin.right, iShowBottom < rectWin.bottom ? iShowBottom : rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}
}