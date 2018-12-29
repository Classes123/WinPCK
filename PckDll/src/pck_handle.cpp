#include "pck_handle.h"
#include "PckControlCenter.h"

CPckControlCenter this_handle;

BOOL checkIfValidPck()
{
	return this_handle.IsValidPck();
}

BOOL checkIfWorking()
{
	return this_handle.isThreadWorking();
}

BOOL checkIfWriteAllowed()
{
	if (checkIfValidPck())
		return !this_handle.isThreadWorking();

	return FALSE;
}

WINPCK_API CLPSTR pck_version()
{
	return WINPCK_VERSION;
}

WINPCK_API PCKRTN pck_open(CLPWSTR lpszFile)
{
	if (checkIfWorking())
		return WINPCK_WORKING;

	if (this_handle.Open(lpszFile))
	{
		return WINPCK_OK;
	}
	else {
		return WINPCK_ERROR;
	}
}

WINPCK_API PCKRTN pck_close()
{
	if (checkIfWorking()) {
		this_handle.ForceBreakThreadWorking();


		//�ȴ�60��
		for (int i = 0; i < 120; i++) {

			if (!checkIfWorking())
				break;
			Sleep(500);
		}
	}

	if (!checkIfWorking()) {

		this_handle.New();
		return WINPCK_OK;
	}

	return WINPCK_WORKING;

}

WINPCK_API int pck_getVersion()
{
	if (!checkIfValidPck())
		return -1;

	return this_handle.GetPckVersion();

}

WINPCK_API PCKRTN pck_setVersion(int verID)
{
	if (checkIfWorking())
		return WINPCK_WORKING;

	if (this_handle.SetPckVersion(verID))
	{
		return WINPCK_OK;
	}
	else {
		return WINPCK_ERROR;
	}
}

WINPCK_API DWORD pck_getVersionCount()
{
	return CPckControlCenter::GetVersionCount();
}

WINPCK_API CLPWSTR pck_getVersionNameById(int verID)
{
	return CPckControlCenter::GetVersionNameById(verID);
}

//ֱ�����һ��AlgorithmId
WINPCK_API int pck_addVersionAlgorithmId(int AlgorithmId, int Version)
{
	return CPckControlCenter::AddVersionAlgorithmId(AlgorithmId, Version);
}

//��ȡ��ǰ��������
WINPCK_API CLPWSTR pck_GetCurrentVersionName()
{
	return this_handle.GetCurrentVersionName();
}

//��Ч��
WINPCK_API int	pck_IsValidPck()
{
	return this_handle.IsValidPck();
}

//��ȡnode·��
WINPCK_API BOOL	pck_getNodeRelativePath(LPWSTR _out_szCurrentNodePathString, CLPPUFE lpFileEntry)
{
	return CPckControlCenter::GetCurrentNodeString(_out_szCurrentNodePathString, lpFileEntry);
}

WINPCK_API CLPPUFE pck_getFileEntryByPath(LPWSTR _in_szCurrentNodePathString)
{
	if (!checkIfValidPck())
		return NULL;

	return this_handle.GetFileEntryByPath(_in_szCurrentNodePathString);
}

WINPCK_API CLPPUFE pck_getRootNode()
{
	if (!checkIfValidPck())
		return NULL;

	return this_handle.GetRootNode();
}

//�ļ���С
WINPCK_API QWORD	pck_filesize()
{
	if (!checkIfValidPck())
		return -1;

	return this_handle.GetPckSize();
}

WINPCK_API QWORD	pck_file_data_area_size()
{
	if (!checkIfValidPck())
		return -1;

	return this_handle.GetPckDataAreaSize();
}

WINPCK_API QWORD	pck_file_redundancy_data_size()
{
	if (!checkIfValidPck())
		return -1;

	return this_handle.GetPckRedundancyDataSize();
}

WINPCK_API DWORD	pck_filecount()
{
	if (!checkIfValidPck())
		return -1;

	return this_handle.GetPckFileCount();
}

WINPCK_API QWORD	pck_getFileSizeInEntry(CLPPUFE lpFileEntry)
{
	return CPckControlCenter::GetFileSizeInEntry(lpFileEntry);
}

WINPCK_API QWORD	pck_getCompressedSizeInEntry(CLPPUFE lpFileEntry)
{
	return CPckControlCenter::GetCompressedSizeInEntry(lpFileEntry);
}

WINPCK_API DWORD	pck_getFoldersCountInEntry(CLPPUFE lpFileEntry)
{
	return CPckControlCenter::GetFoldersCountInEntry(lpFileEntry);
}

WINPCK_API DWORD	pck_getFilesCountInEntry(CLPPUFE lpFileEntry)
{
	return CPckControlCenter::GetFilesCountInEntry(lpFileEntry);
}

//��ǰ�ڵ��ļ�����ǰ����
WINPCK_API DWORD	pck_getFilelenBytesOfEntry(CLPPUFE lpFileEntry)
{
	return CPckControlCenter::GetFilelenBytesOfEntry(lpFileEntry);
}

//��ǰ�ڵ��ļ�����󳤶�-��ǰ����
WINPCK_API DWORD	pck_getFilelenLeftBytesOfEntry(CLPPUFE lpFileEntry)
{
	return CPckControlCenter::GetFilelenLeftBytesOfEntry(lpFileEntry);
}

WINPCK_API QWORD	pck_getFileOffset(CLPPUFE lpFileEntry)
{
	return CPckControlCenter::GetFileOffset(lpFileEntry);
}

//���ø�����Ϣ
WINPCK_API CLPSTR pck_GetAdditionalInfo()
{
	if (!checkIfValidPck())
		return NULL;

	return this_handle.GetAdditionalInfo();
}

WINPCK_API DWORD	pck_GetAdditionalInfoMaxSize()
{
	return CPckControlCenter::GetAdditionalInfoMaxSize();
}

WINPCK_API PCKRTN pck_SetAdditionalInfo(CLPSTR lpszAdditionalInfo)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	if (this_handle.SetAdditionalInfo(lpszAdditionalInfo))
	{
		return WINPCK_OK;
	}
	else {
		return WINPCK_ERROR;
	}
}

//�Ƿ���֧�ָ��µ��ļ�
WINPCK_API BOOL	pck_isSupportAddFileToPck()
{
	return this_handle.isSupportAddFileToPck();
}

//������һ���ڵ�
WINPCK_API PCKRTN pck_RenameEntry(LPPUFE lpFileEntry, CLPWSTR lpszReplaceString)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.RenameEntry(lpFileEntry, lpszReplaceString) ? WINPCK_OK : WINPCK_ERROR;
}

//�������ļ�
WINPCK_API PCKRTN	pck_RenameSubmit()
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.RenameSubmit() ? WINPCK_OK : WINPCK_ERROR;
}

WINPCK_API PCKRTN do_RenameEntry(CLPWSTR  szPckFile, CLPWSTR lpFullPathToRename, CLPWSTR lpszReplaceString)
{
	BOOL rtn = FALSE;

	if (this_handle.Open(szPckFile)) {
		if (this_handle.IsValidPck()) {

			LPPUFE lpFileEntry = (LPPUFE)this_handle.GetFileEntryByPath(lpFullPathToRename);

			if (NULL == lpFileEntry) {
				this_handle.New();
				return WINPCK_NOTFOUND;
			}

			if(!this_handle.RenameEntry(lpFileEntry, lpszReplaceString))
			{
				this_handle.New();
				return WINPCK_ERROR;
			}

			rtn = this_handle.RenameSubmit();
		}
	}
	this_handle.New();

	return rtn ? WINPCK_OK : WINPCK_ERROR;
}

//Ԥ���ļ�
WINPCK_API PCKRTN pck_GetSingleFileData(CLPPUFE lpFileEntry, LPSTR _inout_buffer, size_t _in_sizeOfBuffer)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.GetSingleFileData(lpFileEntry, _inout_buffer, _in_sizeOfBuffer) ? WINPCK_OK : WINPCK_ERROR;
}

//��ѹ�ļ�
WINPCK_API PCKRTN	pck_ExtractFilesByEntrys(CLPPUFE* lpFileEntryArray, int nEntryCount, CLPWSTR lpszDestDirectory)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.ExtractFiles(lpFileEntryArray, nEntryCount, lpszDestDirectory) ? WINPCK_OK : WINPCK_ERROR;
}

WINPCK_API PCKRTN	pck_ExtractAllFiles(CLPWSTR lpszDestDirectory)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.ExtractAllFiles(lpszDestDirectory) ? WINPCK_OK : WINPCK_ERROR;
}

WINPCK_API PCKRTN	do_ExtractPartFiles(CLPWSTR lpszSrcPckFile, CLPWSTR lpszDestDirectory, CLPWSTR lpszFileToExtract)
{
	BOOL rtn = FALSE;

	if (this_handle.Open(lpszSrcPckFile)) {
		if (this_handle.IsValidPck()) {

			CLPPUFE lpFileEntry = this_handle.GetFileEntryByPath(lpszFileToExtract);

			if (NULL == lpFileEntry) {
				this_handle.New();
				return WINPCK_NOTFOUND;
			}

			rtn = this_handle.ExtractFiles(&lpFileEntry, 1, lpszDestDirectory);
		}
	}
	this_handle.New();

	return rtn ? WINPCK_OK : WINPCK_ERROR;
}
WINPCK_API PCKRTN	do_ExtractAllFiles(CLPWSTR lpszSrcPckFile, CLPWSTR lpszDestDirectory)
{
	BOOL rtn = FALSE;

	if (this_handle.Open(lpszSrcPckFile)) {
		if (this_handle.IsValidPck()) {

			rtn = this_handle.ExtractAllFiles(lpszDestDirectory);
		}
	}

	this_handle.New();
	return rtn ? WINPCK_OK : WINPCK_ERROR;
}

//�ؽ�pck�ļ�
WINPCK_API PCKRTN	pck_TestScript(CLPWSTR lpszScriptFile)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.TestScript(lpszScriptFile) ? WINPCK_OK : WINPCK_ERROR;
}

WINPCK_API PCKRTN	pck_RebuildPckFile(CLPWSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.RebuildPckFile(nullptr, szRebuildPckFile, bUseRecompress) ? WINPCK_OK : WINPCK_ERROR;
}

WINPCK_API PCKRTN	pck_RebuildPckFileWithScript(CLPWSTR lpszScriptFile, CLPWSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.RebuildPckFile(lpszScriptFile, szRebuildPckFile, bUseRecompress) ? WINPCK_OK : WINPCK_ERROR;
}

WINPCK_API PCKRTN do_RebuildPckFileWithScript(CLPWSTR szSrcPckFile, CLPWSTR lpszScriptFile, CLPWSTR szDstRebuildPckFile, BOOL bUseRecompress, int level)
{
	if (checkIfWorking())
		return WINPCK_WORKING;

	BOOL rtn = FALSE;

	if (this_handle.Open(szSrcPckFile)) {
		if (this_handle.IsValidPck()) {

			if (bUseRecompress)
				this_handle.setCompressLevel(level);

			rtn = this_handle.RebuildPckFile(lpszScriptFile, szDstRebuildPckFile, bUseRecompress);
		}
	}

	this_handle.New();
	return rtn ? WINPCK_OK : WINPCK_ERROR;
}

//��Ϸ����
WINPCK_API PCKRTN pck_StripPck(CLPWSTR szStripedPckFile, int flag)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.StripPck(szStripedPckFile, flag) ? WINPCK_OK : WINPCK_ERROR;
}

//�½�������pck�ļ�
WINPCK_API void	pck_StringArrayReset()
{
	return this_handle.StringArrayReset();
}

WINPCK_API void	pck_StringArrayAppend(CLPWSTR lpszFilePath)
{
	return this_handle.StringArrayAppend(lpszFilePath);
}

WINPCK_API PCKRTN	pck_UpdatePckFileSubmit(CLPWSTR szPckFile, CLPPUFE lpFileEntry)
{
	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.UpdatePckFileSubmit(szPckFile, lpFileEntry) ? WINPCK_OK : WINPCK_ERROR;
}

//����ļ���pck
WINPCK_API PCKRTN	do_AddFileToPckFile(CLPWSTR lpszFilePathSrc, CLPWSTR szPckFile, CLPWSTR lpszPathInPckToAdd, int level)
{

	BOOL rtn = FALSE;

	if (this_handle.Open(szPckFile)) {
		if (this_handle.IsValidPck()) {

			CLPPUFE lpFileEntry = this_handle.GetFileEntryByPath(lpszPathInPckToAdd);

			if (NULL == lpFileEntry) {
				this_handle.New();
				return WINPCK_NOTFOUND;
			}

			this_handle.StringArrayReset();
			this_handle.StringArrayAppend(lpszFilePathSrc);

			this_handle.setCompressLevel(level);

			rtn = this_handle.UpdatePckFileSubmit(szPckFile, lpFileEntry);
		}
	}

	this_handle.New();

	return rtn ? WINPCK_OK : WINPCK_ERROR;
}

//�����µ�pck�ļ�
WINPCK_API PCKRTN	do_CreatePckFile(CLPWSTR lpszFilePathSrc, CLPWSTR szPckFile, int _versionId, int level)
{

	BOOL rtn = FALSE;

	this_handle.SetPckVersion(_versionId);
	this_handle.StringArrayReset();
	this_handle.StringArrayAppend(lpszFilePathSrc);

	this_handle.setCompressLevel(level);

	rtn = this_handle.UpdatePckFileSubmit(szPckFile, NULL);

	this_handle.New();

	return rtn ? WINPCK_OK : WINPCK_ERROR;
}

//ɾ��һ���ڵ�
WINPCK_API PCKRTN pck_DeleteEntry(CLPPUFE lpFileEntry)
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.DeleteEntry(lpFileEntry) ? WINPCK_OK : WINPCK_ERROR;
}

//�ύ
WINPCK_API PCKRTN pck_DeleteEntrySubmit()
{
	if (!checkIfValidPck())
		return WINPCK_INVALIDPCK;

	if (checkIfWorking())
		return WINPCK_WORKING;

	return this_handle.DeleteEntrySubmit() ? WINPCK_OK : WINPCK_ERROR;
}

WINPCK_API PCKRTN	 do_DeleteFromPck(CLPWSTR szSrcPckFile, int count, ...)
{

	if (0 == count)
		return WINPCK_ERROR;

	BOOL rtn = FALSE;

	if (this_handle.Open(szSrcPckFile)) {
		if (this_handle.IsValidPck()) {

			va_list	ap;
			va_start(ap, count);

			for (int i = 0; i < count; i++)
			{

				CLPWSTR lpszPathInPckToDel = va_arg(ap, LPWSTR);
				CLPPUFE lpFileEntry = this_handle.GetFileEntryByPath(lpszPathInPckToDel);

				if (NULL != lpFileEntry)
					this_handle.DeleteEntry(lpFileEntry);

			}

			va_end(ap);
			rtn = this_handle.DeleteEntrySubmit();
		}
	}

	this_handle.New();
	return rtn ? WINPCK_OK : WINPCK_ERROR;
}

//��ѯ
WINPCK_API DWORD pck_searchByName(CLPWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (!checkIfValidPck())
		return 0;

	if (checkIfWorking())
		return 0;

	return this_handle.SearchByName(lpszSearchString, _in_param, _showListCallback);
}

WINPCK_API DWORD pck_listByNode(CLPPUFE lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	return CPckControlCenter::ListByNode(lpFileEntry, _in_param, _showListCallback);
}

WINPCK_API DWORD do_listPathInPck(CLPWSTR szSrcPckFile, CLPWSTR lpszListPath, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (NULL == lpszListPath)
		return WINPCK_ERROR;

	BOOL rtn = FALSE;

	if (this_handle.Open(szSrcPckFile)) {
		if (this_handle.IsValidPck()) {


			CLPPUFE lpFileEntry = this_handle.GetFileEntryByPath(lpszListPath);

			if (NULL == lpFileEntry) {
				this_handle.New();

				return WINPCK_NOTFOUND;
			}

			rtn = CPckControlCenter::ListByNode(lpFileEntry, _in_param, _showListCallback);
		}
	}

	this_handle.New();
	return rtn ? WINPCK_OK : WINPCK_ERROR;
}

WINPCK_API BOOL pck_isThreadWorking()
{
	return checkIfWorking();
}

WINPCK_API void	pck_forceBreakThreadWorking()
{
	if (!checkIfWorking())
		return;

	return this_handle.ForceBreakThreadWorking();
}

WINPCK_API BOOL	pck_isLastOptSuccess()
{
	return this_handle.isLastOptSuccess();
}

WINPCK_API BOOL	pck_getLastErrorMsg()
{
	return this_handle.GetLastErrorMsg();
}

//�ڴ�ռ��
WINPCK_API DWORD	pck_getMTMemoryUsed()
{
	return this_handle.getMTMemoryUsed();
}

WINPCK_API DWORD	pck_getMTMaxMemory()
{
	return this_handle.getMTMaxMemory();
}

WINPCK_API void		pck_setMTMaxMemory(DWORD dwMTMaxMemory)
{
	if (checkIfWorking())
		return;

	return this_handle.setMTMaxMemory(dwMTMaxMemory);
}

WINPCK_API DWORD	pck_getMaxMemoryAllowed()
{
	return CPckControlCenter::getMaxMemoryAllowed();
}

//�߳���
WINPCK_API DWORD	pck_getMaxThreadUpperLimit()
{
	return CPckControlCenter::getMaxThreadUpperLimit();
}

WINPCK_API DWORD	pck_getMaxThread()
{
	return this_handle.getMaxThread();
}

WINPCK_API void		pck_setMaxThread(DWORD dwThread)
{
	if (checkIfWorking())
		return;

	return this_handle.setMaxThread(dwThread);
}

//ѹ���ȼ�
WINPCK_API DWORD	pck_getMaxCompressLevel()
{
	return CPckControlCenter::getMaxCompressLevel();
}

WINPCK_API DWORD	pck_getDefaultCompressLevel()
{
	return CPckControlCenter::getDefaultCompressLevel();
}

WINPCK_API DWORD	pck_getCompressLevel()
{
	return this_handle.getCompressLevel();
}

WINPCK_API void		pck_setCompressLevel(DWORD dwCompressLevel)
{
	if (checkIfWorking())
		return;

	return this_handle.setCompressLevel(dwCompressLevel);
}

WINPCK_API DWORD	pck_getUIProgress()
{
	return this_handle.getUIProgress();
}

WINPCK_API void		pck_setUIProgress(DWORD dwUIProgress)
{
	return this_handle.setUIProgress(dwUIProgress);
}

WINPCK_API DWORD	pck_getUIProgressUpper()
{
	return this_handle.getUIProgressUpper();
}

//���/�����ļ����ؽ���嵥
WINPCK_API DWORD	pck_getUpdateResult_OldFileCount()
{
	return this_handle.GetUpdateResult_OldFileCount();
}

WINPCK_API DWORD	pck_getUpdateResult_PrepareToAddFileCount()
{
	return this_handle.GetUpdateResult_PrepareToAddFileCount();
}

WINPCK_API DWORD	pck_getUpdateResult_ChangedFileCount()
{
	return this_handle.GetUpdateResult_ChangedFileCount();
}

WINPCK_API DWORD	pck_getUpdateResult_DuplicateFileCount()
{
	return this_handle.GetUpdateResult_DuplicateFileCount();
}

WINPCK_API DWORD	pck_getUpdateResult_FinalFileCount()
{
	return this_handle.GetUpdateResult_FinalFileCount();
}

//��־
WINPCK_API void		log_regShowFunc(ShowLogW _ShowLogW)
{
	CPckControlCenter::regShowFunc(_ShowLogW);
}

WINPCK_API const char	log_getLogLevelPrefix(int _loglevel)
{
	return CPckControlCenter::getLogLevelPrefix(_loglevel);
}

WINPCK_API void		log_PrintA(const char chLevel, CLPSTR _fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckControlCenter::Print(chLevel, _fmt, ap);
	va_end(ap);
}

WINPCK_API void		log_PrintW(const char chLevel, CLPWSTR _fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckControlCenter::Print(chLevel, _fmt, ap);
	va_end(ap);
}

//�򿪡��رա���ԭ���¼�ע��
WINPCK_API void		pck_regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack)
{
	CPckControlCenter::regMsgFeedback(pTag, _FeedbackCallBack);
}