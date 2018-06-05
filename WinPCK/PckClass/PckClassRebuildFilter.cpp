//////////////////////////////////////////////////////////////////////
// PckClassRebuildFilter.cpp: ���ڽ������˽ű�
//
// �˳����� �����/stsm/liqf ��д�����ִ����д��RapidCRC
//
// �˴��뿪Դ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.5.15
//////////////////////////////////////////////////////////////////////
#include "PckClass.h"
#include <stdlib.h>
#include "CharsCodeConv.h"
#include "PckClassRebuildFilter.h"
#include "PckClassLinkList.h"
//chkfile
//protect
//delete
//rmdir

namespace NPckClassRebuildFilter
{

#define MAX_LINE_LENGTH (MAX_PATH + 10)
#define MAX_SCRIPT_SIZE	(10*1024*1024)

	typedef enum { OP_CheckFile = 0, OP_Protect, OP_Delete, OP_Rmdir, OP_Unknown } SCRIPTOP;
	static const char szOperators[] = \
		"chkfile;"
		"protect;"
		"delete;;"
		"rmdir;;;";

	typedef struct
	{
		BYTE*			buffer;
		const BYTE*		bufpos;
		DWORD			dwSize;
		DWORD			dwPos;

	}SCRIPTBUFFER;

	typedef struct _FILEOPS
	{
		SCRIPTOP op;
		wchar_t	szFilename[MAX_PATH];
		wchar_t	szFilenameBuffer[MAX_PATH];		//��·���е�\��/��0���ֿ���·�����Ʒֱ���lpszSepratedPathsָ��
		wchar_t*	lpszSepratedPaths[MAX_PATH];
		_FILEOPS * next;
	}FILEOP;


	VOID DeallocateFileopMemory(FILEOP *m_firstFileOp)
	{
		FILEOP* Fileinfo;

		while(m_firstFileOp != NULL) {
			Fileinfo = m_firstFileOp->next;
			free(m_firstFileOp);
			m_firstFileOp = Fileinfo;
		}
	}

	CPckClassLog	m_PckLog;						//��ӡ��־

	void SetBufPointer(SCRIPTBUFFER *sfvBuf, int pos)
	{
		sfvBuf->dwPos += pos;
		sfvBuf->bufpos += pos;
	}

	BOOL ReadFromBuffer(void *_dstbuf, size_t size, SCRIPTBUFFER *sfvBuf)
	{
		if((sfvBuf->dwPos + size) > sfvBuf->dwSize)
			return FALSE;

		memcpy(_dstbuf, sfvBuf->bufpos, size);
		SetBufPointer(sfvBuf, size);
		return TRUE;
	}

	VOID GetNextLine(SCRIPTBUFFER *sfvBuf, CHAR * szLineAnsi, CONST UINT uiLengthLine,
		UINT * puiStringLength, BOOL * pbErrorOccured, BOOL * pbEndOfFile)
	{
		CHAR myChar;

		UINT uiCount;
		BOOL bSuccess;

		// ������Ҫһ���ֽڴ�� \0
		if(uiLengthLine <= 1) {
			(*pbErrorOccured) = TRUE;
			return;
		}

		ZeroMemory(szLineAnsi, uiLengthLine);
		uiCount = 0;

		while(TRUE) {
			//���´�ѭ����д��szLineAnsi[uiCount]���� szLineAnsi[uiCount+1] ��д��\0
			if(uiCount >= uiLengthLine - 1) {
				(*pbErrorOccured) = TRUE;
				return;
			}
			bSuccess = ReadFromBuffer(&myChar, sizeof(myChar), sfvBuf);

			if(!bSuccess) {
				(*pbErrorOccured) = FALSE;
				(*puiStringLength) = uiCount;
				(*pbEndOfFile) = TRUE;
				return;
			}

			if((myChar == '\n') || (myChar == '\r')) {
				do {
					bSuccess = ReadFromBuffer(&myChar, sizeof(myChar), sfvBuf);

				} while((bSuccess) && ((myChar == '\n') || (myChar == '\r')));

				if(bSuccess && !((myChar == '\n') || (myChar == '\r')))
					SetBufPointer(sfvBuf, -1);

				(*pbErrorOccured) = FALSE;
				(*puiStringLength) = uiCount;
				(*pbEndOfFile) = FALSE;
				return;
			} else {
				szLineAnsi[uiCount] = myChar;
				uiCount += 1;
			}
		}
		return;
	}

	BOOL ParseOneLine(FILEOP * pFileOp, char * lpszLine, TCHAR *lpszFileName)
	{

		char szOperator[16] = { 0 };
		//���ȼ��16���ַ�����û�пո��tab
		char *lpszCell = lpszLine, *lpszSearch = lpszLine;
		size_t count = 0;

		char *lpszOperator = szOperator;
		BOOL isValid = FALSE;

		while((16 > (++count)) &&
			(*lpszSearch)) {

			if((' ' == *lpszSearch) || ('\t' == *lpszSearch)) {
				isValid = TRUE;
				break;
			}
			*lpszOperator++ = *lpszSearch++;
		}

		if(!isValid)
			return FALSE;

		//����szOperator
		const char *lpszOpPos = strstr(szOperators, szOperator);
		if(NULL == lpszOpPos)
			return FALSE;

		pFileOp->op = SCRIPTOP((lpszOpPos - szOperators) / 8);

		//���˵�ʣ�µĿո��tab
		while((' ' == *lpszSearch) || ('\t' == *lpszSearch))
			lpszSearch++;

		isValid = FALSE;
		count = 0;

		if((MAX_PATH <= strlen(lpszSearch)) || (0 == *lpszSearch))
			return FALSE;

		CAnsi2Ucs cA2U;
		cA2U.GetString(lpszSearch, pFileOp->szFilename, sizeof(pFileOp->szFilename) / sizeof(TCHAR));
		wcscpy(pFileOp->szFilenameBuffer, pFileOp->szFilename);

		//����ļ����Ƿ���ȷ
		if(OP_CheckFile == pFileOp->op) {
#ifdef UNICODE
			if(0 == _tcsicmp(lpszFileName, pFileOp->szFilename))
#else
			CAnsi2Ucs cA2U;
			if(0 == wcsicmp(cA2U.GetString(lpszFileName), pFileOp->szFilename))
#endif
				return TRUE;
			else
				return FALSE;
		}

		return TRUE;
	}

	//�ֽ�ű��е�Ŀ¼
	void SepratePaths(FILEOP * pFileOp)
	{
		pFileOp->lpszSepratedPaths[0] = pFileOp->szFilenameBuffer;

		wchar_t *lpszSearch = pFileOp->szFilenameBuffer;
		int nPathDepthCount = 1;


		while(*lpszSearch) {

			wchar_t *test = wcschr(lpszSearch, L'\\');
			//����û�п��Ǵ���\\˫б�ܵ����
			if((L'\\' == *lpszSearch) || (L'/' == *lpszSearch)) {
				*lpszSearch = 0;
				++lpszSearch;
				pFileOp->lpszSepratedPaths[nPathDepthCount] = lpszSearch;
				++nPathDepthCount;
			}
			++lpszSearch;
		}

	}

	LPPCK_PATH_NODE LocationFileIndex(LPWSTR *lpszPaths, LPPCK_PATH_NODE lpNode)
	{
		LPCWSTR lpszSearchDir = *lpszPaths;

		if((NULL == lpszSearchDir) || (NULL == lpNode))
			return NULL;

		//ֱ������..Ŀ¼
		LPPCK_PATH_NODE lpNodeSearch = lpNode->next;

		while(1) {

			if(NULL == lpNodeSearch)
				return NULL;

			if(0 == wcsicmp(lpszSearchDir, lpNodeSearch->szName)) {

				//�Ƿ��Ѿ�ƥ����
				if(NULL == *(lpszPaths + 1)) {

					return lpNodeSearch;
				} else {
					return LocationFileIndex(lpszPaths + 1, lpNodeSearch->child);
				}

			}

			lpNodeSearch = lpNodeSearch->next;
		}

		return NULL;
	}

	void MarkFilterFlagToFileIndex(LPPCKINDEXTABLE	lpPckIndexTable, SCRIPTOP op)
	{
		switch(op) {

		case OP_Protect:
			lpPckIndexTable->isProtected = TRUE;
			break;

		case OP_Delete:
		case OP_Rmdir:
			if((!lpPckIndexTable->isInvalid) && (!lpPckIndexTable->isProtected)) {

				lpPckIndexTable->isToDeDelete = TRUE;
				lpPckIndexTable->isInvalid = TRUE;
			}

			break;

		}

	}

	//��һ�нű�����Ӧ�õ����ҵ����ļ��б���
	void MarkFilterFlagToNode(LPPCK_PATH_NODE lpNode, SCRIPTOP op)
	{
		lpNode = lpNode->child->next;

		while(NULL != lpNode) {
			if(NULL == lpNode->child) {
				MarkFilterFlagToFileIndex(lpNode->lpPckIndexTable, op);
			} else {
				MarkFilterFlagToNode(lpNode, op);
			}

			lpNode = lpNode->next;
		}

	}


#pragma region ApplyScript2IndexList,���ű�����Ӧ�õ��ļ��б���

	//���ű�����Ӧ�õ��ļ��б���
	BOOL ApplyScript2IndexList(VOID *pfirstFileOp, PCK_ALL_INFOS _DstPckAllInfo)
	{
		FILEOP *m_firstFileOp = (FILEOP *)pfirstFileOp;
		FILEOP * pFileOp = m_firstFileOp;

		//���������Ƿ����˴���
		BOOL bHasErrorHappend = FALSE;

		while(NULL != pFileOp) {

			if(OP_CheckFile != pFileOp->op) {

				//�ֽ�ű��е�Ŀ¼
				SepratePaths(pFileOp);

				//��λ�ļ�����
				LPPCK_PATH_NODE lpFoundNode = LocationFileIndex(pFileOp->lpszSepratedPaths, _DstPckAllInfo.lpRootNode->child);
				if(NULL == lpFoundNode) {

					m_PckLog.PrintLogW(TEXT("�ѽ����ű�ʧ����: %s, ����..."), pFileOp->szFilename);
					bHasErrorHappend = TRUE;

				} else {
					if(NULL != lpFoundNode->child) {

						MarkFilterFlagToNode(lpFoundNode, pFileOp->op);
					} else {
						MarkFilterFlagToFileIndex(lpFoundNode->lpPckIndexTable, pFileOp->op);
					}
				}

			}
			pFileOp = pFileOp->next;
		}

		return (!bHasErrorHappend);
	}

#pragma endregion

	BOOL ParseScript(LPCTSTR lpszScriptFile, PCK_ALL_INFOS &_DstPckAllInfo)
	{

		CHAR	szLineAnsi[MAX_LINE_LENGTH];
		char	szLine[MAX_LINE_LENGTH];
		UINT	uiStringLength;
		BOOL	bErrorOccured, bEndOfFile;
		SCRIPTBUFFER sfvBuf = { 0 };
		FILEOP * pFileOp;

		BYTE   * lpBufferToRead;
		CMapViewFileRead	cFileRead;
		LinkList<FILEOP> m_firstFileOp;

		m_PckLog.PrintLogI("��ʼ�����ű�...");

		//��ȡ�ļ������ַ�
		if(NULL == (lpBufferToRead = cFileRead.OpenMappingAndViewAllRead(lpszScriptFile))) {

			return FALSE;
		}

		sfvBuf.dwSize = (DWORD)cFileRead.GetFileSize();
		//�������10MB���˳�
		if(MAX_SCRIPT_SIZE < sfvBuf.dwSize)
			return FALSE;

		if(NULL == (sfvBuf.buffer = (BYTE*)malloc(sfvBuf.dwSize)))
			return FALSE;

		memcpy(sfvBuf.buffer, lpBufferToRead, sfvBuf.dwSize);

		m_firstFileOp.insertNext();
		pFileOp = m_firstFileOp.last();

		//�����ȡ������
		sfvBuf.bufpos = sfvBuf.buffer;
		GetNextLine(&sfvBuf, szLineAnsi, MAX_LINE_LENGTH, &uiStringLength, &bErrorOccured, &bEndOfFile);

		if(bErrorOccured) {
			m_PckLog.PrintLogEL("�޷���ȡ SFV �ļ�", __FILE__, __FUNCTION__, __LINE__);
			free(sfvBuf.buffer);
			return FALSE;
		}

		//��¼�Ѿ���ȡ���ڶ�������
		int iReadLineNumber = 1;
		//���������Ƿ����˴���
		BOOL bHasErrorHappend = FALSE;

		while(!(bEndOfFile && uiStringLength == 0)) {

			if(uiStringLength > 5) {
				strcpy_s(szLine, MAX_LINE_LENGTH, szLineAnsi);

				//ɾ��β������Ŀո�
				while((szLine[uiStringLength - 1] == TEXT(' ')) && (uiStringLength > 8)) {
					szLine[uiStringLength - 1] = NULL;
					uiStringLength--;
				}

				//�Ŷ�ע���кͿ���
				if((szLine[0] != TEXT(';')) && (szLine[0] != TEXT('\0'))) {

					//һ�нű���Ϊ�����֣��������ļ���
					if(ParseOneLine(pFileOp, szLine, _DstPckAllInfo.szFileTitle)) {

						m_firstFileOp.insertNext();
						pFileOp = m_firstFileOp.last();

					} else {

						m_PckLog.PrintLogW("�ű�����ʧ������%d: %s, ����...", iReadLineNumber, szLine);
						bHasErrorHappend = TRUE;
					}
				}
			}

			GetNextLine(&sfvBuf, szLineAnsi, MAX_LINE_LENGTH, &uiStringLength, &bErrorOccured, &bEndOfFile);
			if(bErrorOccured) {
				m_PckLog.PrintLogEL("�޷���ȡ SFV �ļ�", __FILE__, __FUNCTION__, __LINE__);
				free(sfvBuf.buffer);
				return FALSE;
			}

			++iReadLineNumber;

		}
		free(sfvBuf.buffer);


		BOOL rtn = FALSE;

		//������Ӧ����tree��
		if(!bHasErrorHappend)
			rtn = ApplyScript2IndexList(m_firstFileOp.first(), _DstPckAllInfo);

		if(!rtn) {
			ResetRebuildFilterInIndexList(_DstPckAllInfo);
			m_PckLog.PrintLogI("�����ű�ʧ��");
		} else {
			m_PckLog.PrintLogI("�����ű��ɹ�");
		}

		return rtn;
	}


	//������ؽ���ʱ����Ҫ��ȡ�Ĺ�����Ϣ
	void ResetRebuildFilterInIndexList(PCK_ALL_INFOS &_DstPckAllInfo)
	{
		LPPCKINDEXTABLE lpPckIndexTable = _DstPckAllInfo.lpPckIndexTable;

		for(DWORD i = 0;i < _DstPckAllInfo.dwFileCount;++i) {

			if(lpPckIndexTable->isToDeDelete) {
				lpPckIndexTable->isInvalid = FALSE;
			}
			lpPckIndexTable->isProtected = lpPckIndexTable->isToDeDelete = FALSE;
			++lpPckIndexTable;
		}
	}
}

BOOL CPckClass::ParseScript(LPCTSTR lpszScriptFile)
{
	return NPckClassRebuildFilter::ParseScript(lpszScriptFile, m_PckAllInfo);
}