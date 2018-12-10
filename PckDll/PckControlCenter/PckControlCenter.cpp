//////////////////////////////////////////////////////////////////////
// PckControlCenter.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "PckClass.h"
#include "PckClassLog.h"
#include <thread>
	
const DWORD	CPckControlCenter::m_dwNumberOfProcessors = thread::hardware_concurrency();

CPckControlCenter::CPckControlCenter():
	m_lpClassPck(NULL),
	m_lpPckRootNode(NULL),
	m_emunFileFormat(FMTPCK_UNKNOWN),
	cParams({ 0 })
{
	init();
	New();
}

CPckControlCenter::~CPckControlCenter()
{
	regMsgFeedback(NULL, DefaultFeedbackCallback);
	Close();
	uninit();
}

void CPckControlCenter::init()
{
	cParams.lpPckControlCenter = this;
	cParams.code_page = 936;
	cParams.dwCompressLevel = getDefaultCompressLevel();
	cParams.dwMTThread = m_dwNumberOfProcessors;
	cParams.dwMTMaxMemory = getMaxMemoryAllowed();

	m_lpPckLog = new CPckClassLog();
}

void CPckControlCenter::uninit()
{
	lpszFilePathToAdd.clear();
	delete m_lpPckLog;
}

void CPckControlCenter::Reset(DWORD dwUIProgressUpper)
{
	memset(&cParams.cVarParams, 0, sizeof(PCK_VARIETY_PARAMS));
	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
}

#pragma region ��־��ع���
//��־
void	CPckControlCenter::regShowFunc(ShowLogW _ShowLogW)
{
	CPckClassLog::PckClassLog_func_register(_ShowLogW);
}

const char CPckControlCenter::getLogLevelPrefix(int _loglevel)
{
	return CPckClassLog::GetLogLevelPrefix(_loglevel);
}

void CPckControlCenter::Print(const char chLevel, const char *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckControlCenter::Print(const char chLevel, const wchar_t *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckControlCenter::Print(const char chLevel, const char *_fmt, va_list ap)
{
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
}

void CPckControlCenter::Print(const char chLevel, const wchar_t *_fmt, va_list ap)
{
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
}
#pragma endregion