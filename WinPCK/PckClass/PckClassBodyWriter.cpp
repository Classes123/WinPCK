
#include "PckClassBodyWriter.h"

CPckClassBodyWriter::CPckClassBodyWriter()
{}

CPckClassBodyWriter::~CPckClassBodyWriter()
{}

//��ȡ����ѹ����Ĵ�С�����Դ��СС��һ��ֵ�Ͳ�ѹ��
DWORD CPckClassBodyWriter::GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize)
{
	if(PCK_BEGINCOMPRESS_SIZE < dwFileSize) {
		lpPckFileIndex->dwFileClearTextSize = dwFileSize;
		lpPckFileIndex->dwFileCipherTextSize = compressBound(dwFileSize);
	} else {
		lpPckFileIndex->dwFileCipherTextSize = lpPckFileIndex->dwFileClearTextSize = dwFileSize;
	}

	return lpPckFileIndex->dwFileCipherTextSize;
}