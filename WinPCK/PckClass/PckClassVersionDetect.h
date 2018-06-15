//////////////////////////////////////////////////////////////////////
// PckClassVersionDetect.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// �汾��صļ���Լ����ݵ�д�롢��ȡ
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.6.27
//////////////////////////////////////////////////////////////////////
#pragma once
#include "PckClassBaseFeatures.h"

//PckVersion
#define PCK_VERSION_INVALID		(-1)
#define	PCK_VERSION_ZX			0
#define	PCK_VERSION_SDS			1
#define	PCK_VERSION_XAJH		2
#define	PCK_VERSION_ZXNEW		3
#define PCK_VERSION_SM			4
#define	PCK_VERSION_NUMS		5

class CPckClassVersionDetect :
	private virtual CPckClassBaseFeatures
{
public:
	CPckClassVersionDetect();
	~CPckClassVersionDetect();

	//���ð汾
	const	PCK_KEYS*	GetPckVersion();
	void	SetSavePckVersion(int verID);
	LPCTSTR	GetSaveDlgFilterString();

	//���pck�İ汾��д�뵽iDetectedPckID��ͬʱ��ȡ�ļ�ͷ��β
	BOOL	DetectPckVerion(LPCTSTR lpszPckFile);

private:

	const static PCK_KEYS			cPckKeys[PCK_VERSION_NUMS];
	const static PCK_VERSION_FUNC	cPckVersionFunc[PCK_VERSION_NUMS];

	TCHAR		szSaveDlgFilterString[1024];

	//�ļ�ͷ��β��������д������д��
	static LPVOID	FillHeadData_V2020(void *param);
	static LPVOID	FillHeadData_V2030(void *param);

	static LPVOID	FillTailData_V2020(void *param);
	static LPVOID	FillTailData_V2030(void *param);
	static LPVOID	FillTailData_V2031(void *param);

	static LPVOID FillIndexData_V2020(void *param, void *pckFileIndexBuf);
	static LPVOID FillIndexData_V2030(void *param, void *pckFileIndexBuf);
	static LPVOID FillIndexData_V2031(void *param, void *pckFileIndexBuf);

	//���ݴ�lpIndex -> param
	static BOOL PickIndexData_V2020(void *param, void* lpIndex);
	static BOOL PickIndexData_V2030(void *param, void* lpIndex);
	static BOOL PickIndexData_V2031(void *param, void* lpIndex);

	void	BuildSaveDlgFilterString();

	void	PrintInvalidVersionDebugInfo(LPCTSTR lpszPckFile);

	
};
