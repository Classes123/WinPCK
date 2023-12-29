#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"

class CPckClassHeadTail :
	public virtual CPckClassBaseFeatures
{
public:
	CPckClassHeadTail();
	~CPckClassHeadTail();

	//Set extensions
	const char*	GetAdditionalInfo();
	BOOL	SetAdditionalInfo(const char* lpszAdditionalInfo);

private:

};
