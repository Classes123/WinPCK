#pragma once
#include "ShowPicture.h"
class CShowPictureWithZoom :
	public CShowPicture
{
public:
	CShowPictureWithZoom(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCTSTR _lpszFileTitle, PICFORMAT _picFormat);
	virtual ~CShowPictureWithZoom();

	//�ָ�ԭʼ�ֱ���
	void	ZoomToOriginalResolution(__int64 pos_x, __int64 pos_y);

	void	ZoomWithMousePoint(UINT nFlags, short zDelta, __int64 pos_x, __int64 pos_y);

	//���ڴ�С�仯��Ҫ���¼���ֵ
	void	ChangeClientSize(WORD nWidth, WORD nHeight);
	double	CalcFixedRatioByClientSize(UINT uRealX, UINT uRealY, UINT uToX, UINT uToY);
	//���ڳ�ʼ��ʱ���������ֵ�����ͼƬ���ڷֱ��ʣ����ھ���󻯣�����ֵΪ�����Ƿ���Ҫ��󻯣���Ҫ����TRUE
	BOOL	CalcFixedRatioAndSizeOnInit(__int64 &_inout_cx, __int64 &_inout_cy, int _in_min_cx, int _in_min_cy);


	//����������
	double	GetZoomRatio();

	//�ƶ�ͼ��
	void	MovePicture(int xOffset, int yOffset);

	//��ʾ
	void	Paint();


protected:


	double		m_dZoomRatioShow;	//��ǰ������ʾ��������
	double		m_dZoomRatio;		//��ǰͼƬ�����ʣ���С��m_fixedZoomRatioʱm_dZoomRatioShow����m_fixedZoomRatio���������m_dZoomRatioShow = m_dZoomRatio
	//��Ӧ��ǰ���ڴ�С ��������
	double		m_fixedZoomRatio;


	//��ǰ��ʾ���꣬�Դ������Ͻ�Ϊ0,0
	__int64		m_ShowX, m_ShowY;
	//��ǰ��ʾ��С
	UINT64		m_ShowPicWidth;
	UINT64		m_ShowPicHeight;

	//�ڸ������������
	void	ZoomPictureAtPoint(double dZoomRatio, __int64 pos_x, __int64 pos_y);

	//������������m_fixedZoomRatio���µĵ�һ����ֵ
	void	CalcMinZoomRatio();

};
