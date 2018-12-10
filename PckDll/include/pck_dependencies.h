#pragma once
#include "PckDefines.h"

//��ʾ�б�Ļص�����
typedef void(*SHOW_LIST_CALLBACK)(void*, int, const wchar_t *, int, unsigned __int64, unsigned __int64, void*);


//�򿪡��رա���ԭ���¼�ע��
#define PCK_FILE_OPEN_SUCESS		1
#define PCK_FILE_CLOSE				2
#define PCK_FILE_NEED_RESTORE		3

#define PCK_FEEDBACK_YES			1
#define PCK_FEEDBACK_NO				2
#define PCK_FEEDBACK_IGNORE			3

typedef int(*FeedbackCallback)(void* pTag, int eventId, WPARAM wParam, LPARAM lParam);

#if 0
typedef struct _RESTORE_INFOS
{
	BOOL			isValid;
	TCHAR			szFile[MAX_PATH];
	DWORD			dwMaxSinglePckSize;
	struct 
	{
		DWORD		dwHeadCheckHead;
		DWORD		dwPckSize;
		DWORD		dwHeadCheckTail;
	}Head;
	QWORD			dwAddressOfFilenameIndex;
	size_t			sizeOfIndexTailBuffer;
	LPBYTE			lpIndexTailBuffer;
}RESTORE_INFOS, *LPRESTORE_INFOS;
#endif

class CPckControlCenter;

typedef struct _PCK_VARIETY_PARAMS {

	DWORD		dwOldFileCount;
	DWORD		dwPrepareToAddFileCount;
	DWORD		dwChangedFileCount;
	DWORD		dwDuplicateFileCount;
	DWORD		dwFinalFileCount;

	//DWORD		dwUseNewDataAreaInDuplicateFileSize;
	DWORD		dwDataAreaSize;
	DWORD		dwRedundancyDataSize;

	LPCSTR		lpszAdditionalInfo;

	DWORD		dwUIProgress;
	DWORD		dwUIProgressUpper;

	DWORD		dwMTMemoryUsed;

	BOOL		bThreadRunning;
	BOOL		bForcedStopWorking;	//ǿ��ֹͣ
	int			errMessageNo;			//0 - ok

}PCK_VARIETY_PARAMS;

typedef struct _PCK_RUNTIME_PARAMS {

	PCK_VARIETY_PARAMS	cVarParams;

	DWORD		dwMTMaxMemory;		//���ʹ���ڴ�
	DWORD		dwMTThread;			//ѹ���߳���
	DWORD		dwCompressLevel;	//����ѹ����

	int			code_page;			//pck�ļ�ʹ�ñ���

	CPckControlCenter	*lpPckControlCenter;

}PCK_RUNTIME_PARAMS, *LPPCK_RUNTIME_PARAMS;


typedef struct _PCK_UNIFIED_FILEENTRY {
	int				entryType;
	wchar_t			szName[MAX_PATH_PCK_260];
}PCK_UNIFIED_FILE_ENTRY, *LPPCK_UNIFIED_FILE_ENTRY;
