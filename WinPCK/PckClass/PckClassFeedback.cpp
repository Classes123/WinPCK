//////////////////////////////////////////////////////////////////////
// PckClassFeedback.cpp: ��Ϣ��������Ϣ��������Ե� 
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"
#include "PckControlCenter.h"

#define define_one_PrintLog(_loglevel)	\
void CPckClass::PrintLog##_loglevel(const char *_text, ...)\
{\
	va_list	ap;\
	va_start(ap, _text);\
	lpPckParams->lpPckControlCenter->PrintLog##_loglevel(_text, ap);\
	va_end(ap);\
}\
void CPckClass::PrintLog##_loglevel(const wchar_t *_text, ...)\
{\
	va_list	ap;\
	va_start(ap, _text);\
	lpPckParams->lpPckControlCenter->PrintLog##_loglevel(_text, ap);\
	va_end(ap);\
}\
void CPckClass::PrintLog##_loglevel(const char *_text, va_list ap)\
{\
	lpPckParams->lpPckControlCenter->PrintLog##_loglevel(_text, ap);\
}\
void CPckClass::PrintLog##_loglevel(const wchar_t *_text, va_list ap)\
{\
	lpPckParams->lpPckControlCenter->PrintLog##_loglevel(_text, ap);\
}

define_one_PrintLog(I);
define_one_PrintLog(W);
define_one_PrintLog(E);
define_one_PrintLog(D);
define_one_PrintLog(N);

void CPckClass::PrintLogEL(const char *_maintext, const char *_file, const char *_func, const long _line)
{
	lpPckParams->lpPckControlCenter->PrintLogEL(_maintext, _file, _func, _line);
}

void CPckClass::PrintLogEL(const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	lpPckParams->lpPckControlCenter->PrintLogEL(_maintext, _file, _func, _line);
}

void CPckClass::PrintLogEL(const char *_fmt, const char *_maintext, const char *_file, const char *_func, const long _line)
{
	lpPckParams->lpPckControlCenter->PrintLogEL(_fmt, _maintext, _file, _func, _line);
}

void CPckClass::PrintLogEL(const char *_fmt, const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	lpPckParams->lpPckControlCenter->PrintLogEL(_fmt, _maintext, _file, _func, _line);
}

void CPckClass::PrintLog(const char chLevel, const char *_fmt, va_list ap)
{
	lpPckParams->lpPckControlCenter->PrintLog(chLevel, _fmt, ap);
}

void CPckClass::PrintLog(const char chLevel, const wchar_t *_fmt, va_list ap)
{
	lpPckParams->lpPckControlCenter->PrintLog(chLevel, _fmt, ap);
}

void CPckClass::PrintLog(const char chLevel, const char *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	lpPckParams->lpPckControlCenter->PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckClass::PrintLog(const char chLevel, const wchar_t *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	lpPckParams->lpPckControlCenter->PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}


#undef define_one_PrintLog