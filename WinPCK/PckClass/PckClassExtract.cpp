//////////////////////////////////////////////////////////////////////
// PckClassExtract.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ��ѹ����
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4267 )

#include "PckClass.h"

BOOL CPckClass::GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer)
{

	CMapViewFileRead	*lpFileRead = (CMapViewFileRead*)lpvoidFileRead;

	LPPCKFILEINDEX lpPckFileIndex = &lpPckFileIndexTable->cFileIndex;

	if(NULL == lpvoidFileRead) {
		lpFileRead = new CMapViewFileRead();

		if(!lpFileRead->OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
			delete lpFileRead;
			return FALSE;
		}

	}

	BYTE	*lpMapAddress;
	if(NULL == (lpMapAddress = lpFileRead->View(lpPckFileIndex->dwAddressOffset, lpPckFileIndex->dwFileCipherTextSize))) {

		m_PckLog.PrintLogEL(TEXT_VIEWMAPNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);

		if(NULL == lpvoidFileRead)
			delete lpFileRead;
		return FALSE;
	}

	DWORD	dwFileLengthToWrite = lpPckFileIndex->dwFileClearTextSize;

	if(0 != sizeOfBuffer && sizeOfBuffer < dwFileLengthToWrite)
		dwFileLengthToWrite = sizeOfBuffer;


	if(check_zlib_header(lpMapAddress)) {

		if(Z_OK != decompress_part((BYTE*)buffer, &dwFileLengthToWrite,
			lpMapAddress, lpPckFileIndex->dwFileCipherTextSize, lpPckFileIndex->dwFileClearTextSize)) {
			if(lpPckFileIndex->dwFileClearTextSize == lpPckFileIndex->dwFileCipherTextSize)
				memcpy(buffer, lpMapAddress, dwFileLengthToWrite);
			else {

				m_PckLog.PrintLogEL(TEXT_UNCOMPRESSDATA_FAIL, lpPckFileIndex->szFilename, __FILE__, __FUNCTION__, __LINE__);
				assert(FALSE);
				lpFileRead->UnmapView();

				if(NULL == lpvoidFileRead)
					delete lpFileRead;

				return FALSE;
			}
		}
	} else {
		memcpy(buffer, lpMapAddress, dwFileLengthToWrite);
	}

	lpFileRead->UnmapView();

	if(NULL == lpvoidFileRead)
		delete lpFileRead;

	return TRUE;

}

BOOL CPckClass::ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount)
{

	m_PckLog.PrintLogI(TEXT_LOG_EXTRACT);

	CMapViewFileRead	cFileRead;

	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) 
		return FALSE;

	BOOL	ret = TRUE;

	wchar_t	szFilename[MAX_PATH_PCK_260], *szStrchr;

	LPPCKINDEXTABLE *lpIndexToExtractPtr = lpIndexToExtract;

	for(int i = 0;i < nFileCount;i++) {
		if(!m_lpPckParams->cVarParams.bThreadRunning) {
			m_PckLog.PrintLogW(TEXT_USERCANCLE);

			return FALSE;
		}

		CAnsi2Ucs cA2U;
		cA2U.GetString((*lpIndexToExtractPtr)->cFileIndex.szFilename, szFilename, MAX_PATH_PCK_260);

		szStrchr = szFilename;
		for(int j = 0;j < MAX_PATH_PCK_260;j++) {
			if(TEXT('\\') == *szStrchr)*szStrchr = TEXT('_');
			else if(TEXT('/') == *szStrchr)*szStrchr = TEXT('_');
			//else if(0 == *szStrchr)break;
			++szStrchr;
		}

		//��ѹ�ļ�
		if(!(DecompressFile(szFilename, *lpIndexToExtractPtr, &cFileRead))) {
			m_PckLog.PrintLogEL(TEXT_UNCOMP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			return FALSE;
		} else {
			//dwCount++;
			++(m_lpPckParams->cVarParams.dwUIProgress);
		}

		++lpIndexToExtractPtr;
	}

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);
	return	ret;
}

BOOL CPckClass::ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount)
{

	m_PckLog.PrintLogI(TEXT_LOG_EXTRACT);

	CMapViewFileRead	cFileRead;

	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) 
		return FALSE;

	BOOL	ret = TRUE;

	LPPCK_PATH_NODE *lpNodeToExtractPtr = lpNodeToExtract;

	for(int i = 0;i < nFileCount;i++) {
		if(!m_lpPckParams->cVarParams.bThreadRunning) {
			m_PckLog.PrintLogW(TEXT_USERCANCLE);
			return FALSE;
		}

		if(NULL == (*lpNodeToExtractPtr)->child) {

			//��ѹ�ļ�
			CUcs2Ansi cU2A;
			if(!(DecompressFile((*lpNodeToExtractPtr)->szName, (*lpNodeToExtractPtr)->lpPckIndexTable, &cFileRead))) {
				m_PckLog.PrintLogEL(TEXT_UNCOMP_FAIL, __FILE__, __FUNCTION__, __LINE__);
				return FALSE;
			} else {
				//dwCount++;
				++(m_lpPckParams->cVarParams.dwUIProgress);
			}

		} else {

			CreateDirectoryW((*lpNodeToExtractPtr)->szName, NULL);
			SetCurrentDirectoryW((*lpNodeToExtractPtr)->szName);
			ret = StartExtract((*lpNodeToExtractPtr)->child->next, &cFileRead);
			SetCurrentDirectoryA("..\\");
		}

		lpNodeToExtractPtr++;
	}

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);
	return	ret;
}
BOOL CPckClass::StartExtract(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpvoidFileRead)
{

	CMapViewFileRead	*lpFileRead = (CMapViewFileRead*)lpvoidFileRead;

	do {
		if(!m_lpPckParams->cVarParams.bThreadRunning) {
			m_PckLog.PrintLogW(TEXT_USERCANCLE);
			return FALSE;
		}

		//lpThisNodePtr = lpNodeToExtract;
		if(NULL != lpNodeToExtract->child) {

			CreateDirectoryW(lpNodeToExtract->szName, NULL);
			SetCurrentDirectoryW(lpNodeToExtract->szName);

			StartExtract(lpNodeToExtract->child->next, lpFileRead);

			SetCurrentDirectoryA("..\\");

		} else {

			//��ѹ�ļ�
			if(!DecompressFile(lpNodeToExtract->szName, lpNodeToExtract->lpPckIndexTable, lpFileRead)) {
				m_PckLog.PrintLogEL(TEXT_UNCOMP_FAIL, __FILE__, __FUNCTION__, __LINE__);

				return FALSE;
			} else {
				//dwCount++;
				++(m_lpPckParams->cVarParams.dwUIProgress);
			}
		}

		lpNodeToExtract = lpNodeToExtract->next;

	} while(NULL != lpNodeToExtract);

	return TRUE;
}

BOOL CPckClass::DecompressFile(LPCWSTR	lpszFilename, LPPCKINDEXTABLE lpPckFileIndexTable, LPVOID lpvoidFileRead)
{
	LPPCKFILEINDEX lpPckFileIndex = &lpPckFileIndexTable->cFileIndex;

	CMapViewFileWrite cFileWrite(0xffffffff);

	LPBYTE	lpMapAddressToWrite;
	DWORD	dwFileLengthToWrite;

	//if (0 == strcmp(lpszFilename, "�����м��ܹ���1.gfx")){
	//	dwFileLengthToWrite = 1;
	//}

	dwFileLengthToWrite = lpPckFileIndex->dwFileClearTextSize;

	//�����Ǵ���һ���ļ������������ѹ������ļ�
	if(!cFileWrite.Open(lpszFilename, CREATE_ALWAYS)) {
		m_PckLog.PrintLogEL(TEXT_OPENWRITENAME_FAIL, lpszFilename, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	if(!cFileWrite.Mapping(dwFileLengthToWrite)) {
		if(0 == dwFileLengthToWrite)
			return TRUE;
		m_PckLog.PrintLogEL(TEXT_CREATEMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	if(NULL == (lpMapAddressToWrite = cFileWrite.View(0, 0))) {
		m_PckLog.PrintLogEL(TEXT_VIEWMAPNAME_FAIL, lpszFilename, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	BOOL rtn = GetSingleFileData(lpvoidFileRead, lpPckFileIndexTable, (char*)lpMapAddressToWrite);

	cFileWrite.SetFilePointer(dwFileLengthToWrite, FILE_BEGIN);
	cFileWrite.SetEndOfFile();

	return rtn;
}

