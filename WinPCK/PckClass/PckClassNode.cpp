#include "PckClassNode.h"
#include "CharsCodeConv.h"

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
#pragma region �ຯ��

CPckClassNode::CPckClassNode()
{
	m_lpRootNode = &m_PckAllInfo.cRootNode;
}

CPckClassNode::~CPckClassNode()
{
	DeAllocMultiNodes(m_lpRootNode->child);
}

VOID CPckClassNode::DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode)
{

	LPPCK_PATH_NODE		lpThisNodePtr;

	while(NULL != lpThisNode) {

		lpThisNodePtr = lpThisNode;
		if(NULL != lpThisNodePtr->child) {
			DeAllocMultiNodes(lpThisNodePtr->child);
		}
		lpThisNode = lpThisNodePtr->next;

		free(lpThisNodePtr);
	}
	//memset(&m_cRootNode, 0, sizeof(m_cRootNode));
}

#pragma endregion
#pragma region ParseIndexTableToNode

void CPckClassNode::ParseIndexTableToNode(LPPCKINDEXTABLE lpMainIndexTable)
{
	LPPCKINDEXTABLE lpPckIndexTable = lpMainIndexTable;

	for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {
		//����Ŀ¼

		AddFileToNode(lpPckIndexTable);
		++lpPckIndexTable;
	}

}

/********************************
*
*���ļ�����Ŀ¼(\��/����֣���ӵ�Ŀ¼�ڵ���ȥ
*
*
**********************************/

BOOL CPckClassNode::AddFileToNode(LPPCKINDEXTABLE lpPckIndexTable)
{
	LPPCK_PATH_NODE lpChildNode = &m_PckAllInfo.cRootNode;
	LPPCK_PATH_NODE	lpFirstNode = NULL;

	size_t			sizePckPathNode = sizeof(PCK_PATH_NODE);

	wchar_t			*lpszFilename = lpPckIndexTable->cFileIndex.szwFilename;
	wchar_t			*lpszToFind;

	do {
		//�˽ڵ��»�û���ļ�����һ���²����Ľڵ㣩���������".."
		if(NULL == (lpChildNode->child)) {
			lpChildNode->child = (LPPCK_PATH_NODE)AllocMemory(sizePckPathNode);
			lpChildNode->child->parent = lpChildNode;
			lpChildNode->child->parentfirst = lpFirstNode;

			//���..Ŀ¼
			//strcpy(lpChildNode->child->szName, "..");
			//*(DWORD*)lpChildNode->child->szName = 0x2e2e;	//".."
			memcpy(lpChildNode->child->szName, L"..", wcslen(L"..") * sizeof(wchar_t));
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
#pragma endregion

#pragma region FindFileNode
LPPCK_PATH_NODE CPckClassNode::FindFileNode(const LPPCK_PATH_NODE lpBaseNode, char* lpszFile)
{
	LPPCK_PATH_NODE lpChildNode = lpBaseNode;

	CAnsi2Ucs		cA2U;
	wchar_t			szFilename[MAX_PATH];
	cA2U.GetString(lpszFile, szFilename, MAX_PATH);

	wchar_t			*lpszFilename = szFilename;
	wchar_t			*lpszToFind;


	if(NULL == lpChildNode->szName)
		return NULL;

	do {
		do {
			lpszToFind = lpszFilename;

			if(0 == strpathcmp(lpChildNode->szName, lpszToFind)) {
				lpszFilename = lpszToFind;

				if(NULL == lpChildNode->child && 0 == *lpszFilename)return lpChildNode;

				if((NULL == lpChildNode->child && (TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)) || (NULL != lpChildNode->child && 0 == *lpszFilename)) {
					return (LPPCK_PATH_NODE)INVALID_NODE;
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

#pragma endregion
#pragma region DeleteNode

VOID CPckClassNode::DeleteNode(LPPCKINDEXTABLE lpIndex)
{
	lpIndex->isInvalid = TRUE;
}

VOID CPckClassNode::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode) {
		if(NULL == lpNode->child) {
			DeleteNode(lpNode->lpPckIndexTable);
		} else {
			DeleteNode(lpNode);
		}

		lpNode = lpNode->next;
	}
}

#pragma endregion

#pragma region RenameNode

BOOL CPckClassNode::RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
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

BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
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


BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	//�����ļ���Ϊa\b\c\d.exe
	//���ڵ�Ϊc
	size_t lenNodeRes, lenrs, lenrp;
	char	lpReplacePos[MAX_PATH_PCK_260];

	CUcs2Ansi		cU2A;
	lenNodeRes = cU2A.GetStrlen(lpNode->szName);

	lenrs = strlen(lpszReplaceString);

	//ȡ��"a\b\"
	GetCurrentNodeString(lpReplacePos, lpNode->child);

	//lpReplacePos = "models\z1\"
	lenrp = strlen(lpReplacePos) - 1;

	return RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp);

}


VOID CPckClassNode::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	size_t nBytesReadayToWrite;
	char	*lpszPosToWrite;

	CUcs2Ansi		cU2A;
	nBytesReadayToWrite = strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename) - cU2A.GetStrlen(lpNode->szName);

	lpszPosToWrite = lpNode->lpPckIndexTable->cFileIndex.szFilename + nBytesReadayToWrite;
	//nBytesReadayToWrite = MAX_PATH_PCK - nBytesReadayToWrite;

	memset(lpszPosToWrite, 0, MAX_PATH_PCK_260 - nBytesReadayToWrite);
	strcpy(lpszPosToWrite, lpszReplaceString);

}

VOID CPckClassNode::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	memset(lpIndex->cFileIndex.szFilename, 0, MAX_PATH_PCK_260);
	strcpy(lpIndex->cFileIndex.szFilename, lpszReplaceString);

}

BOOL CPckClassNode::GetCurrentNodeString(char *szCurrentNodePathString, LPPCK_PATH_NODE lpNode)
{

	if((NULL == lpNode) || (NULL == lpNode->parentfirst)) {
		*szCurrentNodePathString = 0;
		//return TRUE;
	} else {

		GetCurrentNodeString(szCurrentNodePathString, lpNode->parentfirst);

		CUcs2Ansi		cU2A;
		strcat_s(szCurrentNodePathString, MAX_PATH_PCK_260, cU2A.GetString(lpNode->parent->szName));

		strcat_s(szCurrentNodePathString, MAX_PATH_PCK_260, "\\");
	}

	return TRUE;
}

#pragma endregion


BOOL CPckClassNode::FindDuplicateNodeFromFileList(const LPPCK_PATH_NODE lpNodeToInsertPtr, DWORD &_in_out_DuplicateFileCount)
{
	LPFILES_TO_COMPRESS lpfirstFile = m_firstFile;
	while(NULL != lpfirstFile->next) {
		LPPCK_PATH_NODE lpDuplicateNode;
		lpDuplicateNode = FindFileNode(lpNodeToInsertPtr, lpfirstFile->lpszFileTitle);

		if(INVALID_NODE == (int)lpDuplicateNode) {
			m_PckLog.PrintLogE(TEXT_ERROR_DUP_FOLDER_FILE);
			assert(FALSE);
			return FALSE;
		}

		if(NULL != lpDuplicateNode) {
			lpfirstFile->samePtr = lpDuplicateNode->lpPckIndexTable;
			lpDuplicateNode->lpPckIndexTable->isInvalid = TRUE;
			++_in_out_DuplicateFileCount;
		}

		lpfirstFile = lpfirstFile->next;

	}
	return TRUE;
}