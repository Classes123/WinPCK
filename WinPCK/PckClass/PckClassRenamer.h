#pragma once
#include "PckClassBodyWriter.h"
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"

class CPckClassRenamer :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassBodyWriter
{
public:
	CPckClassRenamer();
	~CPckClassRenamer();

	//�������ļ�
	virtual BOOL	RenameFilename();

private:

};

