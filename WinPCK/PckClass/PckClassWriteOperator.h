#pragma once
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"
#include "PckClassThread.h"

typedef enum { OP_CheckFile = 0, OP_Protect, OP_Delete, OP_Rmdir, OP_Unknown } SCRIPTOP;

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
	//_FILEOPS * next;
}FILEOP;

class CPckClassWriteOperator :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassThreadWorker
{
public:
	CPckClassWriteOperator();
	~CPckClassWriteOperator();

	//�ؽ�pck�ļ�
	virtual BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress);

protected:
	BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile);
	BOOL	RecompressPckFile(LPCTSTR szRecompressPckFile);

#pragma region PckClassRebuildFilter.cpp
public:
	BOOL	ParseScript(LPCTSTR lpszScriptFile);
private:
	const char *szOperators = 
		"chkfile;"
		"protect;"
		"delete;;"
		"rmdir;;;";
	BOOL	ParseScript(LPCTSTR lpszScriptFile, PCK_ALL_INFOS &_DstPckAllInfo);
	void	ResetRebuildFilterInIndexList(PCK_ALL_INFOS &_DstPckAllInfo);
	BOOL	ApplyScript2IndexList(VOID *pfirstFileOp, PCK_ALL_INFOS _DstPckAllInfo);
	void	MarkFilterFlagToNode(LPPCK_PATH_NODE lpNode, SCRIPTOP op);
	void	MarkFilterFlagToFileIndex(LPPCKINDEXTABLE	lpPckIndexTable, SCRIPTOP op);
	LPPCK_PATH_NODE LocationFileIndex(LPWSTR *lpszPaths, LPPCK_PATH_NODE lpNode);
	void	SepratePaths(FILEOP * pFileOp);
	BOOL	ParseOneLine(FILEOP * pFileOp, char * lpszLine, TCHAR *lpszFileName);
	VOID	GetNextLine(SCRIPTBUFFER *sfvBuf, CHAR * szLineAnsi, CONST UINT uiLengthLine, UINT * puiStringLength, BOOL * pbErrorOccured, BOOL * pbEndOfFile);
	BOOL	ReadFromBuffer(void *_dstbuf, size_t size, SCRIPTBUFFER *sfvBuf);
	void	SetBufPointer(SCRIPTBUFFER *sfvBuf, int pos);
#pragma endregion

#pragma region PckClassAppendFiles.cpp
public:
	//�½�������pck�ļ�
	virtual BOOL	UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert);
#pragma endregion
#pragma region PckClassRenamer.cpp

public:
	//�������ļ�
	virtual BOOL	RenameFilename();

#pragma endregion
};

