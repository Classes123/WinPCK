//////////////////////////////////////////////////////////////////////
// CPckControlCenter.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma once

#include "pck_dependencies.h"

typedef struct _PCK_PATH_NODE * LPPCK_PATH_NODE;
typedef struct _PCK_RUNTIME_PARAMS * LPPCK_RUNTIME_PARAMS;

#if 0
#ifdef _WINDLL	//.dll
#define EXPORT_CLASS _declspec(dllexport) 
#elif defined(_DLL)	//.exe
#define EXPORT_CLASS _declspec(dllimport)
#else	//other
#define EXPORT_CLASS 
#endif
#else
#define EXPORT_CLASS 
#endif

class CPckClass;
class CPckClassLog;

class EXPORT_CLASS CPckControlCenter
{
	//����
public:
	CPckControlCenter();
	virtual ~CPckControlCenter();

	void	New();

private:

	void	init();
	void	uninit();
	void	Reset(DWORD dwUIProgressUpper = 1);
#pragma region �򿪹ر��ļ�

	//BOOL	Open(LPCTSTR lpszFile, BOOL isOpenAfterRestore);
	void	Close();
public:

	BOOL	Open(LPCTSTR lpszFile);

#pragma endregion

#pragma region �������ڵ�

	//������һ���ڵ�
	BOOL	RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString);
	//�ύ
	BOOL	RenameSubmit();

#pragma endregion

#pragma region Ԥ����ѹ�ļ�

	//Ԥ���ļ�
	BOOL		GetSingleFileData(LPCENTRY lpFileEntry, char *buffer, size_t sizeOfBuffer = 0);

	//��ѹ�ļ�
	BOOL		ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, LPCWSTR lpszDestDirectory);
	BOOL		ExtractAllFiles(LPCWSTR lpszDestDirectory);

#pragma endregion

#pragma region �ؽ�pck�ļ�
	//�ؽ�pck�ļ�
	BOOL	TestScript(LPCWSTR lpszScriptFile);
	BOOL	RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress);

#pragma endregion

#pragma region ��Ϸ����
	BOOL	StripPck(LPCWSTR lpszStripedPckFile, int flag);
#pragma endregion



#pragma region create or update pck file

	//����ļ��б����ѹ��ʱʹ�õ�vector����
	void	StringArrayReset();
	void	StringArrayAppend(LPCWSTR lpszFilePath);
	BOOL	UpdatePckFileSubmit(LPCWSTR szPckFile, LPCENTRY lpFileEntry);

#pragma endregion

#pragma region ɾ���ڵ�
	//ɾ��һ���ڵ�
	BOOL	DeleteEntry(LPCENTRY lpFileEntry);
	//�ύ
	BOOL	DeleteEntrySubmit();

#pragma endregion


#pragma region �汾���

	int		GetPckVersion();
	BOOL	SetPckVersion(int verID);
	//��ȡ��ǰ��������
	LPCWSTR	GetCurrentVersionName();
	static DWORD	GetVersionCount();
	static LPCWSTR	GetVersionNameById(int verID);
	static int		AddVersionAlgorithmId(int AlgorithmId, int Version);

#pragma endregion

#pragma region �ڵ����Բ���

	LPCENTRY GetRootNode();
	//��ȡnode·��
	static BOOL			GetCurrentNodeString(LPWSTR szCurrentNodePathString, LPCENTRY lpFileEntry);
	LPCENTRY			GetFileEntryByPath(LPCWSTR _in_szCurrentNodePathString);
#pragma endregion

#pragma region pck�ļ�����

	BOOL				IsValidPck();

	//��ȡ�ļ���
	DWORD				GetPckFileCount();
	//ʵ���ļ���С
	QWORD				GetPckSize();
	QWORD				GetPckDataAreaSize();
	QWORD				GetPckRedundancyDataSize();

	static QWORD		GetFileSizeInEntry(LPCENTRY lpFileEntry);
	static QWORD		GetCompressedSizeInEntry(LPCENTRY lpFileEntry);
	static DWORD		GetFoldersCountInEntry(LPCENTRY lpFileEntry);
	static DWORD		GetFilesCountInEntry(LPCENTRY lpFileEntry);

	static size_t		GetFilelenBytesOfEntry(LPCENTRY lpFileEntry);
	static size_t		GetFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);

	static QWORD		GetFileOffset(LPCENTRY lpFileEntry);

	//���ø�����Ϣ
	const char*			GetAdditionalInfo();
	static DWORD		GetAdditionalInfoMaxSize();
	BOOL				SetAdditionalInfo(LPCSTR lpszAdditionalInfo);

	//�Ƿ���֧�ָ��µ��ļ�
	BOOL				isSupportAddFileToPck();

#pragma endregion

	//�򿪡��رա���ԭ���¼�ע��
	static void		regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);
	static int		DefaultFeedbackCallback(void* pTag, int eventId, WPARAM wParam, LPARAM lParam);

#pragma region ��ѯ��Ŀ¼���
private:
	static void		DefaultShowFilelistCallback(void* _in_param, int sn, LPCWSTR lpszFilename, int entry_type, unsigned __int64 qwFileSize, unsigned __int64 qwFileSizeCompressed, void* fileEntry);

public:
	DWORD			SearchByName(LPCWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
	static DWORD	ListByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

#pragma endregion

#pragma region �߳̿���
	//�߳����в���
	BOOL			isThreadWorking();
	void			ForceBreakThreadWorking();
#pragma endregion

	//error no
	int				GetLastErrorMsg();
	BOOL			isLastOptSuccess();

#pragma region �ڴ�ռ��
	//�ڴ�ռ��
	DWORD			getMTMemoryUsed();
	//�ڴ�ֵ
	void			setMTMaxMemory(DWORD dwMTMaxMemory);
	DWORD			getMTMaxMemory();

	//����ڴ�
	static DWORD	getMaxMemoryAllowed();

#pragma endregion

#pragma region �߳���
	//�߳���
	DWORD	getMaxThread();
	void	setMaxThread(DWORD dwThread);
	//�߳�Ĭ�ϲ���
	static DWORD	getMaxThreadUpperLimit();
#pragma endregion

#pragma region ѹ���ȼ�

	//ѹ���ȼ�
	DWORD	getCompressLevel();
	void	setCompressLevel(DWORD dwCompressLevel = Z_DEFAULT_COMPRESS_LEVEL);

	//ѹ���ȼ�Ĭ�ϲ���
	static DWORD	getMaxCompressLevel();
	static DWORD	getDefaultCompressLevel();
#pragma endregion

#pragma region �������

	DWORD	getUIProgress();
	void	setUIProgress(DWORD dwUIProgress);
	DWORD	getUIProgressUpper();

#pragma endregion


	//��־
	static void	regShowFunc(ShowLogW _ShowLogW);
	static const char	getLogLevelPrefix(int _loglevel);
	static void	Print(const char chLevel, LPCSTR _fmt, ...);
	static void	Print(const char chLevel, LPCWSTR _fmt, ...);
	static void	Print(const char chLevel, LPCSTR _fmt, va_list ap);
	static void	Print(const char chLevel, LPCWSTR _fmt, va_list ap);
#pragma endregion



#pragma region ��ӡ���/�����ļ����

	DWORD	GetUpdateResult_OldFileCount();
	DWORD	GetUpdateResult_PrepareToAddFileCount();
	DWORD	GetUpdateResult_ChangedFileCount();
	DWORD	GetUpdateResult_DuplicateFileCount();
	DWORD	GetUpdateResult_FinalFileCount();

#pragma endregion

private:

	FMTPCK	GetPckTypeFromFilename(LPCTSTR lpszFile);

	LPPCK_PATH_NODE				m_lpPckRootNode;
	vector<wstring>				lpszFilePathToAdd;	//����Ӷ���ļ�ʱ�ṩ����

	wstring						szUpdateResultString;

	PCK_RUNTIME_PARAMS			cParams;
	CPckClass					*m_lpClassPck;
	CPckClassLog				*m_lpPckLog;

	//��ʽ
	FMTPCK						m_emunFileFormat;

	static FeedbackCallback		pFeedbackCallBack;
	static void*				pTag;

};