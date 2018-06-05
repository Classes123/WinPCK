
#define COMPRESSTHREADFUNC CompressThreadAdd
#define COMPRESSSINGLETHREADFUNC UpdatePckFileSingleThread
#define TARGET_PCK_MODE_COMPRESS PCK_MODE_COMPRESS_ADD
#include "PckClassThreadCompressFunctions.h"
#include "PckClassFileDisk.h"

//����pck��
BOOL CPckClass::UpdatePckFile(LPTSTR szPckFile, TCHAR(*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert)
{
	DWORD		dwFileCount = 0, dwOldPckFileCount;			//�ļ�����, ԭpck�ļ��е��ļ���
	QWORD		qwTotalFileSize = 0;						//δѹ��ʱ�����ļ���С
	size_t		nLen;

	char		szPathMbsc[MAX_PATH];

	int			level = m_lpPckParams->dwCompressLevel;
	int			threadnum = m_lpPckParams->dwMTThread;

	QWORD		dwAddress;

	//��ʼ�����ļ�
	LinkList<FILES_TO_COMPRESS> cFileLinkList;
	TCHAR(*lpszFilePathPtr)[MAX_PATH] = (TCHAR(*)[MAX_PATH])lpszFilePath;
	DWORD				dwAppendCount = nFileCount;
	LPPCK_PATH_NODE		lpNodeToInsertPtr;


	//���ò���
	if(m_ReadCompleted) {
		_tcscpy(szPckFile, m_PckAllInfo.szFilename);

		dwOldPckFileCount = m_PckAllInfo.dwFileCount;

		lpNodeToInsertPtr = lpNodeToInsert;

		//ȡ�õ�ǰ�ڵ�����·��
		if(!GetCurrentNodeString(mt_szCurrentNodeString, lpNodeToInsert)) {
			free(lpszFilePath);
			assert(FALSE);
			return FALSE;
		}

		mt_nCurrentNodeStringLen = strlen(mt_szCurrentNodeString);

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	} else {

		m_PckAllInfo.dwAddressOfFilenameIndex = PCK_DATA_START_AT;
		dwOldPckFileCount = 0;
		lpNodeToInsertPtr = m_PckAllInfo.lpRootNode->child;
		*mt_szCurrentNodeString = 0;
		mt_nCurrentNodeStringLen = 0;

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	}

	m_firstFile = cFileLinkList.first();

	//���ļ�
	CMapViewFileRead cFileRead;
	for(DWORD i = 0; i < dwAppendCount; i++) {

#ifdef UNICODE
		CUcs2Ansi cU2A;
		cU2A.GetString(*lpszFilePathPtr, szPathMbsc, MAX_PATH);
#else
		strcpy_s(szPathMbsc, *lpszFilePathPtr);
#endif
		nLen = (size_t)(strrchr(szPathMbsc, '\\') - szPathMbsc) + 1;

		if(FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributesA(szPathMbsc))) {
			//�ļ���
			NPckClassFileDisk::EnumFile(szPathMbsc, FALSE, dwFileCount, &cFileLinkList, qwTotalFileSize, nLen);
		} else {

			if(!cFileRead.Open(szPathMbsc)) {
				free(lpszFilePath);
				m_PckLog.PrintLogEL(TEXT_OPENNAME_FAIL, *lpszFilePathPtr, __FILE__, __FUNCTION__, __LINE__);

				assert(FALSE);
				return FALSE;
			}

			LPFILES_TO_COMPRESS	lpfirstFile = cFileLinkList.last();

			strcpy(lpfirstFile->szFilename, szPathMbsc);

			lpfirstFile->lpszFileTitle = lpfirstFile->szFilename + nLen;
			lpfirstFile->nBytesToCopy = MAX_PATH - nLen;

			qwTotalFileSize += (lpfirstFile->dwFileSize = cFileRead.GetFileSize());

			cFileLinkList.insertNext();

			cFileRead.clear();

			dwFileCount++;
		}

		lpszFilePathPtr++;
	}

	free(lpszFilePath);

	assert(0 != dwFileCount);

	if(0 == dwFileCount)return TRUE;

	//����˵����
	// mt_dwFileCount	��ӵ��ļ����������ظ�
	// dwAllCount		������ʾ������->lpPckParams->cVarParams.dwUIProgressUpper
	// dwFileCount		�������ʹ�ò�����������ļ�������н�ʹ�ô˲�����ʾ��ӵ��ļ������������ظ�
	// 

	//�ļ���д�봰�����б�������ʾ����
	DWORD dwPrepareToAdd = mt_dwFileCount = m_lpPckParams->cVarParams.dwUIProgressUpper = dwFileCount;

	//��������Ҫ���ռ�qwTotalFileSize
	mt_CompressTotalFileSize = NPckClassFileDisk::GetPckFilesizeByCompressed(szPckFile, qwTotalFileSize, m_PckAllInfo.qwPckSize/*m_PckAllInfo.dwAddressName*/);

	//if (PCK_SPACE_DETECT_SIZE >= mt_CompressTotalFileSize)mt_CompressTotalFileSize = PCK_STEP_ADD_SIZE;

	//��ԭ��Ŀ¼�е��ļ��Աȣ��Ƿ�������
	//���ԣ����������ǰ�				��������Ҷ�Ϊ�ļ����ļ��У��򸲸�
	//
	//����FindFileNode����-1�˳�������0����ʾֱ����ӣ���0�������ظ���
	//дר�õ�writethread��compressthread,�Ե���
	//��PCKINDEXTABLE_COMPRESS�����addר�����ԣ����ж��Ƿ����ô˽ڵ㣨����ʱ��0ʹ�ã�1��ʹ��
	//���� ʱʹ��2��ѭ��д��ѹ������ 

	//dwFileCount�����ڴ˴�ָ������ӵ��ļ���ȥ���������� 
	if(m_ReadCompleted) {
		LPFILES_TO_COMPRESS lpfirstFile = m_firstFile;
		while(NULL != lpfirstFile->next) {
			LPPCK_PATH_NODE lpDuplicateNode;
			lpDuplicateNode = m_classNode.FindFileNode(lpNodeToInsertPtr, lpfirstFile->lpszFileTitle);

			if(-1 == (int)lpDuplicateNode) {
				m_PckLog.PrintLogE(TEXT_ERROR_DUP_FOLDER_FILE);
				assert(FALSE);
				return FALSE;
			}

			if(NULL != lpDuplicateNode) {
				lpfirstFile->samePtr = lpDuplicateNode->lpPckIndexTable;
				dwFileCount--;
			}

			lpfirstFile = lpfirstFile->next;

		}
	}

	//��־
	m_PckLog.PrintLogI(TEXT_UPDATE_FILE_INFO, dwPrepareToAdd, mt_CompressTotalFileSize);

	PCK_ALL_INFOS	pckAllInfo;
	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)
	if(!BeforeSingleOrMultiThreadProcess(&pckAllInfo, mt_lpFileWrite, szPckFile, OPEN_ALWAYS, mt_CompressTotalFileSize, threadnum)) {
		assert(FALSE);
		return FALSE;
	}
	if(!initCompressedDataQueue(threadnum, mt_dwFileCountOfWriteTarget = mt_dwFileCount, dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex)) {
		delete mt_lpFileWrite;
		assert(FALSE);
		return FALSE;
	}


	MultiThreadInitialize(CompressThreadAdd, WriteThread, threadnum);
	dwAddress = mt_dwAddressQueue;

	//��ӡ�����ò���
	DWORD	dwUseNewDataAreaInDuplicateFile = 0;

	//д�ļ�����
	QWORD	dwAddressName = dwAddress;

	//����������ʾ���ļ����ȣ���ʼ������ʾд��������mt_hFileToWrite
	m_lpPckParams->cVarParams.dwUIProgress = 0;
	//dwAllCount = mt_dwFileCount + dwOldPckFileCount;	//������ļ���������������������Ӧ��������Ĺ�ʽ
	m_lpPckParams->cVarParams.dwUIProgressUpper = dwFileCount + dwOldPckFileCount;


	//дԭ�����ļ�
	LPPCKINDEXTABLE	lpPckIndexTableSrc = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < dwOldPckFileCount; i++) {

		PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;
		WritePckIndex(mt_lpFileWrite, FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableSrc->cFileIndex), dwAddress);
		lpPckIndexTableSrc++;

	}

	//д��ӵ��ļ�
	LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr = mt_lpPckIndexTable;

	mt_dwFileCount = dwFileCount = mt_dwFileCountOfWriteTarget;
	for(DWORD i = 0; i < mt_dwFileCountOfWriteTarget; i++) {
		////����lpPckFileIndex->dwAddressOffset

		if(!lpPckIndexTablePtr->bInvalid) {

			WritePckIndex(mt_lpFileWrite, lpPckIndexTablePtr, dwAddress);

		} else {

			assert(0 != dwFileCount);
			dwFileCount--;
		}

		if(0 == lpPckIndexTablePtr->dwAddressOfDuplicateOldDataArea)
			dwUseNewDataAreaInDuplicateFile++;

		lpPckIndexTablePtr++;

	}

	//pckTail.dwFileCount = dwFileCount + dwOldPckFileCount;//mt_dwFileCount��ʵ��д�������ظ����Ѿ��������ȥ��
	pckAllInfo.dwAddressOfFilenameIndex = dwAddressName;
	pckAllInfo.dwFileCount = dwFileCount + dwOldPckFileCount;
	AfterProcess(mt_lpFileWrite, pckAllInfo, dwAddress);

	delete mt_lpFileWrite;

	uninitCompressedDataQueue(threadnum);

	//���������´�һ�Σ�����ֱ�Ӵ򿪣��ɽ����߳����
	m_lpPckParams->cVarParams.dwOldFileCount = dwOldPckFileCount;
	m_lpPckParams->cVarParams.dwPrepareToAddFileCount = dwPrepareToAdd;
	m_lpPckParams->cVarParams.dwChangedFileCount = mt_dwFileCount;
	m_lpPckParams->cVarParams.dwDuplicateFileCount = mt_dwFileCount - dwFileCount;
	m_lpPckParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize = dwUseNewDataAreaInDuplicateFile;
	m_lpPckParams->cVarParams.dwFinalFileCount = pckAllInfo.dwFileCount;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}

