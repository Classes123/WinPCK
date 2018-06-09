#pragma once
#include "PckClassBodyWriter.h"
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"
#include "PckClassThreadHelper.h"


class CPckClassAppendFiles :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassBodyWriter,
	public virtual CPckClassThreadHelper
{
public:
	CPckClassAppendFiles();
	~CPckClassAppendFiles();

	//�½�pck�ļ�
	//virtual BOOL	CreatePckFile(LPTSTR szPckFile, LPTSTR szPath);
	//�½�������pck�ļ�
	virtual BOOL	UpdatePckFile(LPCTSTR szPckFile, vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert);
private:

};

