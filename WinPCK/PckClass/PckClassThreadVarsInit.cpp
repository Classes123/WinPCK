#include "PckClassThread.h"

SRWLOCK				CPckClassThreadWorker::g_mt_LockCompressedflag;
SRWLOCK				CPckClassThreadWorker::g_mt_LockThreadID;
SRWLOCK				CPckClassThreadWorker::g_mt_LockReadFileMap;

int					CPckClassThreadWorker::mt_threadID;		//�߳�ID

HANDLE				CPckClassThreadWorker::mt_evtAllWriteFinish;											//�߳��¼�
CMapViewFileWrite	*CPckClassThreadWorker::mt_lpFileWrite;												//ȫ��д�ļ��ľ��
QWORD				CPckClassThreadWorker::mt_CompressTotalFileSize;										//Ԥ�Ƶ�ѹ���ļ���С
DWORD				CPckClassThreadWorker::mt_dwFileCountOfWriteTarget;									//д�����ݵ�Ŀ������,һ��=mt_dwFileCount�����ʱ=��ѹ��ʱ����Ч�ļ�����

#ifdef _DEBUG
DWORD				CPckClassThreadWorker::mt_dwCurrentQueuePosPut;
DWORD				CPckClassThreadWorker::mt_dwCurrentQueuePosGet;				//��ǰ����λ��
#endif

QWORD				CPckClassThreadWorker::mt_dwAddressQueue;												//ȫ��ѹ�����̵�д�ļ���λ�ã�ֻ��Queue����
QWORD				CPckClassThreadWorker::mt_dwAddressNameQueue;											//������pck�ļ���ѹ���ļ���������ʼλ��

