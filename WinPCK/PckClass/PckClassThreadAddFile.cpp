
#define COMPRESSTHREADFUNC CompressThreadAdd
#define COMPRESSSINGLETHREADFUNC UpdatePckFileSingleThread
#define TARGET_PCK_MODE_COMPRESS PCK_MODE_COMPRESS_ADD
#include "PckClassThreadCompressFunctions.h"


//����pck��
BOOL CPckClass::UpdatePckFile(LPTSTR szPckFile, TCHAR(*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert)
{
	DWORD		dwFileCount = 0, dwOldPckFileCount;					//�ļ�����, ԭpck�ļ��е��ļ���
	QWORD		qwTotalFileSize = 0, qwTotalFileSizeTemp;			//δѹ��ʱ�����ļ���С
	size_t		nLen;

	char		szPathMbsc[MAX_PATH];
	char		szLogString[LOG_BUFFER];

	int			level = lpPckParams->dwCompressLevel;
	int			threadnum = lpPckParams->dwMTThread;

	QWORD		dwAddress;

	//��ʼ�����ļ�
	LPFILES_TO_COMPRESS			lpfirstFile;
	TCHAR(*lpszFilePathPtr)[MAX_PATH] = (TCHAR(*)[MAX_PATH])lpszFilePath;
	DWORD				dwAppendCount = nFileCount;
	LPPCK_PATH_NODE		lpNodeToInsertPtr;


	//���ò���
	if(m_ReadCompleted) {
		lstrcpy(szPckFile, m_PckAllInfo.szFilename);

		dwOldPckFileCount = m_PckAllInfo.dwFileCount;

		lpNodeToInsertPtr = lpNodeToInsert;

		//ȡ�õ�ǰ�ڵ�����·��
		if(!GetCurrentNodeString(mt_szCurrentNodeString, lpNodeToInsert)) {
			free(lpszFilePath);
			assert(FALSE);
			return FALSE;
		}

		mt_nCurrentNodeStringLen = strlen(mt_szCurrentNodeString);

		sprintf_s(szLogString, TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);
		PrintLogI(szLogString);

	} else {

		m_PckAllInfo.dwAddressName = PCK_DATA_START_AT;

		dwOldPckFileCount = 0;

		lpNodeToInsertPtr = m_RootNode.child;

		*mt_szCurrentNodeString = 0;

		mt_nCurrentNodeStringLen = 0;

		sprintf_s(szLogString, TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);
		PrintLogI(szLogString);

	}

	if(NULL == m_firstFile)m_firstFile = (LPFILES_TO_COMPRESS)AllocMemory(sizeof(FILES_TO_COMPRESS));
	if(NULL == m_firstFile) {

		PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		free(lpszFilePath);
		assert(FALSE);
		return FALSE;
	}

	lpfirstFile = m_firstFile;

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
			EnumFile(szPathMbsc, FALSE, dwFileCount, lpfirstFile, qwTotalFileSize, nLen);
		} else {

			if(!cFileRead.Open(szPathMbsc)) {
				DeallocateFileinfo();
				free(lpszFilePath);
				PrintLogEL(TEXT_OPENNAME_FAIL, *lpszFilePathPtr, __FILE__, __FUNCTION__, __LINE__);

				assert(FALSE);
				return FALSE;
			}

			strcpy(lpfirstFile->szFilename, szPathMbsc);

			lpfirstFile->lpszFileTitle = lpfirstFile->szFilename + nLen;
			lpfirstFile->nBytesToCopy = MAX_PATH - nLen;

			qwTotalFileSize += (lpfirstFile->dwFileSize = cFileRead.GetFileSize());

			lpfirstFile->next = (LPFILES_TO_COMPRESS)AllocMemory(sizeof(FILES_TO_COMPRESS));
			lpfirstFile = lpfirstFile->next;

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
	DWORD dwPrepareToAdd = mt_dwFileCount = lpPckParams->cVarParams.dwUIProgressUpper = dwFileCount;

	//��������Ҫ���ռ�qwTotalFileSize
	qwTotalFileSizeTemp = qwTotalFileSize * 0.6 + m_PckAllInfo.dwAddressName + PCK_SPACE_DETECT_SIZE;

#if !PCK_SIZE_UNLIMITED
	if(((0 != (qwTotalFileSizeTemp >> 32)) && (0x20002 == m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->Version)) || \
		((0 != (qwTotalFileSizeTemp >> 33)) && (0x20003 == m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->Version))) {

		PrintLogE(TEXT_COMPFILE_TOOBIG, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}
#endif
	mt_CompressTotalFileSize = qwTotalFileSizeTemp;

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
		lpfirstFile = m_firstFile;
		while(NULL != lpfirstFile->next) {
			LPPCK_PATH_NODE lpDuplicateNode;
			lpDuplicateNode = FindFileNode(lpNodeToInsertPtr, lpfirstFile->lpszFileTitle);

			if(-1 == (int)lpDuplicateNode) {
				DeallocateFileinfo();
				PrintLogE(TEXT_ERROR_DUP_FOLDER_FILE);
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
	sprintf_s(szLogString, TEXT_UPDATE_FILE_INFO, dwPrepareToAdd, mt_CompressTotalFileSize);
	PrintLogI(szLogString);

	PCK_ALL_INFOS	pckAllInfo;
	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)
	if(!BeforeSingleOrMultiThreadProcess(&pckAllInfo, mt_lpFileWrite, szPckFile, OPEN_ALWAYS, mt_CompressTotalFileSize, threadnum)) {
		DeallocateFileinfo();
		//PrintLogE(TEXT_ERROR_DUP_FOLDER_FILE);
		assert(FALSE);
		return FALSE;
	}
	if(!initCompressedDataQueue(threadnum, mt_dwFileCountOfWriteTarget = mt_dwFileCount, dwAddress = m_PckAllInfo.dwAddressName)) {
		delete mt_lpFileWrite;
		DeallocateFileinfo();
		//PrintLogE(TEXT_ERROR_DUP_FOLDER_FILE);
		assert(FALSE);
		return FALSE;
	}

	if(PCK_COMPRESS_NEED_ST < threadnum) {

		MultiThreadInitialize(CompressThreadAdd, WriteThread, threadnum);
		dwAddress = mt_dwAddressQueue;
	}
#if PCK_COMPRESS_NEED_ST
	else {
		UpdatePckFileSingleThread(dwAddress);
	}
#endif
	//��ӡ�����ò���
	DWORD	dwUseNewDataAreaInDuplicateFile = 0;

	//д�ļ�����
	QWORD	dwAddressName = dwAddress;

	//����������ʾ���ļ����ȣ���ʼ������ʾд��������mt_hFileToWrite
	lpPckParams->cVarParams.dwUIProgress = 0;
	//dwAllCount = mt_dwFileCount + dwOldPckFileCount;	//������ļ���������������������Ӧ��������Ĺ�ʽ
	lpPckParams->cVarParams.dwUIProgressUpper = dwFileCount + dwOldPckFileCount;


	//дԭ�����ļ�
	LPPCKINDEXTABLE	lpPckIndexTableSrc = m_lpPckIndexTable;

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
	pckAllInfo.dwAddressName = dwAddressName;
	pckAllInfo.dwFileCount = dwFileCount + dwOldPckFileCount;
	AfterProcess(mt_lpFileWrite, pckAllInfo, dwAddress);

	delete mt_lpFileWrite;
	DeallocateFileinfo();

	uninitCompressedDataQueue(threadnum);

	//���������´�һ�Σ�����ֱ�Ӵ򿪣��ɽ����߳����
	lpPckParams->cVarParams.dwOldFileCount = dwOldPckFileCount;
	lpPckParams->cVarParams.dwPrepareToAddFileCount = dwPrepareToAdd;
	lpPckParams->cVarParams.dwChangedFileCount = mt_dwFileCount;
	lpPckParams->cVarParams.dwDuplicateFileCount = mt_dwFileCount - dwFileCount;
	lpPckParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize = dwUseNewDataAreaInDuplicateFile;
	lpPckParams->cVarParams.dwFinalFileCount = pckAllInfo.dwFileCount;

	PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}

