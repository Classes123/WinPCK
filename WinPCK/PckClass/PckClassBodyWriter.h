#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"
#include "MapViewFile.h"
#include "PckClassFileDisk.h"

class CPckClassBodyWriter :
	public virtual CPckClassBaseFeatures,
	public virtual CPckClassZlib,
	public virtual CPckClassFileDisk
{
public:
	CPckClassBodyWriter();
	~CPckClassBodyWriter();

protected:

	//��ȡ����ѹ����Ĵ�С�����Դ��СС��һ��ֵ�Ͳ�ѹ��
	DWORD GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize);

};

