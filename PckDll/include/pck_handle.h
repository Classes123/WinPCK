#ifndef APSTUDIO_READONLY_SYMBOLS
#include "pck_dependencies.h"
#endif

#ifndef WINPCK_DLL_H
#define WINPCK_DLL_H

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

typedef const wchar_t * 	LPCWSTR;
typedef wchar_t *			LPWSTR;
typedef const char * 		LPCSTR;
typedef char *				LPSTR;

#define WINPCK_VERSION_NUMBER  1,31,0,3
#define WINPCK_VERSION        "1.31.0.3"

typedef enum _PCKRTN
{
	WINPCK_OK = 0,			/* �ɹ� */
	WINPCK_ERROR,			/* һ����� */
	WINPCK_WORKING,			/* ���������� */
	WINPCK_INVALIDPCK,		/* ��Ч��PCK�ļ���δ���ļ� */
	WINPCK_NOTFOUND,
}PCKRTN;

#ifdef __cplusplus
#define EXTERN_C       extern "C"
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END   }
#else
#define EXTERN_C       extern
#define EXTERN_C_START
#define EXTERN_C_END
#endif


#ifdef _WINDLL	//.dll
#define WINPCK_API EXTERN_C _declspec(dllexport) 
#elif defined(_DLL)	//.exe
#define WINPCK_API EXTERN_C _declspec(dllimport)
#else	//other
#define WINPCK_API EXTERN_C
#endif

WINPCK_API LPCSTR		pck_version();
WINPCK_API PCKRTN		pck_open(LPCTSTR  lpszFile);
WINPCK_API PCKRTN		pck_close();

//return -1 is invalid 
WINPCK_API int			pck_getVersion();
WINPCK_API PCKRTN		pck_setVersion(int verID);
WINPCK_API DWORD		pck_getVersionCount();
WINPCK_API LPCWSTR 		pck_getVersionNameById(int verID);
//AlgorithmId can not be -1, Version must be AFPCK_VERSION_202(0x00020002) or AFPCK_VERSION_203(0x00020003), return value is id , PCK_VERSION_INVALID = -1
WINPCK_API int 			pck_addVersionAlgorithmId(int AlgorithmId, int Version);


//��ȡ��ǰ��������
WINPCK_API LPCWSTR 		pck_GetCurrentVersionName();

//��Ч��
WINPCK_API BOOL			pck_IsValidPck();

//��ȡnode·��
WINPCK_API BOOL			pck_getNodeRelativePath(LPWSTR _out_szCurrentNodePathString, LPCENTRY lpFileEntry);
WINPCK_API LPCENTRY		pck_getFileEntryByPath(LPWSTR _in_szCurrentNodePathString);

//�ڵ��ѯ 
WINPCK_API LPCENTRY		pck_getRootNode();

//pck�ļ���С
WINPCK_API QWORD		pck_filesize();
//pck��Ч��������С
WINPCK_API QWORD		pck_file_data_area_size();
//�������ݴ�С
WINPCK_API QWORD		pck_file_redundancy_data_size();
//pck�ļ�����
WINPCK_API DWORD		pck_filecount();

//�ӽڵ��л�ȡ����
WINPCK_API QWORD		pck_getFileSizeInEntry(LPCENTRY lpFileEntry);
WINPCK_API QWORD		pck_getCompressedSizeInEntry(LPCENTRY lpFileEntry);
WINPCK_API DWORD		pck_getFoldersCountInEntry(LPCENTRY lpFileEntry);
WINPCK_API DWORD		pck_getFilesCountInEntry(LPCENTRY lpFileEntry);

//��ǰ�ڵ��ļ�����ǰ����
WINPCK_API DWORD		pck_getFilelenBytesOfEntry(LPCENTRY lpFileEntry);
//��ǰ�ڵ��ļ�����󳤶�-��ǰ����
WINPCK_API DWORD		pck_getFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);


//�����ļ���pck�ļ���ѹ�����ݵ�ƫ��ֵ
WINPCK_API QWORD		pck_getFileOffset(LPCENTRY lpFileEntry);

//�Ƿ���֧�ָ��µ��ļ�
WINPCK_API BOOL			pck_isSupportAddFileToPck();

//���ø�����Ϣ
WINPCK_API LPCSTR 		pck_GetAdditionalInfo();
WINPCK_API DWORD		pck_GetAdditionalInfoMaxSize();
//
WINPCK_API PCKRTN		pck_SetAdditionalInfo(LPCSTR  lpszAdditionalInfo);

//������һ���ڵ�
WINPCK_API PCKRTN		pck_RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString);
//�ύ
WINPCK_API PCKRTN		pck_RenameSubmit();
//use it like do_RenameEntry(L"x.pck", L"gfx\\test", L"testabc");
WINPCK_API PCKRTN		do_RenameEntry(LPCWSTR  szPckFile, LPCWSTR lpFullPathToRename, LPCWSTR lpszReplaceString);

//Ԥ���ļ�
WINPCK_API PCKRTN		pck_GetSingleFileData(LPCENTRY lpFileEntry, LPSTR _inout_buffer, size_t _in_sizeOfBuffer = 0);

//��ѹ�ļ�
WINPCK_API PCKRTN		pck_ExtractFilesByEntrys(LPCENTRY* lpFileEntryArray, int nEntryCount, LPCWSTR  lpszDestDirectory);
WINPCK_API PCKRTN		pck_ExtractAllFiles(LPCWSTR  lpszDestDirectory);
WINPCK_API PCKRTN		do_ExtractPartFiles(LPCWSTR  lpszSrcPckFile, LPCWSTR  lpszDestDirectory, LPCWSTR  lpszFileToExtract);
WINPCK_API PCKRTN		do_ExtractAllFiles(LPCWSTR  lpszSrcPckFile, LPCWSTR  lpszDestDirectory);

//�ؽ�pck�ļ�
WINPCK_API PCKRTN		pck_TestScript(LPCWSTR  lpszScriptFile);
WINPCK_API PCKRTN		pck_RebuildPckFile(LPCWSTR szRebuildPckFile, BOOL bUseRecompress);
//set lpszScriptFile = NULL to disable Script filter function
WINPCK_API PCKRTN		pck_RebuildPckFileWithScript(LPCWSTR  lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress);
WINPCK_API PCKRTN		do_RebuildPckFileWithScript(LPCWSTR szSrcPckFile, LPCWSTR  lpszScriptFile, LPCWSTR szDstRebuildPckFile, BOOL bUseRecompress, int level = 9);
//��Ϸ����
WINPCK_API PCKRTN		pck_StripPck(LPCWSTR szStripedPckFile, int flag);
WINPCK_API PCKRTN		do_StripPck(LPCWSTR szSrcPckFile, LPCWSTR szStripedPckFile, int flag, int level);

//�½�������pck�ļ�
WINPCK_API void			pck_StringArrayReset();
WINPCK_API void			pck_StringArrayAppend(LPCWSTR  lpszFilePath);
WINPCK_API PCKRTN		pck_UpdatePckFileSubmit(LPCWSTR  szPckFile, LPCENTRY lpFileEntry);
//����ļ���pck
WINPCK_API PCKRTN		do_AddFileToPckFile(LPCWSTR  lpszFilePathSrc, LPCWSTR  szDstPckFile, LPCWSTR  lpszPathInPckToAdd, int level = 9);
//�����µ�pck�ļ�
WINPCK_API PCKRTN		do_CreatePckFile(LPCWSTR  lpszFilePathSrc, LPCWSTR  szDstPckFile, int _versionId = 0, int level = 9);

//ɾ��һ���ڵ㣬ͨ���ύӦ�ø���
WINPCK_API PCKRTN		pck_DeleteEntry(LPCENTRY lpFileEntry);
//�ύ
WINPCK_API PCKRTN		pck_DeleteEntrySubmit();
//ͨ��pck�ļ�����Ҫɾ�����ļ���ɾ��pck�ж�Ӧ�ļ���ִ�к����ύ����, count ��Ҫɾ�����ļ�����
WINPCK_API PCKRTN		do_DeleteFromPck(LPCWSTR  szSrcPckFile, int count, ...);

//��ѯ��Ŀ¼���
//return = searched filecount
WINPCK_API DWORD		pck_searchByName(LPCWSTR  lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK showListCallback);
WINPCK_API DWORD		pck_listByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
WINPCK_API DWORD		do_listPathInPck(LPCWSTR  szSrcPckFile, LPCWSTR  lpszListPath, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

//���߳�����
//yes -1, no - 0
WINPCK_API BOOL			pck_isThreadWorking();
WINPCK_API void			pck_forceBreakThreadWorking();
WINPCK_API BOOL			pck_isLastOptSuccess();
WINPCK_API BOOL			pck_getLastErrorMsg();

//�ڴ�ռ��
WINPCK_API DWORD		pck_getMTMemoryUsed();
WINPCK_API DWORD		pck_getMTMaxMemory();
WINPCK_API void			pck_setMTMaxMemory(DWORD dwMTMaxMemoryInMB);
WINPCK_API DWORD		pck_getMaxMemoryAllowed();
//�߳���
WINPCK_API DWORD		pck_getMaxThreadUpperLimit();
WINPCK_API DWORD		pck_getMaxThread();
WINPCK_API void			pck_setMaxThread(DWORD dwThread);
//ѹ���ȼ�
WINPCK_API DWORD		pck_getMaxCompressLevel();
WINPCK_API DWORD		pck_getDefaultCompressLevel();
WINPCK_API DWORD		pck_getCompressLevel();
WINPCK_API void			pck_setCompressLevel(DWORD dwCompressLevel);
//����
WINPCK_API DWORD		pck_getUIProgress();
WINPCK_API void			pck_setUIProgress(DWORD dwUIProgress);
WINPCK_API DWORD		pck_getUIProgressUpper();


//���/�����ļ����ؽ���嵥
WINPCK_API DWORD		pck_getUpdateResult_OldFileCount();
WINPCK_API DWORD		pck_getUpdateResult_PrepareToAddFileCount();
WINPCK_API DWORD		pck_getUpdateResult_ChangedFileCount();
WINPCK_API DWORD		pck_getUpdateResult_DuplicateFileCount();
WINPCK_API DWORD		pck_getUpdateResult_FinalFileCount();

//��־
WINPCK_API void			log_regShowFunc(ShowLogW _ShowLogCallBack);
WINPCK_API const char	log_getLogLevelPrefix(int _loglevel);
WINPCK_API void			log_PrintA(const char chLevel, LPCSTR  _fmt, ...);
WINPCK_API void			log_PrintW(const char chLevel, LPCWSTR  _fmt, ...);

#ifdef UNICODE
#define log_Print log_PrintW
#else
#define log_Print log_PrintA
#endif

//�򿪡��رա���ԭ���¼�ע��
WINPCK_API void			pck_regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);

#endif //WINPCK_DLL_H


