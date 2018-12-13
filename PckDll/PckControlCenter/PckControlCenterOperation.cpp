
//////////////////////////////////////////////////////////////////////
// PckControlCenterOperation.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "ZupClass.h"
#include "PckClassZlib.h"
#include "PckClassLog.h"


void CPckControlCenter::New()
{
	Close();
	m_lpClassPck = new CPckClass(&cParams);
}

#pragma region �򿪹ر��ļ�

FMTPCK	CPckControlCenter::GetPckTypeFromFilename(LPCTSTR lpszFile)
{
	size_t nFileLength = _tcsnlen(lpszFile, MAX_PATH);

	if(0 == _tcsicmp(lpszFile + nFileLength - 4, TEXT(".pck"))) {
		return FMTPCK_PCK;
	} else if(0 == _tcsicmp(lpszFile + nFileLength - 4, TEXT(".zup"))) {
		return FMTPCK_ZUP;
	}
	return FMTPCK_UNKNOWN;
}

BOOL CPckControlCenter::Open(LPCTSTR lpszFile)
{
	//�ж��ļ���ʽ
	FMTPCK emunFileFormat = GetPckTypeFromFilename(lpszFile);

	while(1) {

		Close();
		switch(emunFileFormat) {
		case FMTPCK_ZUP:
			m_lpClassPck = new CZupClass(&cParams);
			break;
		case FMTPCK_PCK:
		default:
			m_lpClassPck = new CPckClass(&cParams);
			break;
		}

		m_lpPckLog->PrintLogI(TEXT(TEXT_LOG_OPENFILE), lpszFile);

		if(m_lpClassPck->Init(lpszFile)) {

			m_emunFileFormat = emunFileFormat;

			m_lpPckRootNode = m_lpClassPck->GetPckPathNode();

			//�򿪳ɹ���ˢ�±���
			pFeedbackCallBack(pTag, PCK_FILE_OPEN_SUCESS, NULL, (LPARAM)(m_lpClassPck->GetPckVersion()->name));
			return TRUE;

		} else {

			return FALSE;
		}
	}
}

void CPckControlCenter::Close()
{
	if(NULL != m_lpClassPck) {

		if(IsValidPck())
			m_lpPckLog->PrintLogI(TEXT_LOG_CLOSEFILE);

		delete m_lpClassPck;
		m_lpClassPck = NULL;

		m_lpPckRootNode = NULL;
	}

	m_emunFileFormat = FMTPCK_UNKNOWN;

	//��ղ���
	Reset();

	//�ر��ļ���ˢ�±���
	pFeedbackCallBack(pTag, PCK_FILE_CLOSE, NULL, NULL);
}

#pragma endregion

#pragma region �������ڵ�

BOOL CPckControlCenter::RenameEntry(PCK_UNIFIED_FILE_ENTRY* lpFileEntry, const wchar_t* lpszReplaceString)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	int entryType = lpFileEntry->entryType;
	if (PCK_ENTRY_TYPE_NODE == entryType) {

		return m_lpClassPck->RenameIndex((LPPCK_PATH_NODE)lpFileEntry, lpszReplaceString);
	}
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entryType)) {

		return m_lpClassPck->RenameNode((LPPCK_PATH_NODE)lpFileEntry, lpszReplaceString);
	}
	else if (PCK_ENTRY_TYPE_INDEX == entryType)
	{
		return m_lpClassPck->RenameIndex((LPPCKINDEXTABLE)lpFileEntry, lpszReplaceString);
	}
	else {
		return FALSE;
	}
}
BOOL CPckControlCenter::RenameSubmit()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->RenameFilename();
}

#pragma endregion

#pragma region Ԥ����ѹ�ļ�

//Ԥ���ļ�
BOOL CPckControlCenter::GetSingleFileData(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry, char *buffer, size_t sizeOfBuffer)
{
	if ((NULL == m_lpClassPck) || (NULL == lpFileEntry))
		return FALSE;

	const PCKINDEXTABLE* lpPckFileIndexTable = NULL;
	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		lpPckFileIndexTable = (LPPCKINDEXTABLE)lpFileEntry;
	else
		lpPckFileIndexTable = ((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable;

	return m_lpClassPck->GetSingleFileData(NULL, lpPckFileIndexTable, buffer, sizeOfBuffer);
}

//��ѹ�ļ�
BOOL CPckControlCenter::ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, const wchar_t *lpszDestDirectory)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ExtractFiles(lpFileEntryArray, nEntryCount, lpszDestDirectory);
}

BOOL CPckControlCenter::ExtractAllFiles(const wchar_t *lpszDestDirectory)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ExtractAllFiles(lpszDestDirectory);
}
#if 0
BOOL CPckControlCenter::ExtractFiles(LPCTSTR lpszFilePathSrc, const wchar_t *lpszDestDirectory, const wchar_t *lpszFileToExtract)
{
	CPckControlCenter* new_handle = new CPckControlCenter();
	BOOL rtn = FALSE;

	if (new_handle->Open(lpszFilePathSrc)) {
		if (new_handle->IsValidPck()) {

			const PCK_UNIFIED_FILE_ENTRY* lpFileEntry = new_handle->GetFileEntryByPath(lpszFileToExtract);

			rtn = new_handle->ExtractFiles(&lpFileEntry, 1, lpszDestDirectory);
		}
	}

	delete new_handle;
	return rtn;
}

BOOL CPckControlCenter::ExtractAllFiles(LPCTSTR lpszFilePathSrc, const wchar_t *lpszDestDirectory)
{
	CPckControlCenter* new_handle = new CPckControlCenter();
	BOOL rtn = FALSE;

	if (new_handle->Open(lpszFilePathSrc)) {
		if (new_handle->IsValidPck()) {

			rtn = new_handle->ExtractAllFiles(lpszDestDirectory);
		}
	}

	delete new_handle;
	return rtn;
}
#endif
#pragma endregion

#pragma region �ؽ�pck�ļ�
//�ؽ�pck�ļ�
BOOL CPckControlCenter::ParseScript(LPCTSTR lpszScriptFile)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ParseScript(lpszScriptFile);
}

BOOL CPckControlCenter::RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->RebuildPckFile(szRebuildPckFile, bUseRecompress);
}

BOOL CPckControlCenter::RebuildPckFileWithScript(LPCTSTR lpszScriptFile, LPCTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (NULL != lpszScriptFile) {
		if (!ParseScript(lpszScriptFile))
			return FALSE;
	}

	return RebuildPckFile(szRebuildPckFile, bUseRecompress);
}

#pragma endregion

#pragma region ����ļ��б����ѹ��ʱʹ�õ�vector����
void CPckControlCenter::StringArrayReset()
{
	lpszFilePathToAdd.clear();
}

void CPckControlCenter::StringArrayAppend(const wchar_t * lpszFilePath)
{
	lpszFilePathToAdd.push_back(lpszFilePath);
}

#pragma endregion

#pragma region �½�/����pck�ļ�

BOOL CPckControlCenter::UpdatePckFileSubmit(LPCTSTR szPckFile, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	BOOL rtn = FALSE;

	if (0 != lpszFilePathToAdd.size()) {
		rtn = m_lpClassPck->UpdatePckFile(szPckFile, lpszFilePathToAdd, (const PCK_PATH_NODE*)lpFileEntry);
		StringArrayReset();
	}
	return rtn;
}
#if 0
BOOL CPckControlCenter::AddFileToPckFile(LPCTSTR lpszFilePathSrc, LPCTSTR szPckFile, const wchar_t *lpszPathInPckToAdd)
{
	CPckControlCenter* new_handle = new CPckControlCenter();
	BOOL rtn = FALSE;

	if (new_handle->Open(szPckFile)) {
		if (new_handle->IsValidPck()) {

			const PCK_UNIFIED_FILE_ENTRY* lpFileEntry = new_handle->GetFileEntryByPath(lpszPathInPckToAdd);

			new_handle->StringArrayReset();
			new_handle->StringArrayAppend(lpszFilePathSrc);

			rtn = new_handle->UpdatePckFileSubmit(szPckFile, lpFileEntry);
		}
	}

	delete new_handle;

	return rtn;
}

BOOL CPckControlCenter::CreatePckFile(LPCTSTR lpszFilePathSrc, LPCTSTR szPckFile, int _versionId)
{

	CPckControlCenter* new_handle = new CPckControlCenter();
	BOOL rtn = FALSE;

	new_handle->SetPckVersion(_versionId);
	new_handle->StringArrayReset();
	new_handle->StringArrayAppend(lpszFilePathSrc);

	rtn = new_handle->UpdatePckFileSubmit(szPckFile, NULL);

	delete new_handle;

	return rtn;

}
#endif
#pragma endregion

#pragma region ɾ���ڵ�
//ɾ��һ���ڵ�
BOOL CPckControlCenter::DeleteEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	if ((NULL == m_lpClassPck) || (NULL == lpFileEntry))
		return FALSE;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		m_lpClassPck->DeleteNode((LPPCKINDEXTABLE)lpFileEntry);
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		m_lpClassPck->DeleteNode((LPPCK_PATH_NODE)lpFileEntry);
	else
		m_lpClassPck->DeleteNode(((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable);
	return TRUE;
}

//�ύ
BOOL CPckControlCenter::DeleteEntrySubmit()
{
	return RenameSubmit();
}

#pragma endregion


#pragma region �汾���

BOOL CPckControlCenter::SetPckVersion(int verID)
{
	return m_lpClassPck->SetSavePckVersion(verID);
}

int	CPckControlCenter::GetPckVersion()
{
	return m_lpClassPck->GetPckVersion()->id;
}

//��ȡ��ǰ��������
LPCTSTR	CPckControlCenter::GetCurrentVersionName()
{
	if (NULL == m_lpClassPck)
		return NULL;

	return m_lpClassPck->GetPckVersion()->name;
}

DWORD	CPckControlCenter::GetVersionCount()
{
	return CPckClassVersionDetect::GetPckVersionCount();
}

const wchar_t*	CPckControlCenter::GetVersionNameById(int verID)
{
	return CPckClassVersionDetect::GetPckVersionNameById(verID);
}

int CPckControlCenter::AddVersionAlgorithmId(int AlgorithmId, int Version)
{
	return CPckClassVersionDetect::AddPckVersion(AlgorithmId, Version);
}

#pragma endregion

#pragma region ���ݵĻ�ԭ����
#if 0
void CPckControlCenter::CreateRestoreData()
{
	if(FMTPCK_PCK == m_emunFileFormat) {
		if(!m_lpClassPck->GetPckBasicInfo(&m_RestoreInfomation)) {
			m_lpPckLog->PrintLogE(TEXT_ERROR_GET_RESTORE_DATA);
		}
	}
}

BOOL CPckControlCenter::RestoreData(LPCTSTR lpszFile, FMTPCK emunFileFormat)
{
	BOOL rtn = FALSE;
	if(FMTPCK_PCK == emunFileFormat) {
		if(m_RestoreInfomation.isValid) {

			//if(IDYES == MessageBoxA(m_hWndMain, TEXT_ERROR_OPEN_AFTER_UPDATE, TEXT_ERROR, MB_YESNO | MB_ICONHAND)) {

			if(PCK_FEEDBACK_YES == pFeedbackCallBack(pTag, PCK_FILE_NEED_RESTORE, NULL, NULL)) {
				if(0 == _tcsicmp(m_RestoreInfomation.szFile, lpszFile)) {

					if(!m_lpClassPck->SetPckBasicInfo(&m_RestoreInfomation))
						m_lpPckLog->PrintLogE(TEXT_ERROR_RESTORING);
					else {
						m_lpPckLog->PrintLogI(TEXT_LOG_RESTOR_OK);
						rtn = TRUE;
					}
				}
			}
			DeleteRestoreData();
		}
	}
	return rtn;
}

void CPckControlCenter::DeleteRestoreData()
{
	if(m_RestoreInfomation.isValid) {
		if(NULL != m_RestoreInfomation.lpIndexTailBuffer) {

			free(m_RestoreInfomation.lpIndexTailBuffer);
			m_RestoreInfomation.lpIndexTailBuffer = NULL;
		}
		m_RestoreInfomation.isValid = FALSE;
	}
}
#endif
#pragma endregion