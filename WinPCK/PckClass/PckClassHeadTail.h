#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"

class CPckClassHeadTail :
	protected virtual CPckClassBaseFeatures,
	protected virtual CPckClassZlib
{
public:
	CPckClassHeadTail();
	~CPckClassHeadTail();

	//���ø�����Ϣ
	char*	GetAdditionalInfo();
	BOOL	SetAdditionalInfo();

private:

};
