#pragma once
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include "PckHeader.h"
#include "PckClassLog.h"

#include <vector>
using namespace std;

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

class CPckClassBaseFeatures 
{
public:
	CPckClassBaseFeatures();
	~CPckClassBaseFeatures();

	virtual CONST	LPPCKINDEXTABLE		GetPckIndexTable();
	virtual CONST	LPPCK_PATH_NODE		GetPckPathNode();

	//�ļ���С
	QWORD	GetPckSize();

	//��ȡ�ļ���
	DWORD	GetPckFileCount();

	//��������С
	QWORD	GetPckDataAreaSize();

	//�������������ݴ�С
	QWORD	GetPckRedundancyDataSize();

protected:
	//������Ϣ
	void	ResetPckInfos();

	//���ý�����Ϣ�е����ֵ
	void	SetParams_ProgressUpper(DWORD dwUIProgressUpper);


#ifdef _DEBUG
	static char *formatString(const char *format, ...);
	static int logOutput(const char *file, const char *text);
#endif
	//��Ӵ�ѹ���ļ�ʱ�������ļ�ʱʹ�õ��ļ��б�ĵ�һ���ļ�
	LPFILES_TO_COMPRESS		m_firstFile;

	//��ӡ��־
	CPckClassLog			m_PckLog;
	PCK_ALL_INFOS			m_PckAllInfo;
	//����ʱ���� 
	LPPCK_RUNTIME_PARAMS	m_lpPckParams;
};
