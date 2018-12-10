//////////////////////////////////////////////////////////////////////
// PckControlCenterParams.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include <thread>

#pragma region �߳̿���

//�߳����в���
BOOL CPckControlCenter::isThreadWorking()
{
	return cParams.cVarParams.bThreadRunning;
}

void CPckControlCenter::ForceBreakThreadWorking()
{
	cParams.cVarParams.bForcedStopWorking = TRUE;
}

#pragma endregion


#pragma region �߳�������Ϣ
//
int CPckControlCenter::GetLastErrorMsg()
{
	return cParams.cVarParams.errMessageNo;
}

BOOL CPckControlCenter::isLastOptSuccess()
{
	return (PCK_OK == GetLastErrorMsg());
}

#pragma endregion

#pragma region �ڴ�ռ��
//�ڴ�ռ��
DWORD CPckControlCenter::getMTMemoryUsed()
{
	return cParams.cVarParams.dwMTMemoryUsed;
}

//�ڴ�ֵ
void CPckControlCenter::setMTMaxMemory(DWORD dwMTMaxMemory)
{
	if ((0 < dwMTMaxMemory) && (getMaxMemoryAllowed() >= dwMTMaxMemory)) {
		cParams.dwMTMaxMemory = dwMTMaxMemory;
	}
	else {
		cParams.dwMTMaxMemory = getMaxMemoryAllowed();
	}
}

DWORD CPckControlCenter::getMTMaxMemory()
{
	return cParams.dwMTMaxMemory;
}

//����ڴ�
DWORD CPckControlCenter::getMaxMemoryAllowed()
{
	return MT_MAX_MEMORY;
}

#pragma endregion

#pragma region �߳���


DWORD CPckControlCenter::getMaxThread()
{
	return cParams.dwMTThread;
}

void CPckControlCenter::setMaxThread(DWORD dwThread)
{
	cParams.dwMTThread = dwThread;
}

//�߳�Ĭ�ϲ���
DWORD CPckControlCenter::getMaxThreadUpperLimit()
{
	return (thread::hardware_concurrency() + ((thread::hardware_concurrency() + (thread::hardware_concurrency() & 1)) >> 1));
}

#pragma endregion

#pragma region ѹ���ȼ�

//ѹ���ȼ�
DWORD CPckControlCenter::getCompressLevel()
{
	return cParams.dwCompressLevel;
}

void CPckControlCenter::setCompressLevel(DWORD dwCompressLevel)
{
	cParams.dwCompressLevel = dwCompressLevel;
}

//ѹ���ȼ�Ĭ�ϲ���
//����Ĭ��ֵ
DWORD CPckControlCenter::getDefaultCompressLevel()
{
	return Z_DEFAULT_COMPRESS_LEVEL;
}

DWORD CPckControlCenter::getMaxCompressLevel()
{
	return MAX_COMPRESS_LEVEL;
}

#pragma endregion


#pragma region �������

DWORD CPckControlCenter::getUIProgress()
{
	return cParams.cVarParams.dwUIProgress;
}

void CPckControlCenter::setUIProgress(DWORD dwUIProgress)
{
	cParams.cVarParams.dwUIProgress = dwUIProgress;
}

DWORD CPckControlCenter::getUIProgressUpper()
{
	return cParams.cVarParams.dwUIProgressUpper;
}

//void CPckControlCenter::setUIProgressUpper(DWORD dwUIProgressUpper)
//{
//	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
//}

#pragma endregion

#pragma region ��ӡ���/�����ļ����

DWORD CPckControlCenter::GetUpdateResult_OldFileCount()
{
	return cParams.cVarParams.dwOldFileCount;
}

DWORD CPckControlCenter::GetUpdateResult_PrepareToAddFileCount()
{
	return cParams.cVarParams.dwPrepareToAddFileCount;
}

DWORD CPckControlCenter::GetUpdateResult_ChangedFileCount()
{
	return cParams.cVarParams.dwChangedFileCount;
}

DWORD CPckControlCenter::GetUpdateResult_DuplicateFileCount()
{
	return cParams.cVarParams.dwDuplicateFileCount;
}

DWORD CPckControlCenter::GetUpdateResult_FinalFileCount()
{
	return cParams.cVarParams.dwFinalFileCount;
}

#pragma endregion
