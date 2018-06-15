
#include "PckClassWriteOperator.h"

//�½�������pck��
BOOL CPckClassWriteOperator::UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert)
{
	DWORD		dwNewFileCount = 0;			//�ļ�����, ԭpck�ļ��е��ļ���
	DWORD		dwDuplicateFileCount = 0;
	QWORD		qwTotalNewFileSize = 0;						//δѹ��ʱ�����ļ���С

	int			level = m_lpPckParams->dwCompressLevel;
	int			threadnum = m_lpPckParams->dwMTThread;

	QWORD		dwAddress;
	THREAD_PARAMS		cThreadParams;

	//��ʼ�����ļ�
	//LinkList<FILES_TO_COMPRESS> cFileLinkList;
	LPPCK_PATH_NODE		lpNodeToInsertPtr;

#pragma region ���ò���
	
	m_PckAllInfo.lpFilesToBeAdded = &m_FilesToBeAdded;

	if(m_PckAllInfo.isPckFileLoaded) {

		lpNodeToInsertPtr = lpNodeToInsert;

		//ȡ�õ�ǰ�ڵ�����·��
		if(!GetCurrentNodeString(mt_szCurrentNodeString, lpNodeToInsert)) {
			assert(FALSE);
			return FALSE;
		}

		mt_nCurrentNodeStringLen = strlen(mt_szCurrentNodeString);

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	} else {

		m_PckAllInfo.dwAddressOfFilenameIndex = PCK_DATA_START_AT;
		lpNodeToInsertPtr = m_PckAllInfo.cRootNode.child;
		*mt_szCurrentNodeString = 0;
		mt_nCurrentNodeStringLen = 0;

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	}

	_tcscpy_s(m_PckAllInfo.szNewFilename, szPckFile);
	//m_firstFile = cFileLinkList.first();

#pragma endregion

#pragma region ��������ӵ��ļ� 
	if(!EnumAllFilesByPathList(lpszFilePath, dwNewFileCount, qwTotalNewFileSize, m_PckAllInfo.lpFilesToBeAdded))
		return FALSE;

	if(0 == dwNewFileCount)return TRUE;

	m_PckAllInfo.dwFileCountToAdd = dwNewFileCount;
#pragma endregion
	//����˵����
	// mt_dwFileCount	��ӵ��ļ����������ظ�
	// dwFileCount		�������ʹ�ò�����������ļ�������н�ʹ�ô˲�����ʾ��ӵ��ļ������������ظ�

	//�ļ���д�봰�����б�������ʾ����
	SetParams_ProgressUpper(dwNewFileCount);

	//��������Ҫ���ռ�qwTotalFileSize
	mt_CompressTotalFileSize = GetPckFilesizeByCompressed(szPckFile, qwTotalNewFileSize, m_PckAllInfo.qwPckSize);

	//��ԭ��Ŀ¼�е��ļ��Աȣ��Ƿ�������
	//���ԣ����������ǰ�				��������Ҷ�Ϊ�ļ����ļ��У��򸲸�
	//
	//����FindFileNode����-1�˳�������0����ʾֱ����ӣ���0�������ظ���
	//дר�õ�writethread��compressthread,�Ե���
	//��PCKINDEXTABLE_COMPRESS�����addר�����ԣ����ж��Ƿ����ô˽ڵ㣨����ʱ��0ʹ�ã�1��ʹ��
	//���� ʱʹ��2��ѭ��д��ѹ������ 

	//dwFileCount�����ڴ˴�ָ������ӵ��ļ���ȥ���������� 
	if(m_PckAllInfo.isPckFileLoaded) {
		if(!FindDuplicateNodeFromFileList(lpNodeToInsertPtr, dwDuplicateFileCount))
			return FALSE;
	}

	//��־
	m_PckLog.PrintLogI(TEXT_UPDATE_FILE_INFO, m_PckAllInfo.dwFileCountToAdd, mt_CompressTotalFileSize);

#pragma region ����Ŀ���ļ�
	CMapViewFileWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, OPEN_ALWAYS, mt_CompressTotalFileSize)) {
		return FALSE;
	}

#pragma endregion

	cThreadParams.pThis = (CPckClassThreadWorker*)this;
	cThreadParams.cDataFetchMethod.lpFileToCompress = m_firstFile;
	cThreadParams.GetUncompressedData = &GetUncompressedDataFromFile;

	if(!initMultiThreadVars(&cFileWriter)) {
		assert(FALSE);
		return FALSE;
	}

	if(!initCompressedDataQueue(mt_dwFileCountOfWriteTarget = dwNewFileCount, dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex)) {
		assert(FALSE);
		return FALSE;
	}

	ExecuteMainThreadGroup(m_PckAllInfo, threadnum, &cThreadParams);
	dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex;

	//д�ļ�����
	m_PckAllInfo.dwFileCount = m_PckAllInfo.dwFileCountOld - dwDuplicateFileCount;

	WriteAllIndex(mt_lpFileWrite, &m_PckAllInfo, dwAddress);

	AfterProcess(mt_lpFileWrite, m_PckAllInfo, dwAddress);

	uninitCompressedDataQueue(threadnum);

	//���������´�һ�Σ�����ֱ�Ӵ򿪣��ɽ����߳����
	m_lpPckParams->cVarParams.dwOldFileCount = m_PckAllInfo.dwFileCountOld;
	m_lpPckParams->cVarParams.dwPrepareToAddFileCount = dwNewFileCount;
	m_lpPckParams->cVarParams.dwChangedFileCount = m_PckAllInfo.dwFileCountToAdd;
	m_lpPckParams->cVarParams.dwDuplicateFileCount = dwDuplicateFileCount;
	//m_lpPckParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize = m_PckAllInfo.dwFinalFileCount - m_PckAllInfo.dwFileCountOld;
	m_lpPckParams->cVarParams.dwFinalFileCount = m_PckAllInfo.dwFinalFileCount;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}
