#pragma once
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"
#include "PckClassThread.h"


class CPckClassWriteOperator :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassThreadWorker
{
public:
	CPckClassWriteOperator();
	~CPckClassWriteOperator();

	//�ؽ�pck�ļ�
	virtual BOOL	RebuildPckFile(LPCTSTR lpszScriptFile, LPCTSTR szRebuildPckFile, BOOL bUseRecompress);

protected:
	BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile);
	BOOL	RecompressPckFile(LPCTSTR szRecompressPckFile, int isStripMode = PCK_STRIP_NONE);

public:
#pragma region ��Ϸ����
	virtual BOOL	StripPck(LPCTSTR lpszStripedPckFile, int flag);
#pragma endregion

#pragma region PckClassAppendFiles.cpp
public:
	//�½�������pck�ļ�
	virtual BOOL	UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert);
#pragma endregion
#pragma region PckClassRenamer.cpp

public:
	//�������ļ�
	virtual BOOL	RenameFilename();

#pragma endregion
};

