//////////////////////////////////////////////////////////////////////
// PckClassFunction.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// һЩ�����Ĺ��� 
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#include "tlib.h"
#endif

#include "PckClassAllocFunctions.h"
#include "PckClass.h"
#include "CharsCodeConv.h"
#include "PckClassFileDisk.h"
#include "PckClassRebuildFilter.h"

using namespace NPckClassAllocFuncs;

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

//_inline int __fastcall strpathcmp (const char * src, char * &dst)
//{
//	int ret = 0 ;
//
//	while( (! (ret = (int)(*src - *dst))) && *src )
//		++src, ++dst;
//	if(0 != ret && 0 == *src && ('\\' == *dst || '/'== *dst))return 0;
//	return( ret );
//}

//strpathcmp
//���������¶��Ǵ�����ͬ���ļ���
//��ʱret != 0;
//
template <typename T>
_inline int __fastcall strpathcmp(const T * src, T * &dst)
{
	int ret = 0;

	while((!(ret = (int)(*src - *dst))) && *src)
		++src, ++dst;

	if(0 != ret && 0 == *src && ('\\' == *dst || '/' == *dst))return 0;
	return(ret);
}

template <typename T>
_inline void __fastcall strpathcpy(T * dst, T * &src)
{
	while((*dst++ = *src) && '\\' != *++src && '/' != *src)
		;
}


/********************************
*
*��������
*
********************/
BOOL CPckClass::RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	BOOL rtn = bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);

	//�ؽ��������������
	NPckClassRebuildFilter::ResetRebuildFilterInIndexList(m_PckAllInfo);

	return rtn;
}

BOOL CPckClass::RebuildPckFile(LPCTSTR szRebuildPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_REBUILD);

	LPPCKINDEXTABLE_COMPRESS	lpPckIndexTable, lpPckIndexTablePtr;
	QWORD	dwAddress = PCK_DATA_START_AT, dwAddressName;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = NPckClassFileDisk::GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	PCK_ALL_INFOS		pckAllInfo;

	LPBYTE				lpBufferToWrite, lpBufferToRead;
	CMapViewFileRead	cFileRead;

	//����ͷ��βʱ��Ҫ�Ĳ���
	memset(&pckAllInfo, 0, sizeof(PCK_ALL_INFOS));
	pckAllInfo.lpSaveAsPckVerFunc = m_PckAllInfo.lpSaveAsPckVerFunc;

	//���ý����������ֵ
	lpPckParams->cVarParams.dwUIProgressUpper = dwNoDupFileCount;

	//��Դ�ļ� 
	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		return FALSE;
	}

	//��Ŀ���ļ� 
	//�����Ǵ���һ���ļ������������ؽ�����ļ�
	CMapViewFileWrite	cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild)) {
		return FALSE;
	}

	//����ռ�,�ļ���ѹ������ ����
	if(NULL == (lpPckIndexTable = new PCKINDEXTABLE_COMPRESS[dwNoDupFileCount])) {
		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	//��ʹ��Enum���б�����������_PCK_INDEX_TABLE
	lpPckIndexTablePtr = lpPckIndexTable;

	LPPCKINDEXTABLE lpPckIndexTableSource = m_PckAllInfo.lpPckIndexTable;
	DWORD		IndexCompressedFilenameDataLengthCryptKey1 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1;
	DWORD		IndexCompressedFilenameDataLengthCryptKey2 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2;
	int			level = lpPckParams->dwCompressLevel;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;

		if(NULL == (lpBufferToWrite = cFileWrite.View(dwAddress, dwNumberOfBytesToMap))) {
			m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			delete[] lpPckIndexTable;
			return FALSE;
		}

		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//����ԭ���ĵ�ַ

		if(NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
			m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			delete[] lpPckIndexTable;
			return FALSE;
		}

		memcpy(lpBufferToWrite, lpBufferToRead, dwNumberOfBytesToMap);

		cFileRead.UnmapView();

		//д����ļ���PckFileIndex�ļ�ѹ����Ϣ��ѹ��
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//���ļ���ѹ��������ʼ��ַ

		dwAddress += lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;	//��һ���ļ���ѹ��������ʼ��ַ

		FillAndCompressIndexData(lpPckIndexTablePtr, &lpPckIndexTableSource->cFileIndex);

		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwSrcAddress;	//��ԭԭ���ĵ�ַ

		cFileWrite.UnmapView();

		++lpPckIndexTablePtr;
		++lpPckIndexTableSource;
		++(lpPckParams->cVarParams.dwUIProgress);

	}

	if(!lpPckParams->cVarParams.bThreadRunning) {
		m_PckLog.PrintLogW(TEXT_USERCANCLE);
		dwFileCount = lpPckParams->cVarParams.dwUIProgress;
	}

	//�رն��ļ�
	//delete lpFileRead;

	//д�ļ�����
	dwAddressName = dwAddress;
	WritePckIndexTable(&cFileWrite, lpPckIndexTable, dwNoDupFileCount, dwAddress);

	pckAllInfo.dwAddressName = dwAddressName;
	pckAllInfo.dwFileCount = dwNoDupFileCount;
	AfterProcess(&cFileWrite, pckAllInfo, dwAddress);

	//delete lpFileWrite;
	delete[] lpPckIndexTable;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}

VOID CPckClass::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	size_t nBytesReadayToWrite;
	char	*lpszPosToWrite;

#ifdef UNICODE
	CUcs2Ansi		cU2A;
	nBytesReadayToWrite = strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename) - cU2A.GetStrlen(lpNode->szName);
#else
	nBytesReadayToWrite = strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename) - strlen(lpNode->szName);
#endif
	lpszPosToWrite = lpNode->lpPckIndexTable->cFileIndex.szFilename + nBytesReadayToWrite;
	//nBytesReadayToWrite = MAX_PATH_PCK - nBytesReadayToWrite;

	memset(lpszPosToWrite, 0, MAX_PATH_PCK_260 - nBytesReadayToWrite);
	strcpy(lpszPosToWrite, lpszReplaceString);

}

VOID CPckClass::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	//size_t	nBytesReadayToWrite;

	memset(lpIndex->cFileIndex.szFilename, 0, MAX_PATH_PCK_260);
	strcpy(lpIndex->cFileIndex.szFilename, lpszReplaceString);

}

BOOL CPckClass::GetCurrentNodeString(char *szCurrentNodePathString, LPPCK_PATH_NODE lpNode)
{

	if((NULL == lpNode) || (NULL == lpNode->parentfirst)) {
		*szCurrentNodePathString = 0;
		//return TRUE;
	} else {

		GetCurrentNodeString(szCurrentNodePathString, lpNode->parentfirst);
#ifdef UNICODE
		CUcs2Ansi		cU2A;
		strcat_s(szCurrentNodePathString, MAX_PATH_PCK_260, cU2A.GetString(lpNode->parent->szName));
#else
		strcat_s(szCurrentNodePathString, MAX_PATH_PCK_260, lpNode->parent->szName);
#endif
		
		strcat_s(szCurrentNodePathString, MAX_PATH_PCK_260, "\\");
	}

	return TRUE;
}

BOOL CPckClass::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	//�����ļ���Ϊa\b\c\d.exe
	//���ڵ�Ϊc
	size_t lenNodeRes, lenrs, lenrp;
	char	lpReplacePos[MAX_PATH_PCK_260];

#ifdef UNICODE
	CUcs2Ansi		cU2A;
	lenNodeRes = cU2A.GetStrlen(lpNode->szName);
#else
	lenNodeRes = strlen(lpNode->szName);
#endif
	
	lenrs = strlen(lpszReplaceString);
	
	//ȡ��"a\b\"
	GetCurrentNodeString(lpReplacePos, lpNode->child);

	//lpReplacePos = "models\z1\"
	lenrp = strlen(lpReplacePos) - 1;

	return RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp);

}

/********************************
*
*˽�к���
*
********************/
/********************************
*
*���ļ�����Ŀ¼(\��/����֣���ӵ�Ŀ¼�ڵ���ȥ
*
*
**********************************/

BOOL CPckClass::AddFileToNode(LPPCK_PATH_NODE lpRootNode, LPPCKINDEXTABLE	lpPckIndexTable)
{
	LPPCK_PATH_NODE lpChildNode = lpRootNode;
	LPPCK_PATH_NODE	lpFirstNode = NULL;

	size_t			sizePckPathNode = sizeof(PCK_PATH_NODE);

	TCHAR			*lpszFilename = lpPckIndexTable->cFileIndex.sztFilename;
	TCHAR			*lpszToFind;

	do {
		//�˽ڵ��»�û���ļ�����һ���²����Ľڵ㣩���������".."
		if(NULL == (lpChildNode->child)) {
			lpChildNode->child = (LPPCK_PATH_NODE)AllocMemory(sizePckPathNode);
			lpChildNode->child->parent = lpChildNode;
			lpChildNode->child->parentfirst = lpFirstNode;

			//���..Ŀ¼
			//strcpy(lpChildNode->child->szName, "..");
			//*(DWORD*)lpChildNode->child->szName = 0x2e2e;	//".."
			memcpy(lpChildNode->child->szName, TEXT(".."), _tcslen(TEXT("..")) * sizeof(TCHAR));
		}

		lpFirstNode = lpChildNode = lpChildNode->child;

		do {
			lpszToFind = lpszFilename;

			if((NULL == lpChildNode->lpPckIndexTable) || (!lpChildNode->lpPckIndexTable->isInvalid)) {

				if(0 == strpathcmp(lpChildNode->szName, lpszToFind)) {
					//��������ļ����У���
					lpszFilename = lpszToFind;

					//�����ظ����ļ�������ǰһ���ظ����ļ�Ϊ��Ч
					if(0 == *lpszToFind)
						lpChildNode->lpPckIndexTable->isInvalid = TRUE;

					break;
				}

				if(NULL == lpChildNode->next) {

					//����ļ����У�
					lpChildNode->next = (LPPCK_PATH_NODE)AllocMemory(sizePckPathNode);
					lpChildNode = lpChildNode->next;

					strpathcpy(lpChildNode->szName, lpszFilename);

					//ͳ�Ƹ��ļ��е����ļ�����
					if(0 != *lpszFilename) {
						LPPCK_PATH_NODE	lpAddDirCount = lpFirstNode;
						do {
							++(lpAddDirCount->dwDirsCount);
							lpAddDirCount = lpAddDirCount->parentfirst;

						} while(NULL != lpAddDirCount);

					}

					break;
				}
			}
			lpChildNode = lpChildNode->next;

		} while(1);

		++(lpFirstNode->dwFilesCount);
		lpFirstNode->qdwDirCipherTextSize += lpPckIndexTable->cFileIndex.dwFileCipherTextSize;
		lpFirstNode->qdwDirClearTextSize += lpPckIndexTable->cFileIndex.dwFileClearTextSize;

		if(TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)
			++lpszFilename;

	} while(*lpszFilename);

	lpChildNode->lpPckIndexTable = lpPckIndexTable;

	return TRUE;

}

LPPCK_PATH_NODE CPckClass::FindFileNode(LPPCK_PATH_NODE lpBaseNode, char* lpszFile)
{
	LPPCK_PATH_NODE lpChildNode = lpBaseNode;
	//LPPCK_PATH_NODE	lpFirstNode = NULL;

	//size_t			sizePckPathNode = sizeof(PCK_PATH_NODE);

#ifdef UNICODE
	CAnsi2Ucs		cA2U;
	TCHAR			szFilename[MAX_PATH];
	cA2U.GetString(lpszFile, szFilename, MAX_PATH);

	TCHAR			*lpszFilename = szFilename;
	TCHAR			*lpszToFind;
#else

	TCHAR			*lpszFilename = lpszFile;
	TCHAR			*lpszToFind;
#endif

	if(NULL == lpChildNode->szName)
		return NULL;

	do {
		do {
			lpszToFind = lpszFilename;

			if(0 == strpathcmp(lpChildNode->szName, lpszToFind)) {
				lpszFilename = lpszToFind;

				if(NULL == lpChildNode->child && 0 == *lpszFilename)return lpChildNode;

				if((NULL == lpChildNode->child && (TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)) || (NULL != lpChildNode->child && 0 == *lpszFilename)) {
					return (LPPCK_PATH_NODE)-1;
				}

				break;
			}

			if(NULL == lpChildNode->next) {
				return NULL;
			}
			lpChildNode = lpChildNode->next;

		} while(1);

		lpChildNode = lpChildNode->child;

		if(TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)
			++lpszFilename;

	} while(*lpszFilename);

	return NULL;

}

BOOL CPckClass::RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
{

	lpNode = lpNode->child->next;

	while(NULL != lpNode) {

		if(NULL == lpNode->child) {

			if(!RenameNode(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp))
				return FALSE;
		} else {
			if(!RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp))
				return FALSE;
		}

		lpNode = lpNode->next;
	}
	return TRUE;
}

BOOL CPckClass::RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
{
	if(lenrs >= (MAX_PATH_PCK_260 - strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename + lenNodeRes - 2)))return FALSE;
	char	szTemp[MAX_PATH_PCK_260] = { 0 };
	char	*lpszReplacePos = lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp - lenNodeRes;

	//DebugA("lpszReplaceString = %s \r\nlenNodeRes = %d\r\nlenrs = %d\r\nlenrp = %d\r\n===============================\r\n",
	//		lpszReplaceString, lenNodeRes, lenrs, lenrp);
	memcpy(szTemp, lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp, MAX_PATH_PCK_260 - lenrp);
	memcpy(lpszReplacePos, lpszReplaceString, lenrs);
	memcpy(lpszReplacePos + lenrs, szTemp, MAX_PATH_PCK_260 - lenrp - lenrs + lenNodeRes);

	return TRUE;
}

