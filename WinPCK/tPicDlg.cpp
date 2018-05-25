//////////////////////////////////////////////////////////////////////
// tPicDlg.cpp: WinPCK �����̲߳���
// �Ի������
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4018 )

#include <assert.h>
#include "PreviewDlg.h"
#include <stdio.h>
#include <tchar.h>
#include "OpenSaveDlg.h"
#include "CharsCodeConv.h"

TPicDlg::TPicDlg(LPBYTE *_buf, UINT32 _dwSize, PICFORMAT _iFormat, const TCHAR *_lpszFile, TWin *_win) :
	TDlg(IDD_DIALOG_PIC, _win),
	lpShowPicture(NULL),
	isMouseDown(FALSE),
	buf(_buf),
	dwSize(_dwSize),
	lpszFile(_lpszFile),
	iFormat(_iFormat),
	m_dZoomRatio(1.0),
	m_ShowX(0),
	m_ShowY(0),
	m_ShowPicWidth(0),
	m_ShowPicHeight(0)
{ }

TPicDlg::~TPicDlg()
{
	if (NULL != lpShowPicture)
		delete lpShowPicture;
}

BOOL TPicDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
	case IDCANCEL:

		EndDialog(wID);
		return	TRUE;

	}
	return	FALSE;
}

BOOL TPicDlg::SaveFile()
{
	//����
	if(FMT_RAW != iFormat) {
		WCHAR	szFilename[MAX_PATH];
		::GetCurrentDirectoryW(MAX_PATH, szFilename);

		WCHAR *lpTail = szFilename + wcslen(szFilename) - 1;
		if('\\' == *lpTail) {
			*lpTail = 0;
		}

		wcscat_s(szFilename, MAX_PATH, L"\\");
#ifdef UNICODE
		wcscat_s(szFilename, MAX_PATH, lpszFile);
#else
		CAnsi2Ucs cA2U;
		wcscat_s(szFilename, MAX_PATH, cA2U.GetString(lpszFile));
#endif
		wcscpy(wcsrchr(szFilename, L'.'), L".png\0\0");

		if(::SaveFile(hWnd, szFilename, L"png", TEXT_SAVE_FILTER)) {

			return lpShowPicture->Save(szFilename);
		}
	}
	
	return TRUE;
}

double TPicDlg::CalcFixedRatio(UINT uRealX, UINT uRealY, UINT uToX, UINT uToY)
{
	//����һ�¶�������õ�������
	 double zoomx = uToX / (double)uRealX;
	 double zoomy = uToX / (double)uRealX;

	 return zoomx > zoomy ? zoomy : zoomx;
}

void TPicDlg::InitFixedShowPositionAndShowWindow()
{
	//RECT	rectWin;

	//��ǰ�ֱ���
	const int width = GetSystemMetrics(SM_CXSCREEN);
	const int height = GetSystemMetrics(SM_CYSCREEN);

	double screenx_d_realx = width / (double)m_RealPicWidth;
	double screeny_d_realy = height / (double)m_RealPicHeight;

	BOOL bNeedShowMax = TRUE;
	//�Ƚ�screenx_d_realx��screeny_d_realy���ĸ�С˵���ĸ�����Խ��
	if(screenx_d_realx < screeny_d_realy) {

		if(width < m_RealPicWidth) {
			m_dZoomRatio = screenx_d_realx;

		} else {
			bNeedShowMax = FALSE;
			
		}
	} else {

		if(height < m_RealPicHeight) {
			m_dZoomRatio = screeny_d_realy;
			
		} else {
			bNeedShowMax = FALSE;

		}
	}
	m_ShowPicWidth = m_dZoomRatio * m_RealPicWidth;
	m_ShowPicHeight = m_dZoomRatio * m_RealPicHeight;

	FreshWindowTitle();

	if(bNeedShowMax) {
		ShowWindow(SW_SHOWMAXIMIZED);
	} else {
		::SetWindowPos(hWnd, NULL, 0, 0, m_RealPicWidth + 10, m_RealPicHeight + 30, SWP_NOZORDER | SWP_NOMOVE);
		//��ʾ����
		Show();
	}
}

void TPicDlg::FreshWindowTitle()
{
	TCHAR szTitle[MAX_PATH];
	_stprintf_s(szTitle, TEXT("%s (%d%%)"), m_szTitle, (int)(m_dZoomRatio * 100.0 + 0.5));
	SetWindowText(szTitle);
}

BOOL TPicDlg::EvCreate(LPARAM lParam)
{
	//HGLOBAL		hG;
	//����static��С
	RECT	rectWin;
	GetClientRect(hWnd, &rectWin);
	//::SetWindowPos(GetDlgItem(IDC_STATIC_PIC), NULL, 0, 0, rectWin.right, rectWin.bottom, SWP_NOZORDER | SWP_NOMOVE);

	lpShowPicture = new CShowPicture(hWnd, *buf, dwSize, lpszFile, iFormat);

	if (!lpShowPicture->Decode())
		return FALSE;

	lpShowPicture->GetWindowTitle(m_szTitle, sizeof(m_szTitle) / sizeof(TCHAR));

	m_RealPicWidth = lpShowPicture->GetWidth();
	m_RealPicHeight = lpShowPicture->GetHeight();

	InitFixedShowPositionAndShowWindow();

	return	FALSE;
}


BOOL TPicDlg::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	m_ShowX = ((int)nWidth - (int)m_ShowPicWidth) / 2;
	m_ShowY = ((int)nHeight - (int)m_ShowPicHeight) / 2;

	//����һ�¶�������õ�������
	m_fixedZoomRatio = CalcFixedRatio(m_RealPicWidth, m_RealPicHeight, nWidth, nHeight);

	DebugA("set1 nWidth = %d, m_ShowPicWidth = %d\r\n", nWidth, m_ShowPicWidth);
	DebugA("set1 m_ShowX = %d, m_ShowY = %d\r\n", m_ShowX, m_ShowY);

	EvPaint();
	return	FALSE;

}

BOOL TPicDlg::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	switch(uMsg) {
	case WM_LBUTTONUP:
		//ReleaseCapture();
		isMouseDown = FALSE;
		break;

	case WM_LBUTTONDOWN:

		isMouseDown = TRUE;
		//SetCapture();
		GetCursorPos(&pointMouse);

		break;

	case WM_RBUTTONDOWN:

		ZoomPictureAtPoint(1.0, &pos);

		break;
	case WM_LBUTTONDBLCLK:
		//����
		this->SaveFile();
		break;

	}

	return FALSE;
}

#define CHANGE_RATIO	1.2
#define MAX_ZOOM_RATIO	32.0

void TPicDlg::ZoomPictureAtPoint(double dZoomRatio, POINTS *pos)
{
	POINTS thispos;
	if(NULL == pos) {
		GetCursorPos(&pointMouse);
		thispos.x = pointMouse.x;
		thispos.y = pointMouse.y;
		pos = &thispos;
	}

	m_dZoomRatio = dZoomRatio;

	/*
	����m_ShowX,m_ShowY
	��ǰ��ʾ��С���ڴ���ʱ��
	�����Ե�ǰ���λ��Ϊ��������
	�������ĵ���ͼƬ�ϵ�λ��Ϊrx,ry, ����ڴ����ϵ�λ��Ϊmx,my��ͼƬ0,0�ڴ����ϵ���ʾ��Ϊsx,sy����rx=mx-sx
	�����Ե�ǰ���λ��Ϊ�������ţ��������ͼƬ�ϵĵ㲻������������ֵ��������x�����ϵ�����ֵΪp,�赱ǰͼƬ���Ϊsw����p=rx/sw=(mx-sx)/sw
	���ź�ͼƬ��ȱ仯Ϊsw1,��ʾ��仯Ϊsx1��p���䣬p=(mx-sx1)/sw1���õ�sx1 = mx - (mx-sx)*sw1/sw
	*/
	POINT pointMouse = { pos->x , pos->y };
	ScreenToClient(hWnd, &pointMouse);
	int mouseX = pointMouse.x, mouseY = pointMouse.y;
	//��������ͼ��֮��ʱ����ͼ��߽�Ϊ׼
	if(m_ShowX > mouseX)mouseX = m_ShowX;
	if(m_ShowY > mouseY)mouseY = m_ShowY;
	int iShowRight = m_ShowX + m_ShowPicWidth;
	int iShowBottom = m_ShowY + m_ShowPicHeight;
	if(iShowRight < mouseX)mouseX = iShowRight;
	if(iShowBottom < mouseY)mouseY = iShowBottom;

	//�������ź��m_ShowX, m_ShowY
	UINT	dwShowPicWidth = m_dZoomRatio * m_RealPicWidth;
	UINT	dwShowPicHeight = m_dZoomRatio * m_RealPicHeight;

	//����ʾ��ͼƬ��ȴ��ڵ�ǰ����ʱ
	RECT	rectWin;
	GetClientRect(hWnd, &rectWin);

	if(rectWin.right < dwShowPicWidth) {
		m_ShowX = mouseX - (mouseX - m_ShowX) * dwShowPicWidth / m_ShowPicWidth;
		if(0 < m_ShowX)m_ShowX = 0;
	} else {

		m_ShowX = (rectWin.right - dwShowPicWidth) / 2;
	}

	//����ʾ��ͼƬ�߶ȴ��ڵ�ǰ����ʱ
	if(rectWin.bottom < dwShowPicHeight) {
		m_ShowY = mouseY - (mouseY - m_ShowY) * dwShowPicHeight / m_ShowPicHeight;
		if(0 < m_ShowY)m_ShowY = 0;
	} else {

		m_ShowY = (rectWin.bottom - dwShowPicHeight) / 2;
	}

	m_ShowPicWidth = dwShowPicWidth;
	m_ShowPicHeight = dwShowPicHeight;

	EvPaint();

	FreshWindowTitle();

}

BOOL TPicDlg::EvMouseWheel(UINT nFlags, short zDelta, POINTS pos)
{

	DebugA("%s: nFlags = %x, zDelta = %d\r\n", __FUNCTION__, nFlags, zDelta);
	//���Ŵ���Ϊ8������СΪ��Ӧ��ǰ���ڴ�С
	double dZoomChangeRatio = 1.0;
	//�Ŵ�
	if(0 < zDelta) {

		dZoomChangeRatio *= (zDelta / 120.0 * CHANGE_RATIO);
	} else if(0 > zDelta)
	{
		dZoomChangeRatio /= (zDelta / (-120.0) * CHANGE_RATIO);
	}

	//�������ŷ�Χ
	double dMinZoomRatio = (m_fixedZoomRatio > 0.99) ? 1.0 : m_fixedZoomRatio;

	if(((MAX_ZOOM_RATIO + 0.1) < m_dZoomRatio) || (dMinZoomRatio > (m_dZoomRatio + 0.1)))
		return FALSE;

	m_dZoomRatio *= dZoomChangeRatio;
	if(m_dZoomRatio > MAX_ZOOM_RATIO)m_dZoomRatio = MAX_ZOOM_RATIO;
	if(m_dZoomRatio < dMinZoomRatio) m_dZoomRatio = dMinZoomRatio;

	ZoomPictureAtPoint(m_dZoomRatio, &pos);

	return FALSE;
}

BOOL TPicDlg::EvMouseMove(UINT fwKeys, POINTS pos)
{

	if(isMouseDown) {
		POINT	pointLast = pointMouse;
		GetCursorPos(&pointMouse);

		int		xOffset = pointLast.x - pointMouse.x;
		int		yOffset = pointLast.y - pointMouse.y;

		RECT	rectWin;
		GetClientRect(hWnd, &rectWin);

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
		DebugA("set2 m_ShowX = %d, m_ShowY = %d\r\n", m_ShowX, m_ShowY);

		EvPaint();
	}

	return FALSE;
}

BOOL TPicDlg::EvPaint(void)
{
	DebugA("m_ShowX = %d, m_ShowY = %d\r\n", m_ShowX, m_ShowY);
	lpShowPicture->Paint(m_ShowX, m_ShowY, 0, 0, m_dZoomRatio);


	//������Ч����
	//x
	RECT	rectWin;
	GetClientRect(hWnd, &rectWin);

	if(m_ShowX > 0) {

		RECT rectInvalid = { 0, 0, m_ShowX, rectWin.bottom };
		InvalidateRect(hWnd, &rectInvalid, TRUE);
	}

	if(m_ShowY > 0) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, 0, rectWin.right, m_ShowY };
		InvalidateRect(hWnd, &rectInvalid, TRUE);
	}

	int iShowBottom = m_ShowY + m_ShowPicHeight;

	if(iShowBottom < rectWin.bottom) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, iShowBottom, rectWin.right, rectWin.bottom };
		InvalidateRect(hWnd, &rectInvalid, TRUE);
	}

	int iShowRight = m_ShowX + m_ShowPicWidth;
	if(iShowRight < rectWin.right) {

		RECT rectInvalid = { iShowRight, m_ShowY >0 ? m_ShowY : 0,rectWin.right, iShowBottom < rectWin.bottom ? iShowBottom : rectWin.bottom };
		InvalidateRect(hWnd, &rectInvalid, TRUE);
	}


	return TRUE;
}
