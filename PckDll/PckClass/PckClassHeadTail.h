#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"

class CPckClassHeadTail :
	protected virtual CPckClassBaseFeatures
{
public:
	CPckClassHeadTail();
	~CPckClassHeadTail();

	//���ø�����Ϣ
	const char*	GetAdditionalInfo();
	BOOL	SetAdditionalInfo(const char* lpszAdditionalInfo);

private:

};
