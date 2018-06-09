#include "PckClassThreadHelper.h"

CRITICAL_SECTION	CPckClassThreadHelper::g_cs;
CRITICAL_SECTION	CPckClassThreadHelper::g_mt_threadID;
CRITICAL_SECTION	CPckClassThreadHelper::g_mt_nMallocBlocked;
CRITICAL_SECTION	CPckClassThreadHelper::g_mt_lpMaxMemory;
CRITICAL_SECTION	CPckClassThreadHelper::g_dwCompressedflag;
CRITICAL_SECTION	CPckClassThreadHelper::g_mt_pckCompressedDataPtrArrayPtr;

int					CPckClassThreadHelper::mt_threadID;		//�߳�ID

HANDLE				CPckClassThreadHelper::mt_evtAllWriteFinish;											//�߳��¼�
HANDLE				CPckClassThreadHelper::mt_evtAllCompressFinish;										//�߳��¼�
HANDLE				CPckClassThreadHelper::mt_evtMaxMemory;												//�߳��¼�
CMapViewFileRead	*CPckClassThreadHelper::mt_lpFileRead;													//ȫ�ֶ��ļ��ľ��,Ŀǰ��������ѹ��
CMapViewFileWrite	*CPckClassThreadHelper::mt_lpFileWrite;												//ȫ��д�ļ��ľ��
QWORD				CPckClassThreadHelper::mt_CompressTotalFileSize;										//Ԥ�Ƶ�ѹ���ļ���С
LPDWORD				CPckClassThreadHelper::mt_lpdwCount;													//�ӽ����̴߳��������ļ�����ָ��
DWORD				CPckClassThreadHelper::mt_dwFileCount;													//�ӽ����̴߳����������ļ�����ָ��
DWORD				CPckClassThreadHelper::mt_dwFileCountOfWriteTarget;									//д�����ݵ�Ŀ������,һ��=mt_dwFileCount�����ʱ=��ѹ��ʱ����Ч�ļ�����

#if USE_OLD_QUEUE
LPPCKINDEXTABLE_COMPRESS	CPckClassThreadHelper::mt_lpPckIndexTable;										//ѹ������ļ�������
BYTE			**	CPckClassThreadHelper::mt_pckCompressedDataPtrArray;									//�ڴ�����ָ�������

DWORD				CPckClassThreadHelper::mt_dwMaxQueueLength;											//������󳤶�
#ifdef _DEBUG
DWORD				CPckClassThreadHelper::mt_dwCurrentQueuePosPut;
DWORD				CPckClassThreadHelper::mt_dwCurrentQueuePosGet;				//��ǰ����λ��
#endif
DWORD				CPckClassThreadHelper::mt_dwCurrentQueueLength;										//��ǰ���п�ʹ�����ݵĸ���
BYTE			**	CPckClassThreadHelper::mt_pckCompressedDataPtrArrayGet;
BYTE			**	CPckClassThreadHelper::mt_pckCompressedDataPtrArrayPut;
LPPCKINDEXTABLE_COMPRESS	CPckClassThreadHelper::mt_lpPckIndexTablePut;
LPPCKINDEXTABLE_COMPRESS	CPckClassThreadHelper::mt_lpPckIndexTableGet;
#else
deque<LPPCKINDEXTABLE_COMPRESS>	CPckClassThreadHelper::mt_CompressedIndexs;
CPckClassIndexDataAppend *		CPckClassThreadHelper::mt_lpIndexData = NULL;
#endif
//QWORD				mt_dwAddress;
QWORD				CPckClassThreadHelper::mt_dwAddressQueue;												//ȫ��ѹ�����̵�д�ļ���λ�ã�ֻ��Queue����
QWORD				CPckClassThreadHelper::mt_dwAddressNameQueue;											//������pck�ļ���ѹ���ļ���������ʼλ��

BOOL				*CPckClassThreadHelper::mt_lpbThreadRunning;											//�߳��Ƿ�������״̬��ֵ����Ϊfalseʱ�߳��˳�
DWORD				*CPckClassThreadHelper::mt_lpMaxMemory;												//ѹ����������ʹ�õ�����ڴ棬����һ�����������õģ����ôӽ����̴߳�������ֵ������
DWORD				CPckClassThreadHelper::mt_MaxMemory;													//ѹ����������ʹ�õ�����ڴ�
int					CPckClassThreadHelper::mt_nMallocBlocked;												//�򻺴����걻��ͣ���߳���


																							//���ʱʹ�ñ���

char				CPckClassThreadHelper::mt_szCurrentNodeString[MAX_PATH_PCK_260];						//�������߳��е�ǰ��ʾ�ģ��ڵ��Ӧ��pck�е��ļ�·��
int					CPckClassThreadHelper::mt_nCurrentNodeStringLen;