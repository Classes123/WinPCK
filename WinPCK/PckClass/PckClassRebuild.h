#pragma once
#include "PckClassBodyWriter.h"
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"
#include "PckClassThreadHelper.h"

class CPckClassRebuild :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassBodyWriter,
	public virtual CPckClassThreadHelper
{
public:
	CPckClassRebuild();
	~CPckClassRebuild();

	//�ؽ�pck�ļ�
	virtual BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress);

protected:
	BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile);
	BOOL	RecompressPckFile(LPCTSTR szRecompressPckFile);

};

