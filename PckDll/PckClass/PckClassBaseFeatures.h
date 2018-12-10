#pragma once
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include "PckHeader.h"
#include "PckClassLog.h"

#include <vector>
using namespace std;

class CPckClassBaseFeatures 
{
public:
	CPckClassBaseFeatures();
	~CPckClassBaseFeatures();

	virtual CONST	LPPCKINDEXTABLE		GetPckIndexTable();
	virtual CONST	LPPCK_PATH_NODE		GetPckPathNode();

	//�Ƿ��Ѿ������ļ�
	BOOL	isFileLoaded();

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

	//���ý���
	void	SetParams_ProgressInc();
	void	SetParams_Progress(DWORD dwUIProgres);

	//���ý�����Ϣ�е����ֵ
	void	SetParams_ProgressUpper(DWORD dwUIProgressUpper, BOOL bReset = TRUE);
	void	AddParams_ProgressUpper(DWORD dwUIProgressUpperAdd);

	//���̴߳������
	void	SetThreadFlag(BOOL isThreadWorking);
	//BOOL	CheckIfNeedForcedStopWorking();
	void	SetErrMsgFlag(int errMsg);

#if _DEBUG
#define _USELOGFILE 1
	static SRWLOCK	m_LockLogFile;
	static int logOutput(const char *file, const char *format, ...);
#else
#define _USELOGFILE 0
	static void logOutput(...){}
#endif

	//��ӡ��־
	CPckClassLog			m_PckLog;
	PCK_ALL_INFOS			m_PckAllInfo;

	//����ʱ���� 
	LPPCK_RUNTIME_PARAMS	m_lpPckParams;

};
