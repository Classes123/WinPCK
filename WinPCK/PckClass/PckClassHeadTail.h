#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"

class CPckClassHeadTail :
	public virtual CPckClassBaseFeatures,
	public virtual CPckClassZlib
{
public:
	CPckClassHeadTail();
	~CPckClassHeadTail();

	//���ø�����Ϣ
	char*	GetAdditionalInfo();
	BOOL	SetAdditionalInfo();

private:

};
