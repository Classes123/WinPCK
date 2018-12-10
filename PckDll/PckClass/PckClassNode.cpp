#include "PckClassNode.h"

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
		//������ͱ�ʶ
		lpPckIndexTable->entryType = PCK_ENTRY_TYPE_INDEX;
		//����Ŀ¼
		AddFileToNode(lpPckIndexTable);
		++lpPckIndexTable;
	}
	
	//����һ��root�ڵ��µ�entryType���ϱ�ǩ��ʾ����
	if(NULL != m_PckAllInfo.cRootNode.child)
		m_PckAllInfo.cRootNode.child->entryType |= PCK_ENTRY_TYPE_ROOT;
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

	wchar_t			*lpszFilename = lpPckIndexTable->cFileIndex.szwFilename;
	wchar_t			*lpszToFind;


	do {
		//�˽ڵ��»�û���ļ�����һ���²����Ľڵ㣩���������".."
		if(NULL == (lpChildNode->child)) {
			lpChildNode->child = (LPPCK_PATH_NODE)AllocMemory(sizeof(PCK_PATH_NODE));
			lpChildNode->child->parent = lpChildNode;
			lpChildNode->child->parentfirst = lpFirstNode;

			//ʹ��nNameSizeAnsi ��..Ŀ¼��¼��Ŀ¼·������gfx\�µ�..Ŀ¼���ĳ��ȣ�ansi��
			if(NULL != lpFirstNode)
				lpChildNode->child->nNameSizeAnsi = (lpFirstNode->nNameSizeAnsi + lpChildNode->nNameSizeAnsi + 1);

			//���..Ŀ¼
			//strcpy(lpChildNode->child->szName, "..");
			//*(DWORD*)lpChildNode->child->szName = 0x2e2e;	//".."
			memcpy(lpChildNode->child->szName, L"..", wcslen(L"..") * sizeof(wchar_t));

			lpChildNode->child->entryType = PCK_ENTRY_TYPE_NODE | PCK_ENTRY_TYPE_FOLDER | PCK_ENTRY_TYPE_DOTDOT;
		}

		lpFirstNode = lpChildNode = lpChildNode->child;

		//����Ŀ¼�㣨����\\��
		do {
			//��һ��Ŀ¼(next '\\')
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

				//�˼��ڵ��ѱ����꣬û���ظ��ģ�����½ڵ�
				if(NULL == lpChildNode->next) {

					//����ļ����У�
					lpChildNode->next = (LPPCK_PATH_NODE)AllocMemory(sizeof(PCK_PATH_NODE));
					lpChildNode = lpChildNode->next;

					lpChildNode->parent = lpFirstNode->parent;

					lpChildNode->entryType = PCK_ENTRY_TYPE_NODE | PCK_ENTRY_TYPE_FOLDER;

					strpathcpy(lpChildNode->szName, lpszFilename);
					lpChildNode->nNameSizeAnsi = PckFilenameCode2Ansi(lpChildNode->szName, NULL, 0);
					lpChildNode->nMaxNameSizeAnsi = MAX_PATH_PCK_256;

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
			else {
				//������
			}
			lpChildNode = lpChildNode->next;

		} while(1);

		//���ļ�������ͳ��
		++(lpFirstNode->dwFilesCount);
		lpFirstNode->qdwDirCipherTextSize += lpPckIndexTable->cFileIndex.dwFileCipherTextSize;
		lpFirstNode->qdwDirClearTextSize += lpPckIndexTable->cFileIndex.dwFileClearTextSize;

		if (lpChildNode->nMaxNameSizeAnsi > lpPckIndexTable->nFilelenLeftBytes) {
			lpChildNode->nMaxNameSizeAnsi = lpPckIndexTable->nFilelenLeftBytes;
		}

		if(TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)
			++lpszFilename;

	} while(*lpszFilename);

	lpChildNode->lpPckIndexTable = lpPckIndexTable;
	lpChildNode->entryType = PCK_ENTRY_TYPE_NODE;

	return TRUE;

}
#pragma endregion

#pragma region FindFileNode
const PCK_PATH_NODE* CPckClassNode::FindFileNode(const PCK_PATH_NODE* lpBaseNode, wchar_t* lpszFile)
{
	const PCK_PATH_NODE* lpChildNode = lpBaseNode;

	wchar_t			szFilename[MAX_PATH];
	wcscpy_s(szFilename, lpszFile);

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
	//if(lenrs >= (MAX_PATH_PCK_260 - strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename + lenNodeRes - 2)))return FALSE;
	char	szTemp[MAX_PATH_PCK_260] = { 0 };
	char	*lpszReplacePos = lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp - lenNodeRes;

	//DebugA("lpszReplaceString = %s \r\nlenNodeRes = %d\r\nlenrs = %d\r\nlenrp = %d\r\n===============================\r\n",
	//		lpszReplaceString, lenNodeRes, lenrs, lenrp);
	memcpy(szTemp, lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp, MAX_PATH_PCK_260 - lenrp);
	memcpy(lpszReplacePos, lpszReplaceString, lenrs);
	memcpy(lpszReplacePos + lenrs, szTemp, MAX_PATH_PCK_260 - lenrp - lenrs + lenNodeRes);

	return TRUE;
}
#if 0

BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	//�����ļ���Ϊa\b\c\d.exe
	//���ڵ�Ϊc
	size_t lenNodeRes, lenrs, lenrp;
	char	lpReplacePos[MAX_PATH_PCK_260];

	CUcs2Ansi		cU2A(m_lpPckParams->code_page);
	lenNodeRes = cU2A.GetStrlen(lpNode->szName);

	lenrs = strlen(lpszReplaceString);

	//ȡ��"a\b\"
	GetCurrentNodeString(lpReplacePos, lpNode->child);

	//lpReplacePos = "models\z1\"
	lenrp = strlen(lpReplacePos) - 1;

	return RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp);

}


BOOL CPckClassNode::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	size_t nBytesReadayToWrite;
	char	*lpszPosToWrite;

	CUcs2Ansi		cU2A(m_lpPckParams->code_page);
	nBytesReadayToWrite = strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename) - cU2A.GetStrlen(lpNode->szName);

	lpszPosToWrite = lpNode->lpPckIndexTable->cFileIndex.szFilename + nBytesReadayToWrite;
	//nBytesReadayToWrite = MAX_PATH_PCK - nBytesReadayToWrite;

	memset(lpszPosToWrite, 0, MAX_PATH_PCK_260 - nBytesReadayToWrite);
	strcpy(lpszPosToWrite, lpszReplaceString);
	return TRUE;

}

BOOL CPckClassNode::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	memset(lpIndex->cFileIndex.szFilename, 0, MAX_PATH_PCK_260);
	strcpy(lpIndex->cFileIndex.szFilename, lpszReplaceString);
	return TRUE;

}



BOOL CPckClassNode::RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, wchar_t* lpszReplaceString, size_t lenrs, size_t lenrp)
{

	lpNode = lpNode->child->next;

	while (NULL != lpNode) {

		if (NULL == lpNode->child) {

			if (!RenameNode(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp))
				return FALSE;
		}
		else {
			if (!RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp))
				return FALSE;
		}

		lpNode = lpNode->next;
	}
	return TRUE;
}

BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, wchar_t* lpszReplaceString, size_t lenrs, size_t lenrp)
{
	if (lenrs >= (MAX_PATH_PCK_260 - strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename + lenNodeRes - 2)))return FALSE;

	char lpszReplaceStringAnsi[MAX_PATH_PCK_260];

	PckFilenameCode2Ansi(lpszReplaceString, lpszReplaceStringAnsi, sizeof(lpszReplaceStringAnsi));

	return RenameNode(lpNode, lenNodeRes, lpszReplaceStringAnsi, lenrs, lenrp);

}
#endif
//public
BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString)
{
	//�����ļ���Ϊa\b\c\d.exe
	//���ڵ�Ϊc
	size_t	lenNodeRes, lenrs, lenrp;
	char	lpszReplaceStringAnsi[MAX_PATH_PCK_260];

	lenrs = PckFilenameCode2Ansi(lpszReplaceString, lpszReplaceStringAnsi, sizeof(lpszReplaceStringAnsi));

	lenNodeRes = lpNode->nNameSizeAnsi;

	size_t nAllowedMaxSize = lpNode->nMaxNameSizeAnsi + lenNodeRes;

	if (nAllowedMaxSize < lenrs)
		return FALSE;

	//ȡ��"a\b\"
	lenrp = lpNode->child->nNameSizeAnsi - 1;

	return RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceStringAnsi, lenrs, lenrp);

}


BOOL CPckClassNode::RenameIndex(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString)
{
	//���Ŀ¼���ģʽ�µ��ļ������޸�
	int		nBytesReadayToWrite;
	char	*lpszPosToWrite;	//�ļ�����ַ������lpszFileTitle

	lpszPosToWrite = lpNode->lpPckIndexTable->cFileIndex.szFilename + lpNode->lpPckIndexTable->nFilelenBytes - lpNode->nNameSizeAnsi;
	nBytesReadayToWrite = lpNode->nMaxNameSizeAnsi + lpNode->nNameSizeAnsi;

	//ת��Ϊansi����鳤��
	char szReplaceStringAnsi[MAX_PATH_PCK_260];

	size_t nLenOfReplaceString =  PckFilenameCode2Ansi(lpszReplaceString, szReplaceStringAnsi, MAX_PATH_PCK_260);

	if (nBytesReadayToWrite < nLenOfReplaceString)
		return FALSE;

	memset(lpszPosToWrite, 0, nBytesReadayToWrite);
	strcpy(lpszPosToWrite, szReplaceStringAnsi);
	return TRUE;
}

BOOL CPckClassNode::RenameIndex(LPPCKINDEXTABLE lpIndex, const wchar_t* lpszReplaceString)
{
	//ת��Ϊansi����鳤��
	char szReplaceStringAnsi[MAX_PATH_PCK_260];

	size_t nLenOfReplaceString = PckFilenameCode2Ansi(lpszReplaceString, szReplaceStringAnsi, MAX_PATH_PCK_260);

	if (MAX_PATH_PCK_256 < nLenOfReplaceString)
		return FALSE;

	memset(lpIndex->cFileIndex.szFilename, 0, MAX_PATH_PCK_260);
	strcpy(lpIndex->cFileIndex.szFilename, szReplaceStringAnsi);
	return TRUE;
}

BOOL CPckClassNode::GetCurrentNodeString(wchar_t *szCurrentNodePathString, const PCK_PATH_NODE* lpNode)
{
	if ((NULL == lpNode) || (PCK_ENTRY_TYPE_ROOT == (PCK_ENTRY_TYPE_ROOT & lpNode->entryType))) {
		*szCurrentNodePathString = 0;
		return TRUE;
	}

	int entry_type = lpNode->entryType;

	if (PCK_ENTRY_TYPE_DOTDOT == (PCK_ENTRY_TYPE_DOTDOT & entry_type)) {

		if (NULL == lpNode->parentfirst) {
			*szCurrentNodePathString = 0;
			return TRUE;
		}

		lpNode = lpNode->parentfirst;

		GetCurrentNodeString(szCurrentNodePathString, lpNode->parentfirst);

		wcscat_s(szCurrentNodePathString, MAX_PATH_PCK_260, lpNode->parent->szName);
		wcscat_s(szCurrentNodePathString, MAX_PATH_PCK_260, L"\\");

	}
	else {

		GetCurrentNodeString(szCurrentNodePathString, lpNode->parent);
		wcscat_s(szCurrentNodePathString, MAX_PATH_PCK_260, lpNode->szName);

		if(PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type))
			wcscat_s(szCurrentNodePathString, MAX_PATH_PCK_260, L"\\");

	}
	return TRUE;
}

#pragma endregion


BOOL CPckClassNode::FindDuplicateNodeFromFileList(const PCK_PATH_NODE* lpNodeToInsertPtr, DWORD &_in_out_DuplicateFileCount)
{
	vector<FILES_TO_COMPRESS> *lpFilesList = m_PckAllInfo.lpFilesToBeAdded;
	size_t sizeOfFileList = lpFilesList->size();

	//LPFILES_TO_COMPRESS lpfirstFile = m_firstFile;
	//while(NULL != lpfirstFile->next) {
	for(size_t i=0;i<sizeOfFileList;i++){

		FILES_TO_COMPRESS *lpfirstFile = &(*lpFilesList)[i];
		const PCK_PATH_NODE* lpDuplicateNode = FindFileNode(lpNodeToInsertPtr, lpfirstFile->szwFilename + lpfirstFile->nFileTitleLen);

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

		//lpfirstFile = lpfirstFile->next;

	}
	return TRUE;
}